Assignment 2 Writeup
=============

My name: [이명률]

My POVIS ID: [mlleee]

My student ID (numeric): [20210750]

This assignment took me about [12] hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the TCPReceiver and wrap/unwrap routines:
[
    1. wrap / unwrap
    wrap(absolute seqno->seqno)은 매우 단순한 구현으로 생략합니다.
    unwrap(seqno->absolute seqno)은 data structure에서 다룬 'circular queue'를 연상케 합니다. 이와 유사하게 접근했습니다.
    다만, 변환 과정에서 32bit -> 64bit로 확장되어 주어지는 checkpoint에 근접한 값을 도출해야 합니다.
    주어지는 변수로 syn에서 얼마나 떨어져있는지 index를 구하고, checkpoint를 상위 32bit, 하위 32bit로
    나누어 하위 32bit와 index의 차이가 2^31보다 큰지 작은지에 따라 처리해 줍니다. 이유는 seqno의 크기가 최대 2^32이기 때문입니다.

    2. TCPReceiver
    TCPSegment는 TCPHeader와 payload로 구성됩니다.
    우선 먼저 쉬운 것부터 접근했는데, syn을 수신해야 ackno가 생성됩니다. ackno는 first unassembled이므로 이에 따라 설정을 하고,
    fin flag 또한 공간을 차지하므로 1을 더해줍니다. 이로써 `ackno()`를 작성할 수 있고 window_size() 또한 assn2_description을 참고해
    작성할 수 있습니다.
    `segment_received()`는 TCP 수신을 구현합니다.
    syn 수신 이전 패킷을 모두 무시하고 수신하게 되면 syn_flag를 설정하고 isn도 마찬가지로 설정해줍니다.
    이후에는 수신받은 데이터를 stream_reassembler에 보내 재조립할 수 있도록 합니다.
    마지막에 fin을 수신하게 되면 fin_flag를 설정하고 종료합니다.
]

Implementation Challenges:
[
    1. unwrap 구현
    unwrap이 'circular queue'와 유사하다는 점을 깨달았지만, 구현 과정에서 checkpoint도 고려해야 한다는 점이
    까다롭게 다가왔습니다. 결국에 seqno가 크기가 2^32라는 것에서 영감받아 구현을 완료할 수 있었습니다.
    
    2. tcp_receiver.cc
    구현은 어렵지 않았지만, tcp receiver의 동작을 이해하는 데 시간을 많이 보냈습니다.
]

Remaining Bugs:
[
    `unwrap()`에서 difference가 정확히 2^31일 때 양방향 처리가 가능한 case가 존재할 수 있습니다.
    과제 설명에는 따로 명시되어 있지 않지만 동작 중에 오류를 불러일으킬 수 있습니다.
]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
