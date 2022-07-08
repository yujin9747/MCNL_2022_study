#include <stdio.h>
/*
구간 합 segment tree
tree 는 array로 관리해 공간 효율성을 높인다

1. init
2. sum
3. update

< 실행 기본 설정 >
array size : 14
구간 : 3-9
update : 3 index 값을 +4

*/

int array[14] = {5, 3, 3, 4, 2, 1, 6, 8, 9, 7, 7, 1, 2, 3}; // total : 61
int tree[14*4]; 

/*
배열 이용해 tree 구조 초기화
각 구간 노드의 합 구하기

start : 시작 구간 
end : 끝 구간
node : tree 인덱스 -> 1부터 시작 : 자식 노드 구할 때 2를 곱해야 하므로

실행 예시:
if
    tree의 leaf인 0, 0 구간 노드에 도달 했을 시, 해당 노드의 값에 배열의 index 0인 값을 저장 후 반환(int)
else
    두 구간으로 나누어 init 재귀 호출 -> 반환되는 두 값 더해서 반환(int)
*/
int init(int start, int end, int node){
    if(start == end) return tree[node] = array[start];
    int mid = (start + end)/2 ;
    return tree[node] = init(start, mid, node*2) + init(mid+1, end, node*2+1);
}

/*
주어진 구간 내 합 구하기 -> 재귀 호출 이용

start, end : 현재 노드 구간
left, right : 주어진 구간 -> 변하지 않는 값
node : tree 내 현재 노드 위치 -> 마찬가지로 첫 호출에 1부터 시작

실행 예시 :
if 
    구간이 전혀 겹치지 않는 경우 : return 0
if 
    구간이 완전히 포함되는 경우(ex. 현재 노드 구간이 4-7이고 주어진 구간이 1-7인 경우) : return tree[node]
else
    두 구간으로 나눠 재귀 호출
*/
int sum(int start, int end, int node, int left, int right){
    if(left > end || right < start) return 0;
    if(left <= start && right >= end) return tree[node];
    int mid = (start + end)/2;
    return sum(start, mid, node*2, left, right) + sum(mid+1, end, node*2+1, left, right);
}

/*
특정 원소의 값을 수정
-> 만약 array index 7인 값을 -3만큼 수정함다면, 7을 포함하는 tree의 모든 구간 노드의 값을 변경
-> 재귀 호출 이용

start, end : 노드의 구간
node : 현재 tree 내 위치
index : 수정할 값의 배열 index
dif : 기존 값과 수정할 값의 차

실행 예시 :
if
    index가 구간에 포함되지 않는 경우 기존 값 return;
else
    if start == end => array[index]값 변경, return tree[node] = array[index];
    else 두 구간으로 나누어 더하고, tree[node]에 새로운 값 저장 후 반환
*/
int update(int start, int end, int node, int index, int dif){
    if(end < index || start > index) return tree[node];
    if(start == end && start == index) {
        array[index] += dif;
        return tree[node] = array[index];
    }
    int mid = (start + end)/2;
    return tree[node] = update(start, mid, node*2, index, dif) + update(mid+1, end, node*2+1, index, dif);
}



int main(int argc, char* argv[]){
    int total = init(0, 13, 1); // total : 전체 구간 합
    int result = sum(0, 13, 1, 3, 9); // result : 주어진 구간 합 결과
    int newTotal = update(0, 13, 1, 3, 4); // newTotal : 4+4=8로 바꾼 후, 새로운 total 합
    int newResult = sum(0, 13, 1, 3, 9); // newResult : 새로운 구간 합 결과
    printf("전체 구간 합 : %d\n", total); // 61
    printf("3~9구간 합 : %d\n",result); // 37
    printf("newTotal : %d\n", newTotal); // 65
    printf("새로운 3~9구간 합: %d\n", newResult); // 41
    return 0;
}