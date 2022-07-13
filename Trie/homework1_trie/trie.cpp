/*
실시간 입출력 구현 완료
!! 2단계(첫 단어 외 중간/끝에 있는 경우 추천) 구현 못 한 상태 !!
*/
#include <iostream>
#include <cstdio>
#include <cctype>
#include <string.h>
#include <unistd.h>
#include <queue>
#include <ncurses.h>
#include <sstream>

using namespace std;

const int ALPHABETS = 26; // 대소문자 구분 X, 공백 제외
int toNumber(char c) {return tolower(c)-'a';} // convert to index

void initialize_ncurses() {
    initscr();                          // initializes ncurses
    if(has_colors() == false) {         // checks whether user's terminal supports colors
        endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	}
    use_default_colors();               // use default terminal color
    start_color();                      // start color
    init_pair(1, COLOR_CYAN, -1);       // define color pair for highlight, -1 means to use default
    cbreak();                           // Line buffering disabled
    noecho();                           // no print out on screen
    // keypad(stdscr, true);            // ???

    // default screen print
    mvprintw(0, 0, "(Press ESC to quit)");    // 화면의 0행, 0열부터 출력
    mvprintw(1, 0, "Search Word: ");
    mvprintw(2, 0, "---------------------------------------------");
    move(1,13);                         // input cursor position
    refresh();                          // 화면에 출력하도록 합니다
}

void terminate_ncurses() {
    endwin();                           // restores terminal
}

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
    priority_queue<TopSearched, vector<TopSearched>, cmp> pq;
    initialize_ncurses();
    char ch;
    string queryStr = "", currentWord, tempWord;

    while(true) {
        ch = getch(); // 한 글자 타이핑 받기
        if(ch == '\x1B') break; // esc 입력 시 종료
        
        if('a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || ch == ' ') {
            queryStr.push_back(ch); // 현재까지 타이핑 한 단어에 방금 입력한 글자 추가
        }
        else if(ch == '\x7F') {  // 현재까지 타이핑 한 단어에서 마지막 글자 삭제
            if(queryStr.length() != 0)
                queryStr.pop_back();
        }
        else continue; // 알파벳, backspace, esc 이외에 다른 글자를 쳤을 경우

        for(istringstream is(queryStr) ; is >> tempWord ; ) { // 현재까지 타이핑 한 글자를 currentWord에 저장
            currentWord = tempWord;
        }
        if(queryStr.length() == 0) currentWord.clear(); // 현재까지 입력된 글자가 없을 경우 currentWord 비우기

        mvprintw(1, 13, "%s", queryStr.c_str());        // Search Word 라인에서 오른쪽으로 13만큼 이동한 곳에 현재까지 입력한 단어 출력
                                                        // ex . Search Word: pl
        clrtoeol();                                     // from current position, clear to end of line
        while(!pq.empty()) pq.pop(); // 10개 이상으로 들어있던 값들 제거
        if(queryStr.length() != 0) root->search(queryStr.c_str(), pq); // pq에 모든 추천 단어 저장

        move(3,0); // ---- 밑 출력 부분의 첫 위치로 이동
        for(int i=0; i<10; i++) { // pq에 저장된 상위 10개의 단어만 출력
            if(pq.empty()) break; // 10개 미만으로 들어있을 경우, pq가 비면 중단

            move(3+i, 0); // 10개 중 몇 번째 출력인지에 따라 출력 line 변경
            attron(COLOR_PAIR(1)); // 색상 설정
            string word = pq.top().word;
            int k;
            for(k=0; k<queryStr.length(); k++) printw("%c", word[k]); // 타이핑 한 글자까지 색상을 넣어서 출력
            attrset(A_NORMAL); // 색상 속성 제거
            for(; k<word.length(); k++) printw("%c", word[k]); // 남은 글자들은 원래 색상으로 출력
            printw("\n");
            
            pq.pop(); // 출력한 단어 pueue에서 삭제                  
            clrtoeol();                                 // clear to end of line
        }
        clrtobot();                                     // clear from cursor until the last line
        move(1,13+queryStr.length()); // Search Word: 라인에서 13 + 타이핑한 글자 길이 좌표로 이동
        while(!pq.empty()) pq.pop();  // 10개를 출력하고 남은 단어들 삭제 -> queue 비우기
    }                          
    terminate_ncurses();
    return 0;
}