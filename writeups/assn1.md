Assignment 1 Writeup
=============

My name: [이명률]

My POVIS ID: [mlleee]

My student ID (numeric): [20210750]

This assignment took me about [14] hours to do (including the time on studying, designing, and writing the code).

Program Structure and Design of the StreamReassembler:
[

    이번 과제의 목표는 **Stream Reassembler** 구현이었습니다. 이전 assn0에서 구현한 'ByteStream'에 대한 이해는 반드시
    선행되어야 했고 특히 이 ByteStream의 작은 조각들을 올바른 순서대로 재조립해야 했습니다.

    `StreamReassembler`의 멤버 변수는 기존의 `_output`, `_capacity` 이외에 c++ STL Map을 활용한 `storage`, `_eof_idx`,
    `_output_idx`를 추가로 선언했습니다.
    
    1. `storage`: key를 데이터의 시작 인덱스, value를 실제 데이터로 두었습니다. 비슷한 `set<pair>`도 고려해보았지만,
    **unique key**가 유용해 최종적으로 Map으로 구현했습니다.

    2. `_eof_idx`, `_output_idx`: 데이터의 끝을 가리키는 eof 인덱스는 필수적이었고, 구현 중에 output이 현재 어디까지
    읽혔는지 표현하는 인덱스도 필요하여 구현하게 되었습니다.

    `StreamReassembler`의 method를 구현할 때 최대한 각 method가 최대 한 가지 일만 하도록 주의했습니다. 또한, 구현 과정에서
    추가로 구현이 필요한 기능들도 있었는데, 기존에 주어진 interface만 외부에 노출되도록 private method로 구현하였습니다.

    1. `void set_eof(const string &data, const size_t index, const bool eof)`: `_eof_idx` 설정.

    2. `pair<size_t, string> truncate_string_to_output(const string &data, const size_t index) const`:
    현재 읽힌 ByteStream Index에 맞게 입력을 조정합니다.

    3. `string truncate_string_to_capacity(const string &data, const size_t index) const`:
    용량에 따른 여유 공간과 ByteStream이 처리할 수 있는 경계를 계산하여 입력을 조정합니다.

    4. `void StreamReassembler::reassemble_storage_data()`: Unassembled data에 대해 재조립을 진행합니다.

    5. `void insert_data(const string &data, const size_t index)`:
    `storage`에 input data를 적절히 처리하여 겹치지 않도록 저장합니다.

    단순한 `size_t unassembled_bytes() const`, `bool empty() const`는 제외하고 `void push_substring(const std::string &data, const uint64_t index, const bool eof)`는 위의 private method를 조합하여 구현하였습니다.
]

Implementation Challenges:
[

    구현하는 데 있어 가장 큰 어려운 부분은 크게 두 가지였습니다.

    첫 번째는, `truncate_string_to_capacity`에서 `left_space`였습니다.
    처음에는 단순하게 assn1.pdf에서 소개된 `_capacity - _output.buffer_size() - unassembled_bytes()`만을 계산하여
    구현하려 했으나 실패했습니다. 곰곰이 생각해보니 index >> capacity 와 같은 경우, valid 여부와 관계없이 메모리만 차지하여 
    원래 들어와야 할 input이 못 들어오는 상황이 발생할 수 있었습니다.

    두 번째는, `insert_data`였습니다.
    기존에 storage에 저장된 data와 겹치지 않게 저장해야 했습니다. 그러기 위해서는 겹치는지 여부를 판단하고 겹치는 경우에는
    기존 data와의 병합이 필요했습니다. 최초에 구현한 logic에는 문제가 없어 보였으나 계속 실패하여 다시 갈아엎고
    각 case가 겹치지 않게 구현하였습니다.

]

Remaining Bugs:
[

    1. Storage에 empty string을 삽입.
    `insert_data`에서 for문 이후에 삽입하고 있는데, 빈 문자열인지 확인을 하지 않고 있어 의미없이 메모리를 낭비할 수 있다.

    2. reassemble_storage_data: Recursion
    재귀적으로 자기 자신을 호출하고 있는데, 잠재적으로 stackoverflow를 야기할 수도 있다.

    3. truncate_string_to_capacity: Overflow
    available space를 계산할 때, 음수 임을 확인하지 않고 있는데 자칫 overflow로 매우 큰 양수를 대입할 수 있다.

]

- Optional: I had unexpected difficulty with: [lab0에서는 깃허브 레포지토리를 private 레포지토리로 fork를 하였다면
이번 lab1에서는 원본 레포지토리가 새로운 브랜치를 생성하였습니다. 그래서 새로운 브랜치를 가져온 후 기존 branch 내용과 병합을
하는 과정이 필요했는데 관련해서 배울 수 있었던 시간이었습니다.]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
