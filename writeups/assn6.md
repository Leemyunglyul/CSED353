Assignment 6 Writeup
=============

My name: [이명률]

My POVIS ID: [mlleee]

My student ID (numeric): [20210750]

This assignment took me about [5] hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the Router:
[
    이번 과제 router 구현의 핵심은 `route_one_datagram()`과 `routing_table`이다.

    `route_one_datagram()`은 pdf에 써있는대로 구현을 하였다.
    먼저, longest-prefix-match route를 찾고 없는 경우와 TTL이 1이하인 경우 drop하였다. 
    그렇지 않은 경우에는, datagram을 next hop으로 전송하였다.

    `routing_table`은 `add_route()`에서 받은 정보를 바탕으로 entry를 생성하되, mask만 미리 계산하여 table에 추가했다.

]

Implementation Challenges:
[
    이번 과제는 구현이 어렵지 않았는데, 꽤 구현에 시간이 들었던 것은 longest prefix match였다.
    수업 중에 배우긴 하였지만, 실제 코드로 구현할 때 어떻게 구현할지 고민을 했었다.
    
    이전 랩에서 wrapping_intergers 구현할 때도 비슷한 문제에 닥쳤기에 
    그때의 경험을 살려 적절한 비트연산으로 해결할 수 있었다.
]

Remaining Bugs:
[
    현재 routing_table 상으로는 중복된 entry에 대해 처리하는 logic이 존재하지 않아,
    어떤 entry가 추가되었는지에 따라 라우팅 동작이 달라질 수 있는 버그가 존재한다.
]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
