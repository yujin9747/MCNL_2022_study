#include <iostream>
#include <vector>
#include <algorithm>
#include <limits.h>
// segment tree 내용 중, 교재 743pg의 예제 전까지 따라 온 상태
// 글을 읽고 코드를 거의 보지 않고 코딩
// 중간 중간 맞는지 확인하며 3개의 기능 구현

using namespace std;
int N = 11;
/*
RMQ : range minimum query
구간의 최소 값을 미리 구해 놓는 segment tree 방식

구조체 이용
- RMQ 구조체
    a. properties
        - 배열 길이
        - 각 구간의 최소치
    b. functions
        - constructor
        - init
        - query
        - update
*/

struct RMQ {
    int n;
    vector<int> rangeMin;
    RMQ(const vector<int> &array){
        n = array.size();
        rangeMin.resize(n*4);
        init(array, 0, n-1, 1);
    }
    /*
    array, start, end, node 
    재귀 호출 이용
    leaf에 도달했을 경우 array안의 해당 값을 tree의 노드 값으로 저장 후 return 
    두 구간의 최소치 중 더 작은 값을 선택해 현재 구간의 최소치로 계산
    */
    int init(const vector<int> &array, int start, int end, int node){
        if(start == end) return rangeMin[node] = array.at(start); // case 1
        int mid = (start + end)/2;
        int leftMin = init(array, start, mid, node*2);
        int rightMin = init(array, mid+1, end, node*2+1);
        return rangeMin[node] = min(leftMin, rightMin); // case 2
    }

    /*
    초기화 이후, 임의의 구간의 최소치를 구할 때 사용하는 함수
    순회 이용
    query(left, right, node, nodeLeft, nodeRight)
    현재 node가 표현하는 범위 : nodeLeft, nodeRight
    임의의 구간 : left, right
    두 구간의 교집합의 최소치를 반환

    ex. 임의의 구간 [1,3] , 현재 노드 구간 [3, 3]일 때 교집합인 3, 3 노드에 저장된 최소치 반환(int)

    case 1. 공집합
        -  반환값이 무시되도록 아주 큰 값 설정
    case 2. 교집합이 [nodeLeft, nodeRight]
        - 임의의 구간 안에 노드의 구간이 포함되는 경우니, 해당 노드의 값 그대로 반환
    case 3. else
        - 재귀 호출, 둘 중 더 작은 값 반환
    */
    int query(int left, int right){ // 인터페이스
        return query(left, right, 1, 0, N-1);
    }
    int query(int left, int right, int node, int nodeLeft, int nodeRight){
        if(right < nodeLeft || left > nodeRight) return INT_MAX; 
        if(left <= nodeLeft && right >= nodeRight) return rangeMin[node];
        int mid = (nodeLeft + nodeRight)/2;
        return min(query(left, right, node*2, nodeLeft, mid), query(left, right, node*2+1, mid+1, nodeRight));
    }

    /*
    array의 index위치의 값이 newValue로 바뀌었을 경우 사용
    해당 index가 포함된 모든 구간 노드마다 다시 계산
    init과 query를 응용해 구현

    case 1. leaf 도달 & index 값 하나만 들은 구간
    case 2. index가 해당 구간에 포함되지 않는 경우
    case 3. index가 해당 구간에 포함되는 경우
    */
    int update(int nodeLeft, int nodeRight, int node, int index, int newValue){
        if(nodeLeft == nodeRight && nodeLeft == index) return rangeMin[node] = newValue;
        if(index < nodeLeft || index > nodeRight) return rangeMin[node]; 
        int mid = (nodeLeft + nodeRight)/2;
        return rangeMin[node] = min(update(nodeLeft, mid, node*2, index, newValue), update(mid+1, nodeRight, node*2+1, index, newValue));
    }
    int update(int index, int newValue){ // 인터페이스
        return update(0, N-1, 1, index, newValue);
    }

};


/*
RangeRequest : 임의의 구간에서 가장 출현 빈도가 높은 값을 미리 계산해두는 segment tree에 관한 구조체
-> 후에 추가적으로 시도해 보기

    a. properties
        - 구간 크기
        - 가장 자주 등장하는 횟수
        - 왼쪽 끝 숫자, 왼쪽 끝 숫자의 빈도
        - 오른쪽 끝 숫자, 오른쪽 끝 숫자의 빈도
    b. functions
        - constructor
        - merge

struct RangeResult {
    int size;
    int mostFrequency;
    int left;
    int leftFrequency;
    int right;
    int rightFrequency;

};

*/

int main(int argc, char* argv[]){
    vector<int> vec = {0, 0, 1, 1, 1, 1, 2, 3, 3, 3, 4};

    RMQ rmq(vec); // O(n) -> 각 노드 마다 걸리는 시간 1, n 길이의 배열에 대한 노드도 O(n)
    
    // query test : O(lgn)
    int range1 = rmq.query(1, 2);
    printf("Range 1 [1,2] : %d\n", range1); // 0

    int range2 = rmq.query(2, 5);
    printf("Range 2 [2,5] : %d\n", range2); // 1

    int range3 = rmq.query(2, 6);
    printf("Range 3 [2,6] : %d\n", range3); // 1

    int range4 = rmq.query(9, 10);
    printf("Range 4 [9,10] : %d\n", range4); // 3

    int range5 = rmq.query(0, 10);
    printf("Range 5 [0,10] : %d\n\n", range5); // 0


    // update test : O(lgn)
    vec[2] = 8;
    int update1 = rmq.update(3, 0);
    range2 = rmq.query(2, 5);
    printf("Update 1 index 3(1 -> 0) & range[2,5] : %d\n", range2); // 0

    return 0;
}