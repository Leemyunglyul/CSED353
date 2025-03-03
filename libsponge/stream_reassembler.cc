#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) 
    : _output(capacity), _capacity(capacity), _unassembled_bytes(0), buffer("") {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {

    size_t len_push = min(data.size(), _capacity - _unassembled_bytes - buffer.size());
    
    storage.insert({index})

}

size_t StreamReassembler::unassembled_bytes() const { 
    
    size_t sum = 0;

    for(auto it = storage.begin();  it != storage.end(); it++)
        sum += it->second.size();
    
    return sum; 
}

bool StreamReassembler::empty() const { return storage.empty(); }
