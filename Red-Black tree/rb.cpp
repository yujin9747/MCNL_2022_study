#include <iostream>
using namespace std;
/*
BST를 전제로 한다

5가지 속성
1. 모든 노드는 red 혹은 black
2. 루트 노드는 black
3. 모든 leaf 노드는 black
4. red의 자녀들은 black
5. 임의의 노드에서 nill까지 가는 경로에 black 수는 동일하다

bh : black height -> 5번 속성 전제로 해서 구한 값. 
*/
struct node{
    string color;
    int value;
    node* left;
    node* right;
    node() : color("root"), left(), right(), value(){} 
    node(int v) : color("red") ,left(nullptr) ,right(nullptr){
        value = v;
    }
};
/*
삽입 후, red가 중복되어 한 방향으로 쏠려 있는 경우, 꺾인 채로인지 아닌지 반환
ex. black->red(left)->red(left)
*/
bool straight(node* pOfparent, node* parent, int leftOrRight){
    // pOfparent->color = "red";
    // parent->color = "black";
    if(leftOrRight == 0){ // left 편향
        if(pOfparent->value > parent->value) return true;
        else return false;
    }
    // right 편향
    if(pOfparent->value < parent->value) return true;
    return false;
}

/*
오른쪽/왼쪽으로 rotation
*/
void rotationL(node* pOfparent, node* newParent){
    node* temp = newParent->left;
    newParent->left = pOfparent;
    pOfparent->right = temp;
}
void rotationR(node* pOfparent, node* newParent){
    node* temp = newParent->right;
    newParent->right = pOfparent;
    pOfparent->left = temp;
}

/*
할아버지와 부모의 색상 변경
*/
void colorChange(node* pOfparent, node* parent){
    pOfparent->color = "red";
    parent->color = "black";
}

/*
newNode의 적절한 위치를 찾아 연결 및 RB tree 조건 충족 확인
*/
node* insertBST(node* parent, node* newNode, node* pOfparent){
    if(parent->value == newNode->value) {
        cout << "중복!\n";
        return parent; // 중복 값을 허용하지 않는다.
    }
    if(parent->value > newNode->value) { // 왼쪽
        if(parent->left == nullptr) { // 왼쪽이 비어 있는 경우
            parent->left = newNode;
            // 부모가 red인 경우
            if(parent->color.compare("red") == 0){
                if(straight(pOfparent, parent, 0)) { // 1. 부모가 할아버지의 왼쪽에 연결된 경우(안꺾임)
                    // 삼촌 존재, 색상 여부에 따라
                    if(pOfparent->right != nullptr && pOfparent->right->color.compare("red") == 0){
                        pOfparent->color = "red";
                        parent->color = "black";
                        pOfparent->right->color = "black"; // 할아버지에서 부터 다시 확인하도록 코딩하기
                    }
                    else { 
                        if(pOfparent->right != nullptr && pOfparent->right->color.compare("black") == 0)
                            colorChange(pOfparent, parent);
                        //rotationR(pOfparent, parent);
                    }
                    rotationR(pOfparent, parent);
                }
                else { // 2. 부모가 할아버지의 오른쪽에 연결된 경우(꺾임) 
                    // 부모 기준으로 오른쪽으로 rotation 후에, 삼촌 존재, 색상 여부에 따라 색상 바꾼 후, 왼쪽으로 rotation
                    rotationR(parent, newNode); 
                    if(pOfparent->left != nullptr && pOfparent->left->color.compare("black") == 0) 
                        colorChange(pOfparent, parent);
                    rotationL(pOfparent, parent);
                }
            }
            // 부모가 red가 아닌 경우, 그대로 종료
            //cout << "left에 연결\n";
            return parent;
        }
        else return insertBST(parent->left, newNode, parent); // 왼쪽 노드가 존재하는 경우
    }
    else { // 오른쪽
        if(parent->right == nullptr) {  // 오른쪽이 비어 있는 경우
            parent->right = newNode;
            // 부모가 red인 경우
            if(parent->color.compare("red") == 0){
                if(straight(pOfparent, parent, 1)) { // 1. 부모가 할아버지의 오른쪽에 연결된 경우(안꺾임)
                    // 삼촌 존재, 색상 여부에 따라
                    if(pOfparent->left != nullptr && pOfparent->left->color.compare("red") == 0){
                        pOfparent->color = "red";
                        parent->color = "black";
                        pOfparent->left->color = "black"; // 할아버지에서 부터 다시 확인하도록 코딩하기
                    }
                    else{
                        if(pOfparent->left != nullptr && pOfparent->left->color.compare("black") == 0)
                            colorChange(pOfparent, parent);
                        rotationL(pOfparent, parent);
                    }
                }
                else { // 2. 부모가 할아버지의 왼쪽에 연결된 경우(꺾임) 
                    // 부모 기준으로 왼쪽으로 rotation 후에, 삼촌 존재, 색상 여부에 따라 색상 바꾼 후, 오른쪽으로 rotation
                    rotationL(parent, newNode);
                    if(pOfparent->right != nullptr && pOfparent->right->color.compare("black") == 0)
                        colorChange(pOfparent, parent);
                    rotationR(pOfparent, parent);
                }
            }
            //cout << "right에 연결\n";
            return parent;
        }
        else return insertBST(parent->right, newNode, parent); // 오른쪽 노드가 존재하는 경우
    }
}
/*
insert 후, root 반환
*/
node* insert(node* root, int value){
    node* newNode = new node(value);

    // Case 1. 처음 삽입 : root인 경우
    if(root->color.compare("root") == 0) {
        newNode->color = "black";
        return newNode; // new root
        //cout << "Case 1. 루트 노드는 black 속성 충족\n";
    }
    // root가 아닌 경우
    // BST기반 적절한 위치에 노드 삽입
    return insertBST(root, newNode, root); //정상 작동
}

int main(){
    node* root = new node();
    root = insert(root, 10);
    cout << root->value << " " << root->color << "\n";
    insert(root, 3); 
    cout << root->left->value << " " << root->left->color<< "\n";
    insert(root, 1); 
    cout << root->left->left->value << " " << root->left->left->color<< "\n";
    // insert(root, 15); 
    // cout << root->right->value << " " << root->right->color<< "\n";
    // insert(root, 10); 
    // root->right->color = "black";
    // insert(root, 17);
    // cout << root->right->value << " " << root->right->color;
    // insert(root, 1);
    // cout << root->value;
    // cout << " " << root->left->value << " " << root->right->value << " " << root->right->right->value << "\n";
    return 0;
}