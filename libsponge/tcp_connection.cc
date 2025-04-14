#include "tcp_connection.hh"

#include <iostream>

// Dummy implementation of a TCP connection

// For Lab 4, please replace with a real implementation that passes the
// automated checks run by `make check`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

size_t TCPConnection::remaining_outbound_capacity() const { return _sender.stream_in().remaining_capacity(); }

size_t TCPConnection::bytes_in_flight() const { return _sender.bytes_in_flight(); }

size_t TCPConnection::unassembled_bytes() const { return _receiver.unassembled_bytes(); }

size_t TCPConnection::time_since_last_segment_received() const { return _time_since_last_segment_received; }

void TCPConnection::segment_received(const TCPSegment &seg) {
    if (!_active) return;

    _time_since_last_segment_received = 0;

    if (seg.header().rst) {
        _receiver.stream_out().set_error();
        _receiver.stream_out().error();
        _sender.stream_in().set_error();
        _active = false;
        return;
    }

    _receiver.segment_received(seg);

    if (seg.header().syn && _sender.next_seqno_absolute() == 0) {
        _sender.send_empty_segment();
        _sender.fill_window();
        send_sender_segments();
    }

    if (seg.header().ack) {
        _sender.ack_received(seg.header().ackno, seg.header().win);
    }

    if (_receiver.ackno().has_value() 
        && seg.length_in_sequence_space() == 0
        && seg.header().seqno == _receiver.ackno().value() - 1) {
        _sender.send_empty_segment();
        send_sender_segments();
    }


    if (seg.length_in_sequence_space() > 0 && _sender.segments_out().empty()) {
        _sender.send_empty_segment();
        send_sender_segments();
    }

    if (_receiver.stream_out().eof() && !_sender.stream_in().eof()) {
        _linger_after_streams_finish = false;
    }

}

bool TCPConnection::active() const {
    return _active && !(
        _receiver.stream_out().eof() &&
        _sender.stream_in().eof() &&
        _sender.bytes_in_flight() == 0 &&
        (!_linger_after_streams_finish || 
         (_time_since_last_segment_received >= 10 * _cfg.rt_timeout))
    );
}

size_t TCPConnection::write(const string &data) {
    if(data.size() == 0) return 0;
    size_t written = _sender.stream_in().write(data);
    _sender.fill_window();
    send_sender_segments();
    return written;
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) {
    if (!active()) return;

    // tell the TCPSender about the passage of time. 
    _time_since_last_segment_received += ms_since_last_tick;
    _sender.tick(ms_since_last_tick);
    
    if (_sender.consecutive_retransmissions() > TCPConfig::MAX_RETX_ATTEMPTS) {
        send_rst();
        return;
    }

    // 정상 종료 조건 확인
    if (_receiver.stream_out().eof() && 
        _sender.stream_in().eof() &&
        _sender.bytes_in_flight() == 0) {
        if (!_linger_after_streams_finish || 
            _time_since_last_segment_received >= 10 * _cfg.rt_timeout) {
            _active = false;
        }
    }

    send_sender_segments();

}

void TCPConnection::end_input_stream() {
    _sender.stream_in().end_input();
    _sender.fill_window();
    send_sender_segments();
}

void TCPConnection::connect() {
    _sender.fill_window();
    if (_sender.segments_out().empty()) {
        _sender.send_empty_segment();
    }
    send_sender_segments();
}

void TCPConnection::send_sender_segments() {
    while (!_sender.segments_out().empty()) {
        TCPSegment seg = _sender.segments_out().front();
        _sender.segments_out().pop();

        if (_receiver.ackno().has_value()) {
            seg.header().ack = true;
            seg.header().ackno = _receiver.ackno().value();
            seg.header().win = static_cast<uint16_t>(
                min(_receiver.window_size(), 
                    static_cast<size_t>(numeric_limits<uint16_t>::max()))
            );
        }

        _segments_out.push(seg);
    }
}

void TCPConnection::send_rst() {
    TCPSegment rst_seg;
    
    rst_seg.header().rst = true;

    if (_receiver.ackno().has_value()) {
        rst_seg.header().ack = true;
        rst_seg.header().ackno = _receiver.ackno().value();
    }

    while (!_segments_out.empty()) {
        _segments_out.pop(); 
    }

    _segments_out.push(rst_seg);
    
    _active = false;
    
    _sender.stream_in().set_error();
    _receiver.stream_out().set_error();
}

TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            cerr << "Warning: Unclean shutdown of TCPConnection\n";

            // Your code here: need to send a RST segment to the peer
            send_rst();
        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}
