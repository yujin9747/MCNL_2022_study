#include <iostream>
#include <cstring>

using namespace std;

template <typename T1, typename T2> class my_map{
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
        my_map(T1 key, T2 value): first(key), second(value), color("red"), left(nullptr), right(nullptr) {}
        ~my_map(){}
        void show() const{ cout << "key : " << first <<  "\nvalue : " << second << "\n";}
        void insert(T1 key, T2 value);
        void edit(T1 new_key, T2 new_value);
        void rotationR(my_map* toRight, my_map* newParent);
        void rotationL(my_map* toLeft, my_map* newParent);
        my_map<string, int>* recursiveInsert(T1 key, T2 value);
}; 
template<typename T1, typename T2>
void my_map<T1, T2>::edit(T1 new_key, T2 new_value){
    this->first = new_key;
    this->second = new_value;
}
template<typename T1, typename T2>
void my_map<T1, T2>::rotationL(my_map* toLeft, my_map* newParent){
    my_map* temp = newParent->left; // 위로 올라 갈 새로운 부모
    newParent->left = toLeft; // toLeft = newParent의 부모였지만 왼쪽 아래로 내려가는 노드
    toLeft->right = temp; // 기존에 newParent가 연결되어 있던 곳에 newParent의 기존 left 연결
    toLeft->prev = newParent;
    newParent->prev = toLeft;
}
template<typename T1, typename T2>
void my_map<T1, T2>::rotationR(my_map* toRight, my_map* newParent){
    my_map* temp = newParent->right;
    newParent->right = toRight;
    toRight->left = temp;
    toRight->prev = newParent;
    newParent->prev = toRight;
}
template<typename T1, typename T2>
my_map<string, int>* my_map<T1, T2>::recursiveInsert(T1 key, T2 value){
    int strcompare = strcmp(this->first.c_str(), key.c_str());
    if(strcompare == 0){ // first == key : 중복
        cout << "same!\n";
        return this;
    }
    else{ // 중복이 아닌 경우
        if(strcompare < 0){ // first < key 오른쪽 자손이 들어온 경우
            if(right != nullptr) right->recursiveInsert(key, value); // recursive call ; child를 적절한 위치에 삽입한 right가 반환됨
            else{
                my_map<T1, T2> child(key, value); 
                child.prev = this;
                right = &child;
                cout << "right\n";
                this->right->show();

                if(color.compare("black") == 0) return this; // 2. parent(this) color == black
                else if(color.compare("red") == 0) { // 3. parent(this) color == red
                    color = "black";
                    this->prev->color = "red";
                    if(this->prev->right == this){ // 안 꺾인 경우
                        if(this->prev->left == nullptr || this->prev->left->color.compare("black") == 0) {
                            rotationL(this->prev, this);
                            return this;
                        }
                    }
                    // return 안 된 경우 : 안꺾였는데 삼촌이 red or 꺾인 경우
                    this->prev->left->color = "black";
                    if(this->prev->root) this->prev->color = "black";
                    return this;
                }
            }
        }
        else{ // first > key 왼쪽 자손이 들어온 경우
            if(left != nullptr) left->recursiveInsert(key, value); // recursive call ; child를 적절한 위치에 삽입한 left가 반환됨
            else{
                my_map<T1, T2> child(key, value); 
                child.prev = this;
                left = &child;
                cout << "left\n";
                this->left->show();

                if(color.compare("black") == 0) return this; // 2. parent(this) color == black
                else if(color.compare("red") == 0) { // 3. parent(this) color == red
                    color = "black";
                    this->prev->color = "red";
                    if(this->prev->left == this){ // 안 꺾인 경우
                        if(this->prev->right == nullptr || this->prev->right->color.compare("black") == 0) {
                            rotationR(this->prev, this);
                            return this;
                        }
                    }
                    // return 안 된 경우 : 안꺾였는데 삼촌이 red or 꺾인 경우
                    this->prev->right->color = "black";
                    if(this->prev->root) this->prev->color = "black";
                    return this;
                }
            }
        }
    }
    return this;
}
template<typename T1, typename T2>
void my_map<T1, T2>::insert(T1 key, T2 value) {
    if(!root) { // 1. first insert : change root information with key and value
        this->first = key;
        this->second = value;
        this->root = true;
        this->left = nullptr;
        this->right = nullptr;
        cout << "root\n";
        this->show();
        return;
    }
    this->recursiveInsert(key, value);
    return;
}

int main(){
    //my_map<string, int>* map = new my_map<string, int>(); // defalut constructor -> root of rb tree
    /* 기본 기능 테스트 : root의 직계 자식 insert (root의 key는 yujin으로 가정) 
    map.edit("yujin", 10); 
    map.insert("yujin", 20); // same!
    map.insert("jang", 15); // left
    map.insert("zoo", 90); // right
    */
    
    /* insert case 1. first insert tesst
    map.insert("first input", 1);
    map.show();
    */
   my_map<string, int> map;
   map.insert("dong", 10);
   map.insert("apple", 12);
   map.insert("boint", 90);
   map.insert("function", 14);
   map.show();

   // 해결할 문제점 : 위의 예시 대로 진행하면 root는 boint여야 함. 
   // boint 들어갔을 때, apple의 오른 쪽에 삽입 되었다가 apple 기준으로 왼쪽으로 돌리고, 그 다음에 조상 기준으로 오른쪽으로 돌려야 함. 
   // 로직에 있는 허점 잡아서 수정하기
   



    return 0;
}