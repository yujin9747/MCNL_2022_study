#include <iostream>
#include <vector>
using namespace std;

/*
고윤민 교수님께서 제공해 주신 교재 783pg에 나오는 가장 basic한 trie 구현
=> 메모리 절약 신경 쓰지 않고 빠른 search를 위해 문자 개수 만큼의 포인터 공간을 할당 

0. toNumber
1. insert
2. find

*/

const int ALPHABET = 26;

int toNumber(char c){
    return c-'a';
}

/*
trieNode
    - 자식 노드를 가리키는 포인터 목록 : 동적  X, 고정 배열 
    - 종료 노드인지 표현하는 booleam 형 변수

    들어올 수 있는 모든 문자의 갯수 만큼 고정 배열 할당 -> 메모리 측면에서 비효율적
        => 다음 노드를 찾는 시간이 빠른 것이 장점
    
    constructor 1. 종료 노드인 경우 : children 삭제 -> 새로운 노드를 만들 때 사용되는 constructor이므로 children 삭제.
    constructor 2. 종료 노드가 아닌 경우 : children의 모든 값을 null값으로 채우기 -> 앞으로 다음 노드를 찾아 연결해야 하므로
    
*/
struct trieNode{
    bool terminal;
    trieNode* children[ALPHABET];

    trieNode() : terminal(false){ // terminal의 기본 값은 false
        memset(children, 0, sizeof(children)); // 자식 노드 포인터에 메모리 할당 및 null 초기화
    }
    ~trieNode(){
        for(int i=0; i<ALPHABET; i++){
            if(children[i]) 
                delete children[i];
        }
    }

    void insert(const string str){
        int index; 
        trieNode* child;
        trieNode* curr = this;
        for(int i=0; i<str.length(); i++){
            index = toNumber(str[i]);
            // 자식 노드를 새로 생성해야 하는 경우
            if(curr->children[index] == NULL){
                child = new trieNode();
                curr->children[index] = child;
                printf("new : %c\n", str[i]);
            }
            else{
                printf("exist : %c\n", str[i]);
            }
            curr = curr->children[index]; // 다음 자식 노드를 연결하기 위해 포인터 이동
            if(i==str.length()-1) {
                curr->terminal = true;
                printf("terminal\n");
            }
        } 
    }

    bool find(const string str){
        int index;
        int flag=0;
        trieNode* curr = this;
        for(int i=0; i<str.length(); i++){
            index=toNumber(str[i]);
            if(curr->children[index] == NULL){
                return false; // 1) 단어 길이 내에서 다음 문자가 존재하지 않는 경우
                              // 2) 존재하는 단어보다 더 긴단어를 찾으려 할 경우 -> ex. story 삽입 후, storyy 찾으려 할 때
            }
            curr = curr->children[index];
            //if(i == str.length()-1 && curr->terminal) return true; // 3) 마지막 글자까지 존재하고 terminal이 true인 경우
            if(i == str.length()-1) flag = 1;
        }
        //return false; 
        // 4) 마지막 글자까지 존재하나, terminal이 true가 아닌 경우 -> ex, story 삽입 후 sto가 존재하는지 찾으려 할 때

        // 만약 case 4를 true로 다루고 싶다면, terminal 조건을 확인하지 않고 true 반환하기
        if(flag == 1) return true;
        else return false;
    }

};

int main(){
    trieNode trie;
    bool found;
    string str;
    while(1){
        cout << "Wort to insert(소문자만 입력, q : insert 종료)\n";
        cin >> str; // 한 단어만 입력받아 insert , 소문자만 입력 한다고 가정
        if(str.compare("q")==0) break;
        trie.insert(str);
    }

    while(1){
        cout << "Wort to find(소문자만 입력, q : find 종료)\n";
        cin >> str; // 한 단어만 입력받아 find, 소문자만 입력 한다고 가정
        if(str.compare("q")==0) break;
        found = trie.find(str);
        if(found) printf("found\n");
        else printf("not found\n");
    }

    return 0;
}