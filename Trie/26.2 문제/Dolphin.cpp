#include <iostream>
using namespace std;
const int ALPHABET = 26;

int toNumber(char c){
    return c-'a';
}
struct trieNode {
    bool terminal;
    int freq;
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

    void insert(const string str, int freq){
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
                curr->freq = freq;
                printf("terminal\n");
            }
        } 
    }

    int matching(trieNode* curr){
        int maximum = 0, ret;
        if(curr->terminal) maximum = curr->freq;

        for(int i=0; i<ALPHABET; i++){
            if(curr->children[i] != NULL){
                ret =  matching(curr->children[i]);
                if(ret > maximum) {
                    maximum = ret;
                }
            }
        }

        return maximun;
    }



    int find(const string str){
        int index, times = 0;
        trieNode* curr = this;
        int autoMatched;
        for(int i=0; i<str.length(); i++){
            index = toNumber(str[i]);
            // 다음 글자가 존재하지 않는 경우
            if(curr->children[index] == NULL){
                return times;
            }
            // 다음 글자가 존재하는 경우
            curr->children[index];

            autoMatched = matching(curr);
            times++;
        }
        
        return times;
    }
};

int main(){
    string str;
    trieNode dictionary;
    int testcase, N, M, freq, times;

    cout << "Enter the number of test cases => ";
    cin >> testcase;

    for(int i=0; i<testcase; i++){
        times = 0;
        cout << "Enter the number of words in the dictionary => ";
        cin >> N;
        cout << "Enter the number of words for the dolphin to type => ";
        cin >> M;
        cout << "Enter N number of words(lower case only) and frequencies below(1-10)\n";
        for(int j=0; j<N; j++){
            cin >> str;
            cin >> freq;
            dictionary.insert(str, freq);
        }

        cout << "Enter M number of words(lower case only) below\n";
        for(int j=0; j<M; j++){
            cin >> str;
            times = dictionary.find(str);
            times++; // 공백 타이핑 개수 세기
        }

        printf("Result : %d times\n", times);
    }
    
    return 0;
}