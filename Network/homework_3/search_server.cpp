#include <iostream>
#include <cstdio>
#include <cctype>
#include <string.h>
#include <unistd.h>
#include <queue>
#include <sstream>

#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

#define BUF_SIZE 1024;
const int ALPHABETS = 26; // 대소문자 구분 X, 공백 제외
int toNumber(char c) {return tolower(c)-'a';} // convert to index

// 현재까지 입력한 단어로 검색할 수 있는 단어에 대한 구조체
struct TopSearched{
    int freq;
    char* word;
    TopSearched* next;
    TopSearched() : freq(0), word(0), next(nullptr) {}
    TopSearched(int f, char* w) : freq(f), word(w), next(nullptr) {}
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
    void search(const char* c, priority_queue<TopSearched, vector<TopSearched>, cmp> &pq){
        TrieNode* found = find(c);
        if(found != nullptr) traversal(found, pq);
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
    // --------------------------- < part 1. 소켓 생성 및 연결 > ----------------------------------

    struct sockaddr_in addr;                    // IPv4 주소 정보를 담을 구조체
    struct sockaddr_in client_addr;
    char* serv_port = argv[1];                  // port번호 문자열 선언

    memset(&addr, 0, sizeof(addr));             // addr구조체의 모든 번수 0으로 초기화
    addr.sin_family = AF_INET;                  // 주소체계 지정
    addr.sin_addr.s_addr = htonl(INADDR_ANY);   // 현재 실행중인 컴퓨터 IP를 소켓에 부여할 때 INADDR_ANY 사용
    addr.sin_port = htons(atoi(serv_port));     // 문자열 기반의 port번호 초기화

    int tcp_socket = socket(PF_INET, SOCK_STREAM, 0);           // 연결지향형 소켓 생성
    bind(tcp_socket, (struct sockaddr*)&addr, sizeof(addr));    // 서버 소켓에 주소를 할당
    listen(tcp_socket, 5);                                      // queue의 크기를 5
    socklen_t size = sizeof(client_addr);
    int accepted = accept(tcp_socket, (struct sockaddr*)&client_addr, &size);  // 양방향 수신 가능 상태로 변환


    // --------------------------- < part 2.  trie 구성 > ---------------------------------------

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

    fclose(fp); // 파일 close

    // ---------------- < part 3. 클라이언트와 소통 : 검색어 입력받기 & 추천검색어 보내기 > -------------------
    priority_queue<TopSearched, vector<TopSearched>, cmp> pq;
    char typedWord[1024];
    int recv_length = read(accepted, &typedWord,1023);
    typedWord[recv_length] = 0;
    while(recv_length>0){
        root->search(typedWord, pq);                        // pq에 모든 추천 단어 저장
        int i=0;
        string array[10];
        while(i<10 && !pq.empty()){
            TopSearched word = pq.top();
            pq.pop();
            array[i] = word.word;
            i++;
        }
        i=0;
        while(i<10){
            cout << array[i] << " ";
            i++;
        }
        write(accepted, array, sizeof(array));       // 추천 단어를 담은 queue를 client에게 보낸다
        cout << "after sending\n";
        recv_length=read(accepted, &typedWord, 1024);    // 계속해서 단어를 입력받는다
    }
    close(accepted);
    close(tcp_socket);
    return 0;
}