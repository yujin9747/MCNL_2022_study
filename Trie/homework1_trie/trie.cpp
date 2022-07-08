/*
trie를 이용한 자동 완성 추천 기능 구현
root->search(문자열, priority_queue)를 통해 검색 횟수 기준 내림 차순 자동 추천 작동 확인 완료

root->search("poh", pq);
for(int i=0; i<10; i++){
    if(pq.empty()) break;
    printf( "%s\n", pq.top().word);
    pq.pop();
}

아래와 같은 실시간 입력 및 출력을 시도 했으나 잘 되지 않는 상태 -> 다시 시도 해보기
Search work : "타이핑 공간"
------------------------
단어 1
단어 2
단어 3
.
.
.
*/

#include <iostream>
#include <cstdio>
#include <cctype>
#include <string.h>
#include <unistd.h>
#include <curses.h>
#include <queue>
#include <curses.h>
#include <termios.h>
#include <ncurses.h>

using namespace std;

const int ALPHABETS = 26; // 대소문자 구분 X, 공백 제외
int toNumber(char c) {return tolower(c)-'a';} // convert to index

// 현재까지 입력한 단어로 검색할 수 있는 단어에 대한 구조체
struct TopSearched{
    int freq;
    char* word;
    TopSearched(int f, char* w) : freq(f), word(w) {}
};

// freq를 기준으로 큰 것이 priority queue의 top을 유지하도록 해주는 operator
struct cmp {
    bool operator()(TopSearched a, TopSearched b){
        return a.freq < b.freq;
    }
};

void insert (char *m, char *s, int n)
{
	memmove(m+n, m, strlen(m));
	memmove(m+n, s, strlen(s));
}

struct TrieNode{
    TrieNode* children[ALPHABETS];
    bool terminal;
    int freq;
    char word[100];

    TrieNode() : terminal(false) {
        memset(children, 0, sizeof(children)); // TrieNode 포인터 배열 nullptr 초기화
    }
    ~TrieNode(){
        for(int i=0; i<ALPHABETS; i++){
            if(children[i]) delete children[i]; // TrieNode 포인터에 대한 메모리 반환
        }
    }

    // 재귀 호출 하며 마지막 문자까지 children[] 포인터 배열로 연결
    void insert(const char* c, int f, queue<char> insertWord){
        if(*c == 0) {
            terminal = true; 
            freq = f;
            int n = insertWord.size();
            for(int i=0; i<n; i++) {
                char ch = insertWord.front();
                word[i] = ch;
                insertWord.pop();
            }
            return;
        }
        else {
            insertWord.push(*c);
            int index = toNumber(*c);
            if(children[index] == NULL) children[index] = new TrieNode();
            children[index]->insert(c+1, f, insertWord);
        }
    }

    // c와 대응되는 노드 찾은 경우 반환 -> 종료 노드 아니어도 반환
    TrieNode* find(const char* c){
        if(*c == 0) return this; // trie set안에 해당 문자열이 없는 경우
        int index = toNumber(*c);
        if(children[index] == NULL) return NULL;
        return children[index]->find(c+1); // c의 마지막 문자를 만날 때까지 재귀 호출
    }

    void traversal(TrieNode* next, priority_queue<TopSearched, vector<TopSearched>, cmp> &pq){
        if(next->terminal){ // leaf node인 경우
            TopSearched leaf(next->freq, next->word);
            pq.push(leaf);
        }
        for(int i=0; i<26; i++){
            if(next->children[i]) traversal(next->children[i], pq);
        }
    }

    // traversal 위한 인터페이스
    void search(const char* typed, priority_queue<TopSearched, vector<TopSearched>, cmp> &pq){
        TrieNode* typedNode = find(typed);
        traversal(typedNode, pq);
    }
};

char* DeleteChar(char s[], char ch)
{
	int i, k = 0;
    char str[strlen(s)];
	for (i = 0; i < strlen(s); i++)
		if (s[i] != ch) str[k++] = s[i];

    for(int i=0; i<strlen(s); i++) {
        if(i<k) s[i] = str[i];
        else s[i] = '\0';
    }
	return s;
}

int main(int argc, char* argv[]){
    TrieNode* root = new TrieNode();
    int freq;
    FILE* fp = fopen("data.txt", "r"); // read mode로 파일 오픈

    if (fp == NULL) {
        perror("fopen failed");
        return 1;
    }

    // trie 구성
    queue<char> insertWord;
    while(!feof(fp)){
        char data[100] = {0};
        fscanf(fp, "%[^0123456789]", data);
        DeleteChar(data, ' ');

        fscanf(fp, "%d", &freq); // 숫자만 입력
        fgetc(fp); // 개행문자 

        root->insert(data, freq, insertWord);
    }

    priority_queue<TopSearched, vector<TopSearched>, cmp> pq;
        root->search("poh", pq);
        for(int i=0; i<10; i++){
            if(pq.empty()) break;
            printf( "%s\n", pq.top().word);
            pq.pop();
        }
        root->search("a", pq);
        for(int i=0; i<10; i++){
            if(pq.empty()) break;
            printf( "%s\n", pq.top().word);
            pq.pop();
        }

        root->search("ap", pq);
        for(int i=0; i<10; i++){
            if(pq.empty()) break;
            printf( "%s\n", pq.top().word);
            pq.pop();
        }

        

    
    
    fclose(fp); // 파일 close
    return 0;
}