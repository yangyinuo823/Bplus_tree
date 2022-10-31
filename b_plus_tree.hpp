//
// b_plus_tree.hpp
// COP5536 Adv. Data Struct.
// Created by Yinuo Yang
//

#include <deque>
#include <algorithm>

using namespace std;

class BpTree_Node;
class Pair{
public:
    int key;
    float value;

    Pair();
};

class BpTree{
public:
    BpTree_Node* root;
    unsigned int m; // degree of the tree

    BpTree();
    Pair* search_key(int);
    deque<Pair*>* search_range(int, int);
    void insert(Pair*);
    void del(int);
    void grow();//increase 1 level of height
    void shrink();//decrease 1 level of height
};

class BpTree_Node{
public:
    unsigned int m; //same as degree of the tree
    BpTree* tree;
    BpTree_Node* parent;
    deque<BpTree_Node*> children;
    bool is_leaf;
    BpTree_Node* prev;
    BpTree_Node* next;
    deque<unsigned int> keys;
    deque<Pair*> data;

    BpTree_Node();
    BpTree_Node* search_leaf(int);
    Pair* search_key(int);
    BpTree_Node* split();
    void insert(Pair*);
    bool is_deficient();
    void left_borrow();
    void right_borrow();
    void left_merge();
    void right_merge();
    void del_root();
    void del(int);
    void sort_keys();
    void sort_children();
    void sort_data();
};


bool compare_pairs(Pair*, Pair*);
bool compare_nodes(BpTree_Node*, BpTree_Node*);

