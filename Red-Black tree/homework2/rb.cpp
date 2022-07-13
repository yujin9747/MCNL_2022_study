#include <iostream>
#include <cstring>
#include <string>
// 정상 출력되는 상황
// delete 할 때 rb 트리 유지하는지 다시 확인하기
using namespace std;

/* node class : tree를 구성하는 기본 노드 */
template <typename T1, typename T2> 
class node{
    public :
        T1 first; // key(string)
        T2 second; // value(int)
        node* left;
        node* right;
        node* prev;
        string color; // red/black
    public :
        node(): first(""), second(0), color("red"), prev(this), left(nullptr), right(nullptr){}
        node(pair<T1, T2> p) : first(p.first), second(p.second), color("black"), prev(nullptr), left(nullptr), right(nullptr){} // head node 생성
        ~node(){}
        void show() const{ cout << "key : " << first <<  "\nvalue : " << second << "\n\n";} // DEBUG PRINT
        node* insert(node* newNode, node* head);
        void rotationR(node* toRight, node* newParent);
        void rotationL(node* toLeft, node* newParent);
        node* find(T1 key);
        node* begin(){
            node* curr = this;
            while(curr->left != nullptr){
                curr = curr->left;
            }
            return curr;
        }
        T2& operator[](T1 key); 
        void erase(T1 key);
};

/* MapIterator 클래스 : node 포인터를 가지고 있으며, operator overloading을 통해 curr의 위치를 이동해 반환한다 */
template <typename T1, typename T2>
class MapIterator{
    public:
        node<T1, T2>* curr;
    public:
        MapIterator(node<T1, T2>* p = nullptr) :curr(p){};

        /* 오름차순으로 TRee 순회하도록 위치 이동 */
        MapIterator& operator++(){
            if(curr->right == nullptr) {                                                    // 1. 위로 이동
                if(curr->prev == curr) curr = nullptr;                                      // - root인 경우 -> 순회 끝
                else if(curr->prev->prev == curr->prev && curr->prev->right == curr) curr = nullptr;    // - root의 직계자식인데 root의 오른쪽 자식일 경우 -> 순회 끝
                else if(curr->prev->prev == curr->prev && curr->prev->left == curr) curr = curr->prev;  // - root의 직계자식인데 root의 왼쪽 자식인 경우 -> 하나 위로 이동
                else{
                    if(curr->prev->left == curr){                                           // - 부모의 왼쪽에 위치한 경우 : 조상(오:부모), 부모(왼:CUrr) 모양으로 꺾인 위치까지 이동
                        while(curr->prev != curr->prev->prev->right) {                      
                            curr = curr->prev;
                            if(curr->prev == curr->prev->prev) {                            // 옮긴 후 조상이 head인 경우 : head 순회
                                return *this;
                            }
                        }
                        curr = curr->prev;                                                  // 꺾인 기준점인 부모 순회
                    }
                    else if(curr->prev->right == curr){                                     // - 부모의 오른쪽에 위치한 경우 : 조상(왼:부모), 부모(오:curr) 모양으로 꺾인 위치까지 이동
                        while(curr->prev != curr->prev->prev->left){
                            curr = curr->prev;
                            if(curr->prev == curr->prev->prev){                             // 옮긴 후 조상이 head인 경우 : 순회 끝
                                curr = nullptr;
                                return *this;
                            }
                        }
                        curr = curr->prev->prev;                                            // 조상으로 이동 -> 조상 기준 왼쪽을 모두 방문 후, 자기 자신 방문
                    }
                }
            }
            else {                                                                         // 2. 오른쪽 subtree 중 가장 작은 것 부터 방문
                curr = curr->right->begin();
            }
            return *this;                                                                   // 최종 return : 위치 이동한 curr 반환
        }

        /* curr와 비교하려는 Iterator가 가리키는 노드가 동일한지 반환 */
        bool operator==(const MapIterator &ref){
            return curr == ref.curr;
        }
        /* curr와 비교하려는 Iterator가 가리키는 노드가 동일하지 않은지 반환 */
        bool operator!=(const MapIterator &ref){
            return curr != ref.curr;
        }

        /* curr의 값에 접근하기 위한 Operator로, curr 반환 */
        node<T1, T2>* operator->()
        {
            return curr;
        }
};

/* node로 이루어진 rb tree의 head를 담고 있으며, MapIterator 클래스가 타입으로 지정되어 있다 */
template <typename T1, typename T2>
class my_map{
    public:
        node<T1, T2>* head;
    public:
        my_map():head(nullptr){}

        /* insert : 노드를 삽입 후, head를 update 한다 */
        void insert(pair<T1, T2> p){
            if(head == nullptr) {                           // 1. 첫 노드가 insert되는 경우 -> head에 바로 연결(색:black)
                head = new node<T1, T2>(p);
                head->color = "black";
                head->prev = head;
                // cout << "< root >\n";
                // head->show();
            }
            else {                                          // 2. 첫 노드가 아닌 경우 -> 새로운 노드 생성해 node 클래스 내 insert 함수 call
                node<T1, T2>* newNode = new node<T1, T2>(p);
                head = head->insert(newNode, head);
            }
        }

        /* 노드 삽입 / 찾기 후, 해당 노드의 second의 주소를 반환한다 */
        T2& operator[](T1 key){
            if(head == nullptr) {                                   // 1. tree가 비어있는 경우 -> 새 노드 생성, update head, second 주소 반환
                node<T1, T2>* newNode = new node<T1, T2>();
                newNode->first = key;
                head = newNode;
                return newNode->second;
            }
            iterator found = find(key);                             // key값을 가진 노드 찾기
            if(found == iterator(nullptr)) {                        // 2. 존재하지 않는 경우 -> insert 후, second 주소 반환
                node<T1, T2>* newNode = new node<T1, T2>();
                newNode->first = key;
                newNode->prev = nullptr;
                newNode->color = "red";
                head = head->insert(newNode, head);
                return newNode->second;
            }
            return found->second;                                   // 3. 존재하는 경우 -> 바로 second 주소 반환
        }
        /* erase : node 클래스 내, erase 함수 호출 */
        void erase(T1 key){
            return head->erase(key);
        }

        typedef MapIterator<T1, T2> iterator;                       // MapIterator 클래스 타입 지정자인 iterator 선언

        /* key값을 가진 노드 찾아 iterator 반환 */
        iterator find(T1 key){ 
            if(head == nullptr) return iterator(nullptr);           // 1. tree가 비어 있는 경우
            else { 
                node<T1, T2>* found = head->find(key);
                if(found == nullptr) return iterator(nullptr);      // 2. find 하지 못한 경우
                else return iterator(found);                        // 3. find 한 경우
            }
        }

        /* 현재 노드 기준 가장 작은 노드의 iterator 반환 */
        iterator begin(){
            node<T1, T2>* curr = head;
            while(curr->left != nullptr){
                curr = curr->left;
            }
            return iterator(curr);                                  // left 가 nullptr인 노드 반환
        }

        iterator end(){
            return iterator(nullptr);
        }
};

/* MapIterator를 사용해 오름차순으로 key를 출력한다 */
template <typename T1, typename T2>
void print_map(my_map<T1, T2> m){
    my_map<string, int>::iterator iter;
    iter = m.begin(); // 이 부분에서 iterator가 타입으로 인식되어야 하는데 인식이 되지 않는 상황*
    for(; iter != m.end(); ++iter){
        cout << iter->first << ": " << iter->second << "\n";
    }
}

int main(){
    /*
    구현 목록
    1. insert에 make_pair로 인수 받아서 처리 하도록 수정 (o)
    2. insert하거나 edit할 때 [] 안에 있는 값을 통해서 하기 (o)
    3. erase 함수를 통해 지우기 (세모 -> rb tree 유지하는지 확인 필요)
    4. find를 통해 해당 키값을 가지는 노드 반환 (o)
    4. end를 통해 마지막 값 다음을 가리키는 것 반환 (o)
    5. begin을 통해 처음 값을 가리키는 것 반환 (o)
    */
    my_map<string, int> m;

    cout << "** First Step **\n";
    m.insert(make_pair("Global", 10));
    m.insert(make_pair("Handong", 30));
    m.insert(make_pair("CSEE", 20));
    m.insert(make_pair("MCNL", 15));
    print_map(m);
    // CSEE -> Global -> Handong -> MCNL 순 출력

    cout << "\n** Second Step **\n";
    m["Pohang"] = 50;
    m["Korea"] = 60;
    print_map(m);
    // CSEE - Global -> Handong -> Korea -> MCNL -> Pohang 순 출력

    cout << "\n** Third Step **\n";
    m["CSEE"] = 100;
    m.erase("Global"); 
    print_map(m);
    // CSEE(값 100으로 변경) - Handong -> Korea -> MCNL -> Pohang 순 출력

    cout << "\n** Fourth Step **\n";
    string key = "MCNL";
    if(m.find(key) != m.end()){
        cout << key << " Exists!\n";
    }
    else {
        cout << key << " does not exists!\n";
    }
    // MCNL Exists! 출력

    return 0;
}

/* rotation functions */
template<typename T1, typename T2>
void node<T1, T2>::rotationL(node* toLeft, node* newParent){
    /* newParent를 toLeft의 부모로 위치 변경*/
    
    // stage 1. 자식을 가리키는 left/right 포인터 값 변경
    node* temp = newParent->left; 
    newParent->left = toLeft; 
    toLeft->right = temp;

    // stage 2. 부모를 가리키는 prev 포인터 값 변경
    temp = toLeft->prev;
    toLeft->prev = newParent;
    newParent->prev = temp;
}
template<typename T1, typename T2>
void node<T1, T2>::rotationR(node* toRight, node* newParent){
     /* newParent를 toRight의 부모로 위치 변경*/
    
    // stage 1. 자식을 가리키는 left/right 포인터 값 변경
    node* temp = newParent->right;
    newParent->right = toRight;
    toRight->left = temp; 

    // stage 2. 부모를 가리키는 prev 포인터 값 변경
    temp = toRight->prev;
    toRight->prev = newParent;
    newParent->prev = temp;
}

/*  insert function : 트리 내 적절한 위치 찾아 삽입 및 Rb tree 조건 충족 
    my_map 클래스 내 insert 함수의 helper function
*/
template<typename T1, typename T2>
node<T1, T2>* node<T1, T2>::insert(node<T1, T2>* newNode, node* head){ // insert된 head값 update 후 반환 
    int strcompare = strcmp(this->first.c_str(), newNode->first.c_str());
    if(strcompare == 0){                                        // first == key : 중복 -> 삽입하지 않는다
        cout << "same!\n";
        return head;                                       // 변경사항 없이 그대로 반환
    }
    else{ // 중복이 아닌 경우
        if(strcompare < 0){                                     // this node 기준 오른쪽 자식이 들어온 경우
            if(right != nullptr) {                              // 1) 오른쪽이 비어 있지 않은 경우 -> recursive call
                head = right->insert(newNode, head); 

                if(this->color.compare("red") == 0){            // recursive call 이후 this가 red인 경우
                    if(this == head) {                          // root면 Black으로 다시 변경 후 리턴
                        this->color = "black"; 
                        return head;
                    }
                    else{                                       // 삼촌도 RED면? -> 색상만 변경(this, 삼촌 : black, prev : red)
                        if(this->prev->right == this && this->prev->left->color.compare("red") == 0 || this->prev->left == this && this->prev->right->color.compare("red") == 0){
                                this->prev->left->color = "black";
                                this->prev->right->color = "black";
                                this->prev->color = "red";
                                return head;
                        }
                    }
                }
            }
            else{                                               // 2) 오른쪽이 비어 있는 경우 -> 삽입
                newNode->color = "red";
                newNode->prev = this;
                right = newNode;

                // DEBUG PRINT
                // cout << "< right >\n";
                // this->right->show();

                if(this->prev == this) return head;             // root의 right에 삽입한 경우 바로 반환
                if(this->prev->right != this){                  // 2-1) prev-this-newNode 모양이 꺾인 경우
                    if(this->prev->right == nullptr){           // 2-1-1) 삼촌이 없는 경우 -> newNode(left:prev, right:this) 모양으로 회전
                        newNode->color = "black";
                        rotationL(this, newNode);
                        rotationR(newNode->prev, newNode);
                        return head;            
                    }
                    else{                                       // 2-1-2) 삼촌이 있는 경우
                        if(this->color.compare("red") == 0){    // this가 red라면? (this가 black이면 아무 일도 하지 않는다)
                            this->color = "black";              
                            if(this->prev->right->color.compare("red") == 0){ // 삼촌도 red 라면? -> 색상만 변경(삼촌, this: black, prev : red)
                                this->prev->color = "red";
                                this->prev->right->color = "black";
                                return head;
                                // back tracking이 이어진다 -> recursive call 이후 조건문 확인부분
                            }
                            else{                                            // 삼촌은 black 이라면? -> prev(right:newNode), newNode(left:prev) 모양으로 회전
                                rotationL(this, newNode);                   
                                return head;
                            }
                        }
                    }
                }
                else{                                           // 2-1-2) prev-this->newNode 모양이 안 꺾인 경우 : this(left:prev, right:newNode) 모양으로 변경
                    if(this->prev->left == nullptr){            // 삼촌이 없는 경우
                        if(this->color.compare("red") == 0) this->color = "black";
                        rotationL(this->prev, this); 
                        if(this->prev->right == this->left) this->prev->right = this;
                        else if(this->prev->left == this->left) this->prev->left = this;
                        return head;
                    }
                    else{                                       // 삼촌이 있는 경우
                        if(this->color.compare("red") == 0){    // this가 red면? -> black으로 변경(this가 black이면 아무 일도 하지 않는다)
                            this->color = "black";
                            if(this->prev->left->color.compare("red") == 0){ // 삼촌도 red면? -> 색상만 변경(삼촌:black, prev: red)
                                this->prev->left->color = "black";
                                this->prev->color = "red";
                                return head;
                                // back tracking이 이어진다 -> recursive call 이후 조건문 확인부분
                            }
                            else{                                           // 삼촌은 black 이면? -> newNode(left:this, right:nullptr) 모양으로 변경
                                rotationL(this, newNode);
                                return head;
                            }
                        }
                    }

                }
            }
        }
        else{                                                               // this node 기준으로 왼쪽 자식이 들어 온 경우
            if(left != nullptr) {                                           // 이하 주석 설명 위와 동(방향만 다름)
                head = left->insert(newNode, head);

                if(this->color.compare("red") == 0){
                    if(this == head) {
                        this->color = "black";
                        return head;
                    }
                    else{
                        if(this->prev->right == this && this->prev->left->color.compare("red") == 0 || this->prev->left == this && this->prev->right->color.compare("red") == 0){
                                this->prev->left->color = "black";
                                this->prev->right->color = "black";
                                this->prev->color = "red";
                                return head;
                        }
                    }
                }
            }
            else{
                newNode->color = "red";
                newNode->prev = this;
                left = newNode;

                // DEBUG PRINT
                // cout << "< left >\n";
                // this->left->show();

                if(this->prev == this) return head; 
                if(this->prev->left != this){ 
                    if(this->prev->left == nullptr){
                        if(this->color.compare("red") == 0) newNode->color = "black";
                        rotationR(this, newNode);
                        rotationL(newNode->prev, newNode);
                        return head;
                    }
                    else{ 
                        if(this->color.compare("red") == 0){
                            this->color = "black";
                            if(this->prev->left->color.compare("red") == 0){
                                this->prev->color = "red";
                                this->prev->left->color = "black";
                                return head;
                            }
                            else{ 
                                rotationR(this, newNode);
                                return head;
                            }
                        }
                    }
                }
                else{
                    if(this->prev->right == nullptr){ 
                        if(this->color.compare("red") == 0) this->color = "black";
                        rotationR(this->prev, this);
                        if(this->prev->right == this->right) this->prev->right = this; 
                        else if(this->prev->left == this->right) this->prev->left = this;
                        return head;
                    }
                    else{ 
                        if(this->color.compare("red") == 0){ 
                            this->color = "black";
                            if(this->prev->right->color.compare("red") == 0){ // 삼촌도 red
                                this->prev->right->color = "black";
                                this->prev->color = "red";
                                return head;
                            }
                            else{ 
                                rotationR(this, newNode);
                                return head;
                            }
                        }
                    }
                }
            }
        }
    }
    return head;
}

/* find function : key값을 가진 노드를 찾아 반환한다 */
template<typename T1, typename T2>
node<T1, T2>* node<T1, T2>::find(T1 key){
    int strcompare = strcmp(key.c_str(), this->first.c_str());
    if(strcompare == 0){                                        // found! -> this 반환
        return this; 
    }
    else{                                               
        if(strcompare > 0){                                     // 오른쪽 subtree에서 찾아야 하는 경우
            if(right == nullptr) return nullptr;                // - 오른쪽이 비어있는 경우 : 찾을 수 없음
            else{                                               
                return right->find(key);                        // - 오른쪽에서 Recursive call
            }
        }
        else{                                                   // 왼쪽 subtree에서 찾아야 하는 경우
            if(left == nullptr) return nullptr;                 // - 왼쪽이 비어있는 경우 : 찾을 수 없음
            else{ 
                return left->find(key);                         // - 왼쪽에서 Recursive call
            }
        }
    }
}

/*  erase function : key값을 가진 노드를 오른쪽에서 가장 작은 값으로 대체하고, 대체된 노드에 대한 메모리는 삭제된다 */
template <typename T1, typename T2>
void node<T1, T2>::erase(T1 key){

    int strcompare = strcmp(key.c_str(), this->first.c_str());
    if(strcompare == 0){                                        // 1. Key값을 가진 노드 발견
        // stage 1. 오른쪽 서브 트리에서 가장 작은 값 찾기
        node<T1, T2>* smallestR = this->right->begin();         
        
        // stage 2. 연결된 모양에 따라 자식 포인터 변경
        if(smallestR->prev->left == smallestR) {                
            smallestR->prev->left = smallestR->right;
            smallestR->right->prev = smallestR->prev;
        }
        else if(smallestR->prev->right == smallestR) {
            smallestR->prev->right = smallestR->right;
            smallestR->right->prev = smallestR->prev;
        }

        // stage 3. 대체할 노드의 값을 삭제할 노드의 값에 대입
        this->first = smallestR->first;
        this->second = smallestR->second; 

        // stage 4. 대체된 노드에 대한 메모리 삭제
        delete smallestR;
        return;
    }
    else{                                                   // 2. Key값을 가진 노드 발견 못한 경우
        if(strcompare > 0){                                 // 2-1) 오른쪽 자식을 탐색해야 하는 경우
            if(this->right == nullptr) return;              // 오른쪽이 비어있는 경우 -> 삭제할 노드가 존재하지 않는다
            else{ 
                return this->right->erase(key);             // 비어있지 않은 경우 -> right에서 recursive call
            }
        }
        else{                                               // 2-2) 왼쪽 자식을 탐색해야 하는 경우
            if(this->left == nullptr) return;               // 왼쪽이 비어있는 경우 -> 삭제할 노드가 존재하지 않는다
            else{ 
                return this->left->erase(key);              // 비어있지 않은 경우 -> Left에서 recursive call
            }
        }
    }
}