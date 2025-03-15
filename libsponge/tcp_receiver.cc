#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader hdr = seg.header();

    if (!_syn_flag) {
        if (!hdr.syn)
            return;
        _isn = hdr.seqno;
        _syn_flag = true;
    }

    const uint64_t checkpoint = _reassembler.stream_out().bytes_written() + 1;
    const uint64_t absolute_seqno = unwrap(hdr.seqno, _isn, checkpoint);

    const uint64_t stream_idx = absolute_seqno - 1 + hdr.syn;

    _reassembler.push_substring(seg.payload().copy(), stream_idx, hdr.fin);

    if (hdr.fin || _fin_flag) {
        _fin_flag = true;
        if (_reassembler.unassembled_bytes() == 0)
            _reassembler.stream_out().end_input();
    }
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!_syn_flag)
        return nullopt;

    uint64_t index = _reassembler.output_idx() + 1;
    if (_reassembler.stream_out().input_ended())
        index++;
    return wrap(index, _isn);
}

size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size(); }
