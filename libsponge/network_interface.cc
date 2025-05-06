#include "network_interface.hh"

#include "arp_message.hh"
#include "ethernet_frame.hh"

#include <iostream>

// Dummy implementation of a network interface
// Translates from {IP datagram, next hop address} to link-layer frame, and from link-layer frame to IP datagram

// For Lab 5, please replace with a real implementation that passes the
// automated checks run by `make check_lab5`.

// You will need to add private members to the class declaration in `network_interface.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface(const EthernetAddress &ethernet_address, const Address &ip_address)
    : _ethernet_address(ethernet_address), _ip_address(ip_address) {
    cerr << "DEBUG: Network interface has Ethernet address " << to_string(_ethernet_address) << " and IP address "
         << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but may also be another host if directly connected to the same network as the destination)
//! (Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) with the Address::ipv4_numeric() method.)
void NetworkInterface::send_datagram(const InternetDatagram &dgram, const Address &next_hop) {
    // convert IP address of next hop to raw 32-bit representation (used in ARP header)
    const uint32_t next_hop_ip = next_hop.ipv4_numeric();

    auto it = arp_cache.find(next_hop_ip);
    if (it != arp_cache.end()) {
        EthernetFrame frame;
        frame.header().src = _ethernet_address;
        frame.header().dst = it->second.first;
        frame.header().type = EthernetHeader::TYPE_IPv4;
        frame.payload() = dgram.serialize();
        _frames_out.push(frame);
        return;
    }

    waiting_datagrams.push({dgram, next_hop_ip});

    size_t now = _time_passed;
    if (arp_request_time.count(next_hop_ip) == 0 || now - arp_request_time[next_hop_ip] >= 5000) {
        ARPMessage arp_req;
        arp_req.opcode = ARPMessage::OPCODE_REQUEST;
        arp_req.sender_ethernet_address = _ethernet_address;
        arp_req.sender_ip_address = _ip_address.ipv4_numeric();
        arp_req.target_ip_address = next_hop_ip;

        EthernetFrame frame;
        frame.header().src = _ethernet_address;
        frame.header().dst = ETHERNET_BROADCAST;
        frame.header().type = EthernetHeader::TYPE_ARP;
        frame.payload() = arp_req.serialize();

        _frames_out.push(frame);

        arp_request_time[next_hop_ip] = now;
    }
}

//! \param[in] frame the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame(const EthernetFrame &frame) {
    if (frame.header().dst != _ethernet_address && frame.header().dst != ETHERNET_BROADCAST)
        return {};

    if (frame.header().type == EthernetHeader::TYPE_ARP) {
        ARPMessage arp;
        if (arp.parse(frame.payload()) != ParseResult::NoError)
            return {};

        if (arp.opcode == ARPMessage::OPCODE_REQUEST && arp.target_ip_address == _ip_address.ipv4_numeric()) {
            ARPMessage reply;
            reply.opcode = ARPMessage::OPCODE_REPLY;
            reply.sender_ethernet_address = _ethernet_address;
            reply.sender_ip_address = _ip_address.ipv4_numeric();
            reply.target_ethernet_address = arp.sender_ethernet_address;
            reply.target_ip_address = arp.sender_ip_address;

            EthernetFrame reply_frame;
            reply_frame.header().src = _ethernet_address;
            reply_frame.header().dst = arp.sender_ethernet_address;
            reply_frame.header().type = EthernetHeader::TYPE_ARP;
            reply_frame.payload() = reply.serialize();
            _frames_out.push(reply_frame);
        }

        arp_cache[arp.sender_ip_address] = {arp.sender_ethernet_address, 30000};

        queue<pair<InternetDatagram, uint32_t>> remain;
        while (!waiting_datagrams.empty()) {
            auto [dgram, ip] = waiting_datagrams.front();
            waiting_datagrams.pop();
            if (ip == arp.sender_ip_address) {
                EthernetFrame f;
                f.header().src = _ethernet_address;
                f.header().dst = arp.sender_ethernet_address;
                f.header().type = EthernetHeader::TYPE_IPv4;
                f.payload() = dgram.serialize();
                _frames_out.push(f);
            } else {
                remain.push({dgram, ip});
            }
        }
        swap(waiting_datagrams, remain);

        return {};
    }

    if (frame.header().type == EthernetHeader::TYPE_IPv4) {
        InternetDatagram dgram;
        if (dgram.parse(frame.payload()) == ParseResult::NoError)
            return dgram;
    }

    return {};
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick(const size_t ms_since_last_tick) {
    _time_passed += ms_since_last_tick;

    vector<uint32_t> expired;
    for (auto it : arp_cache) {
        if (it.second.second <= ms_since_last_tick)
            expired.push_back(it.first);
        else
            it.second.second -= ms_since_last_tick;
    }
    for (auto ip : expired)
        arp_cache.erase(ip);
}
