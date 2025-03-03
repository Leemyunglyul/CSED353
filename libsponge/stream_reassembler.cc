#include "stream_reassembler.hh"

#include <limits>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity), _capacity(capacity), storage{}, _eof_idx(numeric_limits<size_t>::max()), _output_idx(0) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    set_eof(data, index, eof);

    try {
        const pair<size_t, string> _truncate_string_to_output = truncate_string_to_output(data, index);

        const string _truncate_data_to_output = _truncate_string_to_output.second;
        const size_t idx = _truncate_string_to_output.first + index;

        if (idx > _output_idx) {
            insert_data(_truncate_data_to_output, idx);
        } else if (idx == _output_idx) {
            const string _data = truncate_string_to_capacity(_truncate_data_to_output, idx);
            _output_idx += _output.write(_data);

            reassemble_storage_data();
        }

        if (empty() && _output_idx == _eof_idx) {
            _output.end_input();
        }

    } catch (const exception &e) {
        return;
    }
}

void StreamReassembler::set_eof(const string &data, const size_t index, const bool eof) {
    if (eof)
        _eof_idx = index + data.size();
}

pair<size_t, string> StreamReassembler::truncate_string_to_output(const string &data, const size_t index) {
    // data 기준 새로운 시작점 index
    const size_t start_idx = _output_idx > index ? _output_idx - index : 0;

    if (start_idx > data.size())
        throw std::out_of_range("Invalid Index");

    return {start_idx, data.substr(start_idx)};
}

string StreamReassembler::truncate_string_to_capacity(const string &data, const size_t index) const {
    const size_t available_space = _capacity - _output.buffer_size() - unassembled_bytes();
    const size_t left_space =
        _capacity + _output.bytes_read() - index > 0 ? _capacity + _output.bytes_read() - index : 0;

    const size_t trunc_len = min(data.size(), min(available_space, left_space));

    return data.substr(0, trunc_len);
}

void StreamReassembler::reassemble_storage_data() {
    for (auto it = storage.begin(); it != storage.end();) {
        const size_t idx = it->first;

        if (idx < _output_idx) {
            const string data = it->second;
            it = storage.erase(it);

            if (idx + data.size() > _output_idx) {
                const string trunc_data = data.substr(_output_idx - idx);
                const size_t trunc_idx = _output_idx;

                insert_data(trunc_data, trunc_idx);
            }
        } else
            it++;
    }

    auto it = storage.find(_output_idx);

    if (it == storage.end())
        return;

    _output_idx += _output.write(it->second);
    storage.erase(it);

    reassemble_storage_data();
}

void StreamReassembler::insert_data(const string &data, const size_t index) {
    string _data = data;
    size_t start_idx = index;
    size_t end_idx = index + data.size() - 1;

    for (auto it = storage.begin(); it != storage.end();) {
        const string cur_data = it->second;
        const size_t cur_start_idx = it->first;
        const size_t cur_end_idx = it->first + cur_data.size() - 1;

        if (start_idx > cur_end_idx || cur_start_idx > end_idx)
            it++;
        else if (cur_start_idx <= start_idx && end_idx <= cur_start_idx)
            return;
        else if (start_idx < cur_start_idx && cur_end_idx < end_idx)
            it = storage.erase(it);
        else {
            if (start_idx <= cur_start_idx) {
                _data += cur_data.substr(end_idx + 1 - cur_start_idx, cur_end_idx - end_idx);
                end_idx = start_idx + _data.size() - 1;
            } else {
                start_idx = cur_start_idx;
                _data.insert(0, cur_data.substr(0, start_idx - cur_start_idx));
            }
            it = storage.erase(it);
        }
    }

    if (!_data.empty()) {
        storage.insert({index, truncate_string_to_capacity(_data, index)});
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    size_t sum = 0;

    for (auto it = storage.begin(); it != storage.end(); it++)
        sum += it->second.size();

    return sum;
}

bool StreamReassembler::empty() const { return storage.empty(); }
