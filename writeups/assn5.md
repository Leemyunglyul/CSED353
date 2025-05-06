Assignment 5 Writeup
=============

My name: [이명률]

My POVIS ID: [mlleee]

My student ID (numeric): [20210750]

This assignment took me about [11] hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the NetworkInterface:
[
    program structure는 과제 pdf에 명시되어 있다.

    ARP 캐시 관리
    + `arp_cache` 맵을 사용해 IP 주소를 MAC 주소 및 TTL과 매핑
    + 수신된 ARP 응답으로 캐시 갱신, 30초 TTL 적용
    + `tick()`에서 주기적으로 TTL 감소 및 만료된 항목 제거

    프레임 전송
    + 목적지 MAC이 캐시에 있으면 즉시 이더넷 프레임 생성
    + 캐시 미스 시 `waiting_datagrams` 큐에 저장
    + ARP 요청은 5초 간격으로 재전송 제한.

    ARP 요청/응답 처리
    + ARP 요청 수신 시 대상 IP가 자신인 경우 즉시 응답 전송
    + ARP 응답 수신 시 캐시 갱신 및 대기중인 데이터그램 처리

    시간 관리
    + `_time_passed`로 시간 추적
    + `tick()`에서 시스템 시간 갱신 및 캐시 TTL 관리

    위의 logic으로 프로그램을 설계하였다.
]

Implementation Challenges:
[
    Design은 이미 pdf에서 잘 명시되어 있었기 때문에 이번 과제 구현 같은 경우 적절한 data structure를 찾는 게 중요했다.
    
    arp_cache, arp_request_time 같은 경우 구현 방법은 자유롭겠지만, map이
    생각하기로는 시간복잡도가 작아서 선택했다. 이외에도 struct를 pair 대신 사용할 수 있었겠지만, 구현의 편의를 위해 pair를 선택했다. 

    waiting_datagrams는 frames_out과 마찬가지로 queue로 구현했다.

    이번 과제에서 구현의 어려움은 많이 없었고, 어떤 data structure를 선택할지에
    시간을 할애하였다.
    
]

Remaining Bugs:
[
    1. `waiting_datagrams.push({dgram, next_hop_ip});`
    ARP 응답 미수신 시 데이터그램이 큐에서 무한대기하는 문제가 발생할 수 있다.
    이에 대한 error handling이 실제 환경에서는 추가적으로 필요할 수 있다.

    2. `frame.header().dst = ETHERNET_BROADCAST;`
    과도한 ARP 요청으로 네트워크 혼잡 가능성이 있다.
]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
