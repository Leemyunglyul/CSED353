Assignment 4 Writeup
=============

My name: [이명률]

My POVIS ID: [mlleee]

My student ID (numeric): [20210750]

This assignment took me about [18] hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Your benchmark results (without reordering, with reordering): [1.89, 1.00]

Program Structure and Design of the TCPConnection:
[
    사실 Program structure와 Design 관련해서는 assn4.pdf의 '3. Assignment 4: The TCP connection'과
    '5. The end of a TCP connection: consensus takes work'에 자세히 설명되어 있습니다. 몇 가지 에러 처리를 제외하고는
    위의 설명을 코드로 옮겨적은 것에 불과합니다.

    TCP connection은 TCP 연결의 전체 상태를 관리하는 객체로, TCP sender와 TCP receiver를 포함합니다.
    _active, _linger_after_streams_finish 등으로 상태 flag를 관리합니다.
    외부에서 세그먼트가 도착하면 segment_received()에서 전적으로 관리하고 이벤트는 tick()에서 처리합니다.
    예외 상황 처리 관련해서는 send_rst()에서 처리합니다.

    특히 이번 어싸인의 핵심 중 하나인 'the end of TCP Connection' 관련해서 Unclean Shutdown, Clean Shutdown 구현은
    해당 선행조건 4가지를 if문으로 구현하였습니다.
]

Implementation Challenges:
[
    이번 TCPConnection 구현은 생각보다 수월했던 것이 assn4.pdf에서 구현에 요구되는 부분을
    거의 다 설명을 해주고 있어 구현을 하는 데는 어렵지 않았습니다.

    하지만, 이전 어싸인에서 구현한 tcp_receiver에서 에러 처리가 부족하여 오히려 발목을 잡았습니다.
    TCPConnection 상에서는 문제가 되지 않았지만, tcp_receiver.cc에서
    syn 패킷이 아님에도, absolute_seqno가 0인 case를 handling하지 않아 계속 TC를 실패했습니다.
    TC 실패 원인이 tcp_receiver와 관련되어 있음을 깨닫고 나서야 눈치챘습니다.
    실제 make check_lab2를 실행하여도 문제는 없었지만 TC가 훨씬 더 많은 make check_lab4에
    와서야 감지되어 어려움을 겪었습니다.
]

Remaining Bugs:
[
    현재 assn4.md를 적는 와중에도 테스트를 돌려보고 있는데, 80번대를 넘어가는 TC의 경우 계속 그 
    번호는 조금씩 바뀌어도 3~4개의 TC를 실패하고 있습니다. 분명 전날에는 잘 되었다가도 다음날 아침에
    돌려보면 TC가 실패하였습니다. 기숙사에서 VPN을 통해 해서 그런지, 아니면 CPU 처리가 문제인지 몰라도
    알 수 없는 원인으로 아직 잔재해있는 버그가 있습니다. 대부분의 실패 원인이 Timeout이라는 점에서
    위의 두 가지 원인 중 하나이지 않을까 생각합니다. 혹은 제 TCPConnection 구현 상 문제가 있을 수도 있습니다.
]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
