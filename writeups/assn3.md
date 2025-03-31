Assignment 3 Writeup
=============

My name: [이명률]

My POVIS ID: [mlleee]

My student ID (numeric): [20210750]

This assignment took me about [11] hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the TCPSender:
[
    이번 과제에서 중요한 method로는 간단한 함수들을 제외하고
    `tick()`, `ack_received()`, `fill_window()`가 있다.

    `tick()`에는 실제 재전송 메커니즘이 담겨있다. exponential backoff로,
    재전송시 RTO를 2배 증가시켜 네트워크 혼잡을 완화한다.

    `ack_received()`는 absolute_seqno를 기준으로 이미 확인된 세그먼트는
    정리하고 확인되지 않은 세그먼트 만을 남긴다. 만약 확인된 세그먼트가 있으면
    재전송 logic을 초기화한다.

    `fill_window()`는 TCP sender의 주 목적이라고도 할 수 있다. 수신자의
    윈도우를 채우는데, 최대한 많이 보내야 한다. 보내기 전에 syn과 fin flag를
    보낼지 말지 결정하고 loop문을 통해 보낼 수 있을 때까지 보낸다.
]

Implementation Challenges:
[
    여러가지 구현의 어려움이 있었지만, 두 가지가 기억에 남는다.

    첫 번째는 fill_window()이다. 수신자의 윈도우 크기를 고려하여 가능한 한 많이 보내야 했는데
    구현 도중 테스트를 돌리는데 멈추는 경우가 빈번히 발생했다. 분명 loop가 계속 도는 것이었는데
    나중에 해결하고 보니 크기가 0인 segment를 고려하지 않아서였다.

    두 번째는 tick()이다. if (_receiver_window_size > 0) 처음에는 이 조건을 넣지 않았더니, 
    계속 테스트가 실패하였다. 과제 설명에서는 수신자 윈도우 크기가 0일지라도 1인 것처럼 취급하라고
    하여 그런 줄 알았지만 사실 0이면 0으로 취급하는 게 맞았다. 따라서 재전송에 포함하지 않고
    처리하는 게 맞았다.
]

Remaining Bugs:
[
    이 과제를 구현하기 단순하게 하기 위해 선행된 가정들이 몇몇 있었다.

    'What do I do if an acknowledgment only partially acknowledges some outstanding segment? Should I try to clip off the bytes that got acknowledged?'

    'If I send three individual segments containing “a,” “b,” and “c”, and they never get acknowledged, can I later retransmit them in one big segment that contains “abc”? Or do I have to retransmit each segment individually?'

    실제 TCP는 처리해야 하지만 이 과제에서는 그러지 않았다. 이와 같은 edge case에 대해서는 처리가 불가하다.
]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
