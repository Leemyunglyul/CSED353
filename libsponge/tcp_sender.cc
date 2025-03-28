#include "tcp_sender.hh"
#include "tcp_config.hh"
#include <random>

using namespace std;

TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity)
    , _next_seqno{0}
    , _timer_running{false}
    , _time_passed{0}
    , _bytes_in_flight{0}
    , _consecutive_retransmissions{0}
    , _retransmission_timeout{retx_timeout}
    , _receiver_window_size{1}
    , _fin_sent{false}
    , _outstanding_segments{} {}
    

uint64_t TCPSender::bytes_in_flight() const { return _bytes_in_flight; }

void TCPSender::fill_window() {
    uint16_t window_size = max(static_cast<uint16_t>(1), _receiver_window_size);
    
    while (window_size > _bytes_in_flight) {
        TCPSegment segment;
        segment.header().seqno = wrap(_next_seqno, _isn);

        if (_next_seqno == 0) {
            segment.header().syn = true;
        }

        size_t payload_size = min(TCPConfig::MAX_PAYLOAD_SIZE, 
                                  static_cast<size_t>(window_size - _bytes_in_flight - segment.header().syn));
        segment.payload() = Buffer(_stream.read(payload_size));

        bool should_send_fin = _stream.eof() && !_fin_sent && 
                               (window_size - _bytes_in_flight) > segment.length_in_sequence_space();

        if (should_send_fin) {
            segment.header().fin = true;
            _fin_sent = true;
        }

        if (segment.length_in_sequence_space() == 0) {
            return;
        }

        _segments_out.push(segment);
        _outstanding_segments.push(segment);
        _bytes_in_flight += segment.length_in_sequence_space();
        _next_seqno += segment.length_in_sequence_space();

        if (!_timer_running) {
            _timer_running = true;
            _time_passed = 0;
        }

        if (segment.header().fin) {
            return;
        }
    }
}

void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) { 
    uint64_t absolute_ackno = unwrap(ackno, _isn, _next_seqno);
    
    if (absolute_ackno > _next_seqno) return;

    _receiver_window_size = window_size;

    bool segments_acked = false;
    while (!_outstanding_segments.empty()) {
        TCPSegment &seg = _outstanding_segments.front();
        uint64_t seg_end_seqno = unwrap(seg.header().seqno, _isn, _next_seqno) + seg.length_in_sequence_space();

        if (seg_end_seqno <= absolute_ackno) {
            _bytes_in_flight -= seg.length_in_sequence_space();
            _outstanding_segments.pop();
            segments_acked = true;
        } else {
            break;
        }
    }

    if (segments_acked) {
        _retransmission_timeout = _initial_retransmission_timeout;
        _consecutive_retransmissions = 0;
        if (!_outstanding_segments.empty()) {
            _timer_running = true;
            _time_passed = 0;
        } else {
            _timer_running = false;
        }
    }

    if (_stream.eof() && _bytes_in_flight == 0 && _fin_sent) {
        _timer_running = false;
    }

    fill_window();
}

void TCPSender::tick(const size_t ms_since_last_tick) { 
    if (!_timer_running) return;

    _time_passed += ms_since_last_tick;

    if (_time_passed >= _retransmission_timeout) {
        if (!_outstanding_segments.empty()) {
            _segments_out.push(_outstanding_segments.front());
            if (_receiver_window_size > 0) {
                _consecutive_retransmissions++;
                _retransmission_timeout *= 2;
            }
        }
        _time_passed = 0;
    }
}

unsigned int TCPSender::consecutive_retransmissions() const { return _consecutive_retransmissions; }

void TCPSender::send_empty_segment() {
    TCPSegment empty_segment;
    empty_segment.header().seqno = wrap(_next_seqno, _isn);
    _segments_out.push(empty_segment);
}
