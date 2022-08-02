#include <iostream>
#include <cstdio>
#include <cctype>
#include <string.h>
#include <unistd.h>
#include <queue>
#include <ncurses.h>
#include <sstream>

#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

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

/*
아래의 구조체를 통해 정보 받아오기
*/
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

int main(int argc, char* argv[]){
    // --------------------------- < part 1. 소켓 생성 및 연결 > ----------------------------------

    struct sockaddr_in addr;                    // IPv4 주소 정보를 담을 구조체

    char* serv_ip = argv[1];                    // ip주소 문자열 선언
    char* serv_port = argv[2];                  // port번호 문자열 선언

    memset(&addr, 0, sizeof(addr));             // addr구조체의 모든 번수 0으로 초기화
    addr.sin_family = AF_INET;                  // 주소체계 지정
    addr.sin_addr.s_addr = inet_addr(serv_ip);  // 문자열 기반의 ip주소 초기화  
    addr.sin_port = htons(atoi(serv_port));     // 문자열 기반의 port번호 초기화

    int tcp_socket = socket(PF_INET, SOCK_STREAM, 0);               // 연결지향형 소켓 생성
    connect(tcp_socket, (struct sockaddr*) &addr, sizeof(addr));    // 서버 주소를 통해 연결 요청

    // ----------------------- < part 2. 검색창 실행 및 서버와 소통 > ---------------------------------

    initialize_ncurses();
    char ch;
    string queryStr = "", currentWord, tempWord;

    while(true) {
        ch = getch(); // 한 글자 타이핑 받기
        if(ch == '\x1B') break; // esc 입력 시 종료
        
        // -------------------- 타이핑 받은 글자 처리 ---------------------------

        if('a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || ch == ' ') {
            queryStr.push_back(ch); // 현재까지 타이핑 한 단어에 방금 입력한 글자 추가
        }
        else if(ch == '\x7F') {  // 현재까지 타이핑 한 단어에서 마지막 글자 삭제
            if(queryStr.length() != 0)
                queryStr.pop_back();
        }
        else continue; // 알파벳, backspace, esc 이외에 다른 글자를 쳤을 경우

        // -------------------- 타이핑 받은 글자 콘솔 출력 ---------------------------

        for(istringstream is(queryStr) ; is >> tempWord ; ) { // 현재까지 타이핑 한 글자를 currentWord에 저장
            currentWord = tempWord;
        }
        if(queryStr.length() == 0) currentWord.clear(); // 현재까지 입력된 글자가 없을 경우 currentWord 비우기

        mvprintw(1, 13, "%s", queryStr.c_str());        // Search Word 라인에서 오른쪽으로 13만큼 이동한 곳에 현재까지 입력한 단어 출력
                                                        // ex . Search Word: pl
        clrtoeol();                                     // from current position, clear to end of line

        // --------------- 입력받은 새로운 단어에 대한 추천검색어 받아오기 --------------------
        string array[10];
        if(queryStr.length() != 0) {
            write(tcp_socket, queryStr.c_str(), sizeof(queryStr));
            read(tcp_socket, &array, sizeof(array));
        }
        // -------------------- 추천 검색어 상위 10개 출력 ---------------------------

        move(3,0); // ---- 밑 출력 부분의 첫 위치로 이동
        for(int i=0; i<sizeof(array); i++) { // pq에 저장된 상위 10개의 단어만 출력

            move(3+i, 0); // 10개 중 몇 번째 출력인지에 따라 출력 line 변경
            attron(COLOR_PAIR(1)); // 색상 설정
            int k;
            string word = array[i];
            for(k=0; k<queryStr.length(); k++) printw("%c", word[k]); // 타이핑 한 글자까지 색상을 넣어서 출력
            attrset(A_NORMAL); // 색상 속성 제거
            for(; k<word.length(); k++) printw("%c", word[k]); // 남은 글자들은 원래 색상으로 출력
            printw("\n");
            
            clrtoeol();                                 // clear to end of line
        }
        clrtobot();                                     // clear from cursor until the last line
        move(1,13+queryStr.length()); // Search Word: 라인에서 13 + 타이핑한 글자 길이 좌표로 이동
    }                          
    terminate_ncurses();
    close(tcp_socket);
    return 0;
}