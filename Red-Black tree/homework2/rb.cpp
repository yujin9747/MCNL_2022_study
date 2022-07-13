#include <iostream>
#include <cstring>
#include <string>
// 문제점 : rotate 하고 돌아갈 때, toLeft 혹은 toRight 한 것 밑에 연결됨. 새로운 this 로 바뀌는 것이 아니라..
// prev 기준 rotate 할 때 prev의 prev 까지 신경써서 돌려야 하나,,?
using namespace std;

template <typename T1, typename T2> 
class node{
    public :
        T1 first; // key(string)
        T2 second; // value(int)
        node* left;
        node* right;
        node* prev;
        string color; // root/red/black
    public :
        node(): first(""), second(0), color("red"), prev(this), left(nullptr), right(nullptr){}
        node(pair<T1, T2> p) : first(p.first), second(p.second), color("black"), prev(nullptr), left(nullptr), right(nullptr){} // head node 생성
        //node(pair<T1, T2> p, node* head): first(p.first), second(p.second), color("red"), left(nullptr), right(nullptr){}
        ~node(){}
        void show() const{ cout << "key : " << first <<  "\nvalue : " << second << "\n\n";}
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
        // void erase(T1 key);  
};

template <typename T1, typename T2>
class MapIterator{
    public:
        node<T1, T2>* curr;
    public:
        MapIterator(node<T1, T2>* p = nullptr) :curr(p){};
        MapIterator& operator++(){
            if(curr->right == nullptr) { // 위로 올라 가야 하는 상황
                if(curr->prev == curr) curr = nullptr; // root인데 오른쪽 자식이 없는 경우
                else if(curr->prev->prev == curr->prev && curr->prev->right == curr) curr = nullptr; // root의 직계자식인데 root의 오른쪽 자식일 경우
                else if(curr->prev->prev == curr->prev && curr->prev->left == curr) curr = curr->prev; // root의 직계자식인데 root의 왼쪽 자식인 경우
                else{
                    while(curr->prev != curr->prev->prev->left) {// 자신의 부모가 조상의 왼쪽에 연결 되어 있을 때, 그 조상까지 이동 해야 함.
                        curr = curr->prev;
                        if(curr->prev == curr->prev->prev) { // 옮긴 후 조상이 head인 경우 -> 더이상 올라갈 곳 없으므로 nullptr 반환
                            curr = nullptr;
                            return *this;
                        }
                    }
                    curr = curr->prev->prev; // 꺾인 조상 위치로 이동 (조상 기준 왼쪽 자손으로부터 올라온 것)
                }
            }
            else { // 오른쪽 자식 존재하는 경우, 오른쪽 자식 중 가장 작은 것 방문
                curr = curr->right->begin();
            }
            return *this;
        }
        bool operator==(const MapIterator &ref){
            return curr == ref.curr;
        }
        bool operator!=(const MapIterator &ref){
            return curr != ref.curr;
        }
        node<T1, T2>* operator->()
        {
            return curr;
        }
};

template <typename T1, typename T2>
class my_map{
    public:
        node<T1, T2>* head;
    public:
        my_map():head(nullptr){}
        void insert(pair<T1, T2> p){
            if(head == nullptr) {
                head = new node<T1, T2>(p);
                head->color = "black";
                head->prev = head;
                cout << "< root >\n";
                head->show();
            }
            else {
                node<T1, T2>* newNode = new node<T1, T2>(p);
                head = head->insert(newNode, head);
            }
        }
        T2& operator[](T1 key){
            if(head == nullptr) { // key값으로 새로운 노드 만들어서 second만 값은 임의로 채워두고 second 주소 반환하기
                node<T1, T2>* newNode = new node<T1, T2>();
                newNode->first = key;
                return newNode->second;
            }
            // 해당 값이 존재하는 위치 찾아서 second 주소 반환하기
            iterator found = find(key);
            if(found == iterator(nullptr)) { // insert 후, 다시 find 해서 second 주소 반환
                node<T1, T2>* newNode = new node<T1, T2>();
                newNode->first = key;
                newNode->prev = nullptr;
                newNode->color = "red";
                head = head->insert(newNode, head);
                return newNode->second;
            }
            return found->second;
        }

        typedef MapIterator<T1, T2> iterator;

        iterator find(T1 key){ // to be fixed
            if(head == nullptr) return iterator(nullptr);
            else { 
                node<T1, T2>* found = head->find(key);
                if(found == nullptr) return iterator(nullptr);
                else return iterator(found);
            }
        }
        iterator begin(){
            node<T1, T2>* curr = head;
            while(curr->left != nullptr){
                curr = curr->left;
            }
            return iterator(curr);
        }
        iterator end(){
            return iterator(nullptr);
        }
};

template <typename T1, typename T2>
void print_map(my_map<T1, T2> m){
    my_map<string, int>::iterator iter;
    iter = m.begin(); // 이 부분에서 iterator가 타입으로 인식되어야 하는데 인식이 되지 않는 상황*
    for(; iter != m.end(); ++iter){
        cout << iter->first << ": " << iter->second << "\n";
    }
}

int main(){
    // 다음에 할 일 : erase 구현
    /*
    구현할 목록
    1. insert에 make_pair로 인수 받아서 처리 하도록 수정 (o)
    2. insert하거나 edit할 때 [] 안에 있는 값을 통해서 하기 (o)
    3. erase 함수를 통해 지우기
    4. find를 통해 해당 키값을 가지는 노드 반환 (o)
    4. end를 통해 마지막 값 다음을 가리키는 것 반환 (o)
    5. begin을 통해 처음 값을 가리키는 것 반환 (o)
    */
   // insert 코드 리뷰 하며 발생할 수 있는 버그 잡기 -> 현재 아래의 테스트로는 잘 돌아가는 상황
    my_map<string, int> m;

    cout << "** First Step **\n";
    m.insert(make_pair("Global", 10));
    m.insert(make_pair("Handong", 30));
    m.insert(make_pair("CSEE", 20));
    m.insert(make_pair("MCNL", 15));
    print_map(m);

    cout << "\n** Second Step **\n";
    m["Pohang"] = 50;
    m["Korea"] = 60;
    print_map(m);

    cout << "\n** Third Step **\n";
    m["CSEE"] = 100;
    // m.erase("Global"); 
    print_map(m);

    cout << "\n** Fourth Step **\n";
    string key = "MCNL";
    if(m.find(key) != m.end()){
        cout << key << " Exists!\n";
    }
    else {
        cout << key << " does not exists!\n";
    }

    return 0;
}

/* my_map functions */
template<typename T1, typename T2>
void node<T1, T2>::rotationL(node* toLeft, node* newParent){
    node* temp = newParent->left; // 위로 올라 갈 새로운 부모
    newParent->left = toLeft; // toLeft = newParent의 부모였지만 왼쪽 아래로 내려가는 노드
    toLeft->right = temp; // 기존에 newParent가 연결되어 있던 곳에 newParent의 기존 left 연결
    temp = toLeft->prev;
    toLeft->prev = newParent;
    newParent->prev = temp;
}
template<typename T1, typename T2>
void node<T1, T2>::rotationR(node* toRight, node* newParent){
    node* temp = newParent->right;
    newParent->right = toRight;
    toRight->left = temp; 
    temp = toRight->prev;
    toRight->prev = newParent;
    newParent->prev = temp;
}
template<typename T1, typename T2>
node<T1, T2>* node<T1, T2>::insert(node<T1, T2>* newNode, node* head){
    int strcompare = strcmp(this->first.c_str(), newNode->first.c_str());
    node<T1, T2>* prevL = this->prev->left;
    node<T1, T2>* prevR = this->prev->right;
    if(strcompare == 0){ // first == key : 중복
        cout << "same!\n";
        if(this != head) return this->prev; 
        else return head;
    }
    else{ // 중복이 아닌 경우
        if(strcompare < 0){ // first < key 오른쪽 자손이 들어온 경우
            if(right != nullptr) {
                if(this->prev == this) {
                    head = right->insert(newNode, head);
                }
                else if(this->prev->left == this) {
                    head = right->insert(newNode, head);
                }
                else if(this->prev->right == this) {
                    head = right->insert(newNode, head); 
                }

                if(this->color.compare("red") == 0){
                    if(this == head) { // root 노드인 경우
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
                right = newNode;
                cout << "< right >\n";
                this->right->show();
                if(this->prev == this) return head; // root 인 경우
                if(this->prev->right != this){ // 꺾인 경우
                    if(this->prev->right == nullptr){ // 삼촌이 없는 경우
                        newNode->color = "black";
                        rotationL(this, newNode);
                        rotationR(newNode->prev, newNode);
                        return head;
                    }
                    else{ // 삼촌이 있는 경우
                        if(this->color.compare("red") == 0){
                            this->color = "black";
                            if(this->prev->right->color.compare("red") == 0){ // 삼촌도 red
                                this->prev->color = "red";
                                this->prev->right->color = "black";
                                return head;
                                // back tracking
                            }
                            else{ // 삼촌은 black
                                rotationL(this, newNode);
                                return head;
                            }
                        }
                        // this가 black이면 아무 일도 하지 않는다
                    }
                }
                else{ // 안 꺾인 경우
                    if(this->prev->left == nullptr){ // 삼촌이 없는 경우
                        if(this->color.compare("red") == 0) this->color = "black";
                        rotationL(this->prev, this); 
                        if(this->prev->right == this->left) this->prev->right = this;
                        else if(this->prev->left == this->left) this->prev->left = this;
                        return head;
                    }
                    else{ // 삼촌이 있는 경우
                        if(this->color.compare("red") == 0){ 
                            this->color = "black";
                            if(this->prev->left->color.compare("red") == 0){ // 삼촌도 red
                                this->prev->left->color = "black";
                                this->prev->color = "red";
                                return head;
                                // back tracking
                            }
                            else{ // 삼촌은 black
                                rotationL(this, newNode);
                                return head;
                            }
                        }
                        // this가 black이면 아무 일도 하지 않는다
                    }

                }
            }
        }
        else{ // first > key 왼쪽 자손이 들어온 경우
            if(left != nullptr) {
                if(this->prev == this) head = left->insert(newNode, head);
                else if(this->prev->left == this) head = left->insert(newNode, head);
                else if(this->prev->right == this) head = left->insert(newNode, head);
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
                cout << "< left >\n";
                this->left->show();

                if(this->prev == this) return head; // root 인 경우
                if(this->prev->left != this){ // 꺾인 경우
                    if(this->prev->left == nullptr){ // 삼촌이 없는 경우
                        if(this->color.compare("red") == 0) newNode->color = "black";
                        rotationR(this, newNode);
                        rotationL(newNode->prev, newNode);
                        return head;
                    }
                    else{ // 삼촌이 있는 경우
                        if(this->color.compare("red") == 0){
                            this->color = "black";
                            if(this->prev->left->color.compare("red") == 0){ // 삼촌도 red
                                this->prev->color = "red";
                                this->prev->left->color = "black";
                                return head;
                                // back tracking
                            }
                            else{ // 삼촌은 black
                                rotationR(this, newNode);
                                return head;
                            }
                        }
                        // this가 black이면 아무 일도 하지 않는다
                    }
                }
                else{ // 안 꺾인 경우
                    if(this->prev->right == nullptr){ // 삼촌이 없는 경우
                        if(this->color.compare("red") == 0) this->color = "black";
                        rotationR(this->prev, this);
                        if(this->prev->right == this->right) this->prev->right = this; // toRight한 조상의 부모에 this 연결
                        else if(this->prev->left == this->right) this->prev->left = this;
                        return head;
                    }
                    else{ // 삼촌이 있는 경우
                        if(this->color.compare("red") == 0){ 
                            this->color = "black";
                            if(this->prev->right->color.compare("red") == 0){ // 삼촌도 red
                                this->prev->right->color = "black";
                                this->prev->color = "red";
                                return head;
                                // back tracking
                            }
                            else{ // 삼촌은 black
                                rotationR(this, newNode);
                                return head;
                            }
                        }
                        // this가 black이면 아무 일도 하지 않는다
                    }

                }
            }
        }
    }
    return head;
}

template<typename T1, typename T2>
node<T1, T2>* node<T1, T2>::find(T1 key){
    int strcompare = strcmp(key.c_str(), this->first.c_str());
    if(strcompare == 0){ // found!
        return this; 
    }
    else{
        if(strcompare > 0){ // this 보다는 찾으려는 Key가 큰 경우
            if(right == nullptr) return nullptr; // 오른쪽이 비어있는 경우
            else{ // 오른쪽에서 있는지 찾기
                return right->find(key);
            }
        }
        else{ // this 보다 찾으려는 Key가 작은 경우
            if(left == nullptr) return nullptr; // 왼쪽이 비어있는 경우
            else{ // 왼쪽에서 있는지 찾기
                return left->find(key);
            }
        }
    }
}

// https://gwpaeng.tistory.com/309 : 참고 사이트 보고 다시 구현 해보기
template<typename T1, typename T2>
void my_map<T1, T2>::erase(T1 key){

// key값을 가지는 노드를 찾는다 -> find 이용
// 해당 노드를 삭제해 해당 노드의 자식을 prev에 적절히 연결한다

    iterator found = find(key);
    if(founr == iterator(nullptr)) return; // 삭제할 노드를 찾지 못 한 경우

    iterator smallestR = found->right->begin(); // 오른쪽 서브 트리에서 가장 작은 값으로 자리 대체
    if(smallestR->prev->left == smallestR) smallestR->prev->left = nullptr;
    else if(smallestR->prev->right == smallestR) smallestR->prev->right = nullptr;
    found->first = smallestR->first;
    found->second = smallestR->second;

    if(found->color.compare("black") == 0){ // 삭제되는 노드의 색상이 black인 경우
        // 이중 흑색 노드인지 검사
        if(smallestR->color.compare("black") == 0) { // 이중 흑색 노드인 경우 : 4가지 cases 존재
            
            // 1. found의 형제가 red
            // 2. found의 형제가 black, found의 형제 자식 모두 black
            // 3. found의 형제가 black, 형제의 왼쪽 자식이 red, 형제의 오른쪽 자식이 black
            // 4. found의 형제가 black, found의 형제의 오른쪽 자식이 red
            

        }
        else{// smallestR이 red 였던 경우에는 black으로 바꾼 채로 대체해야 한다 -> 이미 found가 black 이므로 first, second 값만 바꾼 채로 그대로 삭제
            delete smallestR;
        }
    }
    // 삭제 노드(found)가 red일 경우 그냥 삭제
    
}

