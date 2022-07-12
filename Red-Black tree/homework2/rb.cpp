#include <iostream>
#include <cstring>
#include <string>

using namespace std;

/* my_map class */
template <typename T1, typename T2> 
class my_map{
    private :
        T1 first; // key(string)
        T2 second; // value(int)
        my_map* left;
        my_map* right;
        my_map* prev;
        string color; // root/red/black
        bool root;
    public :
        my_map() : root(false), color("black"){}
        my_map(T1 key, T2 value): first(key), second(value), color("red"), left(NULL), right(NULL) {}
        ~my_map(){}
        void show() const{ cout << "key : " << first <<  "\nvalue : " << second << "\n";}
        void insert(pair<T1, T2>* p);
        void rotationR(my_map* toRight, my_map* newParent);
        void rotationL(my_map* toLeft, my_map* newParent);
        void recursiveInsert(T1 key, T2 value);
        T2& operator[] (T1 key);
        void erase(T1 key);  
        
        typedef MapIterator<T1, T2> iterator; // MapIterator 클래스 타입 지정
        
        iterator find(T1 key){
            if(this->first.compare(key) == 0) return iterator(this);
            else{
                if(this->left != NULL)  // 왼쪽 자식 먼저 찾기
                    return this->left->find(key);               
                if(this->right != NULL)  // 왼쪽에서 못 찾은 경우 오른쪽 찾기
                    return this->right->find(key); 
                return this->end(); // 왼, 오 모두에서 못 찾은 경우
            }
        }
        iterator begin(){
            if(this->left == NULL) return iterator(this);
            else return this->left->begin();
        }
        iterator end(){
            return iterator(nullptr);
        }
};

template<typename T1, typename T2> 
class MapIterator{
    private:
        my_map<T1, T2>* curr;
    public:
        MapIterator(my_map<T1, T2>* p = nullptr) :curr(p){};
        MapIterator& operator++(){
            if(curr->right == nullptr) {
                if(curr->prev->right == this) curr = curr->prev->prev; // prev 기준 오른쪽 자식 방문 후, 자기 위로 바로 이동
                else(curr->prev->left == this) curr = curr->prev; // prev 기준 왼쪽 자식 방문 후, 자기 자신 방문
                return *this;
            }
            else { // 오른쪽 자식 존재하는 경우, 오른쪽 자식 중 가장 작은 것 방문
                curr = this->begin();
                return *this;
            }
        }
        bool operator==(const MapIterator &ref){
            return curr == ref.curr;
        }
        bool operator!=(const MapIterator &ref){
            return curr != ref.curr;
        }
        //my_map<T1, T2>& operator->();
};

template <typename T1, typename T2>
void print_map(my_map<T1, T2> m){
    my_map<T1 , T2>::iterator iter = m.begin(); // 이 부분에서 iterator가 타입으로 인식되어야 하는데 인식이 되지 않는 상황*
    for(; iter != m.end(); iter++){
        cout << iter->first << ": " << iter->second << "\n";
    }
}

int main(){
    // 다음에 할 일 : erase 구현, 컴파일 오류들 해결, 구현한 목록들 점검
    /*
    구현할 목록
    1. insert에 make_pair로 인수 받아서 처리 하도록 수정 (세모 -> 점검 필요)
    2. insert하거나 edit할 때 [] 안에 있는 값을 통해서 하기 (세모 -> 점검 필요)
    3. erase 함수를 통해 지우기
    4. find를 통해 해당 키값을 가지는 노드 반환 (세모 -> 점검 필요)
    4. end를 통해 마지막 값 다음을 가리키는 것 반환 (세모 -> 점검 필요)
    5. begin을 통해 처음 값을 가리키는 것 반환 (세모 -> 점검 필요)
    */
    my_map<string, int> m;

    cout << "** First Step **";
    m.insert(make_pair("Global", 10));
    m.insert(make_pair("Handong", 30));
    m.insert(make_pair("CSEE", 20));
    m.insert(make_pair("MCNL", 15));
    print_map(m);

    cout << "\n** Second Step **";
    m["Pohang"] = 50;
    m["Korea"] = 60;
    print_map(m);

    cout << "\n** Third Step **";
    m["CSEE"] = 100;
    //m.erase("Global");
    print_map(m);

    cout << "\n** Fourth Step **";
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
void my_map<T1, T2>::rotationL(my_map* toLeft, my_map* newParent){
    my_map* temp = newParent->left; // 위로 올라 갈 새로운 부모
    newParent->left = toLeft; // toLeft = newParent의 부모였지만 왼쪽 아래로 내려가는 노드
    toLeft->right = temp; // 기존에 newParent가 연결되어 있던 곳에 newParent의 기존 left 연결
    temp = toLeft->prev;
    toLeft->prev = newParent;
    newParent->prev = temp;
}
template<typename T1, typename T2>
void my_map<T1, T2>::rotationR(my_map* toRight, my_map* newParent){
    my_map* temp = newParent->right;
    newParent->right = toRight;
    toRight->left = temp;
    temp = toRight->prev;
    toRight->prev = newParent;
    newParent->prev = temp;
}
template<typename T1, typename T2>
void my_map<T1, T2>::recursiveInsert(T1 key, T2 value){
    int strcompare = strcmp(this->first.c_str(), key.c_str());
    if(strcompare == 0){ // first == key : 중복
        cout << "same!\n";
        return;
    }
    else{ // 중복이 아닌 경우
        if(strcompare < 0){ // first < key 오른쪽 자손이 들어온 경우
            if(right != NULL) {
                right->recursiveInsert(key, value); 
                if(this->color.compare("red") == 0){
                    if(this->root) this->color = "black";
                    else{
                        if(this->prev->right == this && this->prev->left->color.compare("red") == 0 || 
                            this->prev->left == this && this->prev->right->color.compare("red") == 0){
                                this->prev->left->color = "black";
                                this->prev->right->color = "black";
                                this->prev->color = "red";
                            }
                    }
                }
            }
            else{
                my_map<T1, T2>* child = new my_map(key, value); 
                child->prev = this;
                right = child;
                cout << "right\n";
                this->right->show();

                if(this->prev->right != this){ // 꺾인 경우
                    if(this->prev->right == NULL){ // 삼촌이 없는 경우
                        child->color = "black";
                        rotationL(this, child);
                        rotationR(child->prev, child);
                    }
                    else{ // 삼촌이 있는 경우
                        if(this->color.compare("red") == 0){
                            this->color = "black";
                            if(this->prev->right->color.compare("red") == 0){ // 삼촌도 red
                                prev->color = "red";
                                this->prev->right->color = "black";
                                // back tracking
                            }
                            else{ // 삼촌은 black
                                rotationL(this, child);
                            }
                        }
                        // this가 black이면 아무 일도 하지 않는다
                    }
                }
                else{ // 안 꺾인 경우
                    if(this->prev->left == NULL){ // 삼촌이 없는 경우
                        if(this->color.compare("red") == 0) this->color = "black";
                        rotationL(this->prev, this);
                    }
                    else{ // 삼촌이 있는 경우
                        if(this->color.compare("red") == 0){ 
                            this->color = "black";
                            if(this->prev->left->color.compare("red") == 0){ // 삼촌도 red
                                this->prev->left->color = "black";
                                this->prev->color = "red";
                                // back tracking
                            }
                            else{ // 삼촌은 black
                                rotationL(this, child);
                            }
                        }
                        // this가 black이면 아무 일도 하지 않는다
                    }

                }
            }
        }
        else{ // first > key 왼쪽 자손이 들어온 경우
            if(left != NULL) {
                left->recursiveInsert(key, value); 
                if(this->color.compare("red") == 0){
                    if(this->root) this->color = "black";
                    else{
                        if(this->prev->right == this && this->prev->left->color.compare("red") == 0 || 
                            this->prev->left == this && this->prev->right->color.compare("red") == 0){
                                this->prev->left->color = "black";
                                this->prev->right->color = "black";
                                this->prev->color = "red";
                            }
                    }
                }
            }
            else{
                my_map<T1, T2>* child = new my_map(key, value); 
                child->prev = this;
                left = child;
                cout << "left\n";
                this->left->show();

                if(this->prev->left != this){ // 꺾인 경우
                    if(this->prev->left == NULL){ // 삼촌이 없는 경우
                        if(this->color.compare("red") == 0) child->color = "black";
                        rotationR(this, child);
                        rotationL(child->prev, child);
                    }
                    else{ // 삼촌이 있는 경우
                        if(this->color.compare("red") == 0){
                            this->color = "black";
                            if(this->prev->left->color.compare("red") == 0){ // 삼촌도 red
                                prev->color = "red";
                                this->prev->left->color = "black";
                                // back tracking
                            }
                            else{ // 삼촌은 black
                                rotationR(this, child);
                            }
                        }
                        // this가 black이면 아무 일도 하지 않는다
                    }
                }
                else{ // 안 꺾인 경우
                    if(this->prev->right == NULL){ // 삼촌이 없는 경우
                        if(this->color.compare("red") == 0) this->color = "black";
                        rotationR(this->prev, this);
                    }
                    else{ // 삼촌이 있는 경우
                        if(this->color.compare("red") == 0){ 
                            this->color = "black";
                            if(this->prev->right->color.compare("red") == 0){ // 삼촌도 red
                                this->prev->right->color = "black";
                                this->prev->color = "red";
                                // back tracking
                            }
                            else{ // 삼촌은 black
                                rotationR(this, child);
                            }
                        }
                        // this가 black이면 아무 일도 하지 않는다
                    }

                }
            }
        }
    }
    return;
}
template<typename T1, typename T2>
void my_map<T1, T2>::insert(pair<T1, T2>* p) {
    if(!root) { // 1. first insert : change root information with key and value
        this->first = p->first;
        this->second = p->second;
        this->root = true;
        this->left = NULL;
        this->right = NULL;
        cout << "root\n";
        this->show();
        return;
    }
    this->recursiveInsert(p->first, p->second);
    return;
}

template<typename T1, typename T2>
T2& my_map<T1, T2>::operator[] (T1 key){
/*
key값을 first로 가지는 노드를 찾는다 -> find 이용
만약 없다면? => 새로운 노드를 만들어 삽입한 후, 그 새로운 노드의 second 반환 : insert
만약 있다면? => 그 노드의 second 반환 : 그러면 자동으로 수정됨 : edit
*/
    iterator found = this->find(key);
    if(found == this->end()){ // 새로운 노드 삽입
        this->insert(make_pair(key, 0));
        found = this->find(key);
    }
    return &(found->second);
}
/*
// 미완
// https://gwpaeng.tistory.com/309 : 참고 사이트 보고 다시 구현 해보기
template<typename T1, typename T2>
void my_map<T1, T2>::erase(T1 key){

key값을 가지는 노드를 찾는다 -> find 이용
해당 노드를 삭제해 해당 노드의 자식을 prev에 적절히 연결한다

    iterator found = this->find(key);

    iterator largestR = found->right->begin(); // 오른쪽 서브 트리에서 가장 작은 값으로 자리 대체
    if(largestR->prev->left == largestR) largestR->prev->left = nullptr;
    else if(largestR->prev->right == largestR) largestR->prev->right = nullptr;
    found->first = largestR->first;
    found->second = largestR->second;

    if(found->color.compare("black") == 0){ // 삭제되는 노드의 색상이 black인 경우
        // 이중 흑색 노드인지 검사
        if(largestR->color.compare("black") == 0) { // 4가지 cases 존재
            
            1. found의 형제가 red
            2. found의 형제가 black, found의 형제 자식 모두 black
            3. found의 형제가 black, 형제의 왼쪽 자식이 red, 형제의 오른쪽 자식이 black
            4. found의 형제가 black, found의 형제의 오른쪽 자식이 red
            

        }
        // 대체한 노드가 red인 경우 상관 없다.

    }
    
}
*/
