//
// b_plus_tree.cpp
// COP5536 Adv. Data Struct.
// Created by Yinuo Yang
//

#include "b_plus_tree.hpp"

// Initialize attributes of Pair
Pair::Pair() {
    this->key = 0;
    this->value = 0;
}

// Initialize attributes of BpTree_Node
BpTree_Node::BpTree_Node() {
    this->tree = nullptr;
    this->parent = nullptr;
    this->is_leaf = true;
    this->prev = nullptr;
    this->next = nullptr;
};

// Initialize attributes of BpTree
BpTree::BpTree() {
    this->root = new(BpTree_Node);
    this->root->is_leaf = true;
    this->root->tree = this;
    this->root->m = this->m;
}

//For a given key, find out the corresponding leaf node
BpTree_Node* BpTree_Node::search_leaf(int key) {
    //Already a leaf.
    if (this->is_leaf) {
        return this;
    }
    //Not a leaf. Search in children.
    else {
		unsigned int i = 0;
		while(i < this->keys.size()) {
			if (key < this->keys[i]) {
				return this->children[i]->search_leaf(key);
			}
			i++;
		}
		return this->children.back()->search_leaf(key);
	}
}

//For a given key, find out its position and return the pointer to the corresponding pair
Pair* BpTree_Node::search_key(int key) {
    //Searching in a leaf node
    if (this->is_leaf) {
        unsigned int i = 0;
        while (i < this->data.size()) {
            if (this->data[i]->key == key) {
                return this->data[i];
            }
            i++;
        }
        return nullptr;
    }

    //Search in an internal node
    else {
        return this->search_leaf(key)->search_key(key);
    }
}

//Split a node into two nodes if needed during insertion. 
//Rearrange the relevant keys, pairs, children and parents if necessary
BpTree_Node* BpTree_Node::split() {
    unsigned int mid = this->m / 2;
    auto newNode = new(BpTree_Node);
    newNode->tree = this->tree;
    newNode->parent = this->parent;
    newNode->m = this->m;
    //Split a leaf.
    if (this->is_leaf) {
        //Set new node.
        newNode->is_leaf = true;
        newNode->prev = this;
        newNode->next = this->next;
        this->next = newNode;
        if (newNode->next != nullptr) {
            newNode->next->prev = newNode;
        }
        this->parent->children.push_back(newNode);

        //Transfer half pairs of this node to the new node.
        unsigned int i = 0;
        while (i <= mid) {
            newNode->data.push_back(this->data.back());
            this->data.pop_back();
            i++;
        }

        newNode->sort_data();
        this->parent->keys.push_back(newNode->data.front()->key);
        this->parent->sort_keys();
        this->parent->sort_children();
        if (this->parent->keys.size() > this->m - 1) {
            this->parent->split();
        }

    }
        //Split an internal node.
    else {
        //Set new node.
        newNode->is_leaf = false;
        newNode->prev = this;
        newNode->next = this->next;
        this->next = newNode;
        if (newNode->next != nullptr) {
            newNode->next->prev = newNode;
        }
        this->parent->children.push_back(newNode);
        //Transfer half keys and children to the new node;
        for (int i = 0; i < mid; i++) {
            newNode->keys.push_back(this->keys.back());
            this->children.back()->parent = newNode;
            newNode->children.push_back(this->children.back());
            this->keys.pop_back();
            this->children.pop_back();
        }
        //Always 1 more child than keys.
        newNode->children.push_back(this->children.back());
        this->children.back()->parent = newNode;
        this->children.pop_back();
        newNode->sort_children();
        newNode->sort_keys();
        //Move mid key to the higher level.
        this->parent->keys.push_back(this->keys.back());
        if (this->parent->keys.size() > this->m - 1) {
            this->parent->split();
        }
        this->keys.pop_back();
        this->parent->sort_keys();
        this->parent->sort_children();
    }
    return newNode;
}

//Insert a new pair into a node
void BpTree_Node::insert(Pair* new_pair) {
    //Inserting in a leaf.
    if (this->is_leaf) {
        this->data.push_back(new_pair);
        this->sort_data();
        if (this->data.size() == this->m) {
            this->split();
        }
    }
    //Inserting in a internal node: search for the correct leaf to insert.
    else {
        this->search_leaf(new_pair->key)->insert(new_pair);
        if (this->children.front()->children.size() == this->m + 1) {
            this->children.front()->split();
        }
    }
}

bool BpTree_Node::is_deficient() {
    if (this->tree->root == this) {
        //When root is leaf, only 1 node, cannot be deficient
        if (this->is_leaf) {
            return false;
        }
        //The root is deficient only when having 1 child and no key.
        else {
            return (this->keys.empty());
        }
    }
    else {
        //The leaf is deficient if it doesn't follow btree's degree rules
        if (this->is_leaf) {
            return ((this->m + 1) / 2 - 1) > this->data.size();
        }
        //The internal node is deficient 
        else {
            return ((this->m + 1) / 2) > this->keys.size();
        }
    }
}

//Borrow pairs from the left sibling node
void BpTree_Node::left_borrow() {
    //For leaves
    if (this->is_leaf) {
        this->data.push_front(this->prev->data.back());
        this->prev->data.pop_back();
        //Find out the renewed key in parent node.
        int renewed_key = 0;
        for (unsigned int i = 0; i < this->parent->children.size(); i++) {
            if (this->parent->children[i] == this) {
                renewed_key = i;
                break;
            }
        }
        this->parent->keys[renewed_key - 1] = this->data.front()->key;
    }
    //For internal nodes
    else {
        //Take a key from parent.
        this->keys.push_front(this->parent->keys.back());
        this->parent->keys.pop_back();
        //Parent take a key from left sibling.
        this->parent->keys.push_front(this->prev->keys.back());
        this->prev->keys.pop_back();
        //Move child accordingly.
        this->children.push_front(this->prev->children.back());
        this->prev->children.pop_back();
    }
}

//Borrow pairs from the right sibling pairs
void BpTree_Node::right_borrow() {
    //For leaf
    if (this->is_leaf) {
        this->data.push_back(this->next->data.front());
        this->next->data.pop_front();
        //Find out the renewed key in parent node.
        int renewed_key = 0;
        for (unsigned int i = 0; i < this->parent->children.size(); i++) {
            if (this->parent->children[i] == this) {
                renewed_key = i;
                break;
            }
        }
        if (renewed_key != 0) {
            this->parent->keys[renewed_key - 1] = this->data.front()->key;
        }
            this->parent->keys[renewed_key] = this->next->data.front()->key;
    }
    //For internal node
    else {
        this->keys.push_back(this->parent->keys.front());
        this->parent->keys.pop_front();
        this->parent->keys.push_back(this->next->keys.front());
        this->next->keys.pop_front();
        this->children.push_back(this->next->children.front());
        this->next->children.pop_back();
    }
}

//Merge with left node when left_borrow cannot happen
void BpTree_Node::left_merge() {
    //For leaf left_merge (combine data and delete in-between key in parent)
    if (this->is_leaf) {
        this->prev->data.insert(this->prev->data.end(), this->data.begin(), this->data.end());
        this->prev->next = this->next;
        if (this->next != nullptr) {
            this->next->prev = this->prev;
        }
        for (unsigned int i=0; i < this->parent->children.size(); i++) {
            if (this->parent->children[i] == this) {
                this->parent->children.erase(this->parent->children.begin() + i);
                break;
            }
        }
        this->parent->del(this->data.front()->key);
        free(this);
    }
    //For internal node left_merge (combine keys and delete in-between key in parent)
    else {
        //Find out the corresponding in-between key.
        int bet_key = this->parent->keys.back();
        int renewed_key = this->parent->keys.size() - 1;
        for (unsigned int i = 0; i < this->parent->keys.size(); i++) {
            if (this->parent->keys[i] > this->prev->keys.back()) {
                bet_key = this->parent->keys[i];
                renewed_key = i;
                break;
            }
        }
        this->prev->keys.push_back(bet_key);
        this->parent->children.erase(this->parent->children.begin() + renewed_key + 1);
        this->prev->keys.insert(this->prev->keys.end(), this->keys.begin(), this->keys.end());
        this->prev->children.insert(this->prev->children.end(), this->children.begin(), this->children.end());
        this->prev->next = this->next;
        if (this->next != nullptr) {
            this->next->prev = this->prev;
        }
        this->parent->del(bet_key);
        free(this);
    }
}

//Merge with right node when right_borrow cannot happen
void BpTree_Node::right_merge() {
    if (this->is_leaf) {
        this->next->data.insert(this->next->data.begin(), this->data.begin(), this->data.end());
        this->next->prev = this->prev;
        if (this->prev != nullptr) {
            this->prev->next = this->next;
        }
        for (unsigned int i = 0; i < this->parent->children.size(); i++) {
            if (this->parent->children[i] == this) {
                this->parent->children.erase(this->parent->children.begin() + i);
                break;
            }
        }
        this->parent->del(this->next->data.front()->key);
        free(this);
    }

    else {
        //Find out the corresponding in-between key.
        int bet_key = this->parent->keys.back();
        int renewed_key = this->parent->keys.size() - 1;
        for (unsigned int i=0; i<this->parent->keys.size(); ++i) {
            if (this->parent->keys[i] > this->next->keys.front()) {
                bet_key = this->parent->keys[i-1];
                renewed_key = i;
                break;
            }
        }
        this->next->keys.push_front(bet_key);
        this->parent->children.erase(this->parent->children.begin() + renewed_key);
        this->next->keys.insert(this->next->keys.begin(), this->keys.begin(), this->keys.end());
        this->next->children.insert(this->next->children.begin(), this->children.begin(), this->children.end());
        this->next->prev = this->prev;
        if (this->prev != nullptr) {
            this->prev->next = this->next;
        }

        this->parent->del(bet_key);
        free(this);
    }
}

void BpTree_Node::del_root() {
    this->tree->root = this->tree->root->children.front();
    for (unsigned int i=0; i<this->tree->root->children.size(); ++i) {
        this->tree->root->children[i]->parent = this->tree->root;
    }
    this->tree->root->parent = nullptr;
    free(this);
}

//Delete a pair or a key according to its corresponding key
void BpTree_Node::del(int key) {
    unsigned int lowerBound = (this->m - 1) / 2;
    //Delete in a leaf.
    if (this->is_leaf) {
        for (unsigned int i = 0; i < this->data.size(); i++) {
            if (this->data[i]->key == key) {
                this->data.erase(this->data.begin() + i);
                break;
            }
        }
        //Deficient. Try to borrow first, del higher level later.
        if (this->is_deficient()) {
            //If its left sibling has an extra pair.
            if ((this->prev != nullptr) && (this->prev->parent == this->parent) && (this->prev->data.size() > lowerBound)) {
                this->left_borrow();
            }
            //If its right sibling has an extra pair.
            else if ((this->next != nullptr) && (this->next->parent == this->parent) && (this->next->data.size() > lowerBound)) {
                this->right_borrow();
            }
            //Try to merge with left sibling. If left exists and is sibling.
            else if ((this->prev != nullptr) && (this->prev->parent == this->parent)) {
                this->left_merge();
            }
            //Try to merge with right sibling
            else if ((this->next != nullptr) && (this->next->parent == this->parent)){
                this->right_merge();
            }
            //Deficient root (2-level tree only).
            else if (this->parent != this->tree->root){
                this->del_root();
            }
            //Delete parent to make it deficient as well.
            else {
                this->parent->del(key);
            }
        }
    }
    //Delete in internal node. Called only when children merging.
    else {
        for (unsigned int i=0; i < this->keys.size(); i++) {
            if (this->keys[i] == key) {
                this->keys.erase(this->keys.begin() + i);
                break;
            }
        }
        //Deficient.
        if (this->is_deficient()) {
            if ((this->prev != nullptr) && (this->prev->parent == this->parent) && (this->prev->data.size() > lowerBound)) {
                this->left_borrow();
            }
                //If right is a sibling and has an extra pair.
            else if ((this->next != nullptr) && (this->next->parent == this->parent) && (this->next->data.size() > lowerBound)) {
                this->right_borrow();
            }
                //Try to merge with left sibling node. If left exists and is sibling.
            else if ((this->prev != nullptr) && (this->prev->parent == this->parent)) {
                this->left_merge();
            }
                //Try to merge with right sibling node
            else if ((this->next != nullptr) && (this->next->parent == this->parent)){
                this->right_merge();
            }
            else if (this->parent == nullptr){
                this->del_root();
            }
            else {
                this->parent->del(key);
            }
        }
    }

}

//Compare 2 nodes with smallest pair in each. Called in BpTree_Node::sort_children
bool compare_nodes(BpTree_Node* a, BpTree_Node* b) {
    //Search down to first leaf.
    while (!a->is_leaf) {
        a = a->children.front();
        b = b->children.front();
    }
    //Compare key of first pair.
    return (a->data.front()->key < b->data.front()->key);
}

//Sort the keys in a internal node
void BpTree_Node::sort_keys() {
    sort(this->keys.begin(), this->keys.end());
}

//Sort the children in an internal node.
void BpTree_Node::sort_children() {
    sort(this->children.begin(), this->children.end(), compare_nodes);
}

//Compare 2 pairs with key. Called in BpTree_Node::sort_data.
bool compare_pairs(Pair* a, Pair* b) {
    return (a->key < b->key);
}

//Sort the data list in a leaf node
void BpTree_Node::sort_data() {
    sort(this->data.begin(), this->data.end(), compare_pairs);
}


//Following is the definition for the method in BpTree class


Pair* BpTree::search_key(int key) {
    return this->root->search_key(key);
}

deque<Pair*>* BpTree::search_range(int left, int right) {
    BpTree_Node* left_leaf = this->root->search_leaf(left);
    BpTree_Node* right_leaf = this->root->search_leaf(right);
    Pair* left_pair = left_leaf->data.front();
    Pair* right_pair = right_leaf->data.back();
    auto result = new(deque<Pair*>);
    //small and large are the range of keys
    unsigned int i = 0, small = 0, large = 0;
    while (i < left_leaf->data.size()) {
        if (left_leaf->data[i]->key >= left) {
            small = i;
            left_pair = left_leaf->data[small];
            break;
        }
        else {
            i++;
        }
    }
    i = right_leaf->data.size() - 1;
    while (i > -1) {
        if (right_leaf->data[i]->key <= right) {
            large = i;
            right_pair = right_leaf->data[large];
            break;
        }
        else {
            i--;
        }
    }
    //Push search result in deque.
    BpTree_Node* cur_node = left_leaf;
    Pair* cur_pair = left_pair;
    int cur_pos = small;
    while (cur_pair != right_pair) {
        result->push_back(cur_pair);
        if (cur_pair != cur_node->data.back()) {
            cur_pos++;
            cur_pair = cur_node->data[cur_pos];
        }
        else {
            cur_node = cur_node->next;
            cur_pos = 0;
            cur_pair = cur_node->data.front();
        }
    }
    result->push_back(cur_pair);
    return result;
}


void BpTree::insert(Pair* new_pair) {
    this->grow();
    this->root->insert(new_pair);
    if (this->root->keys.empty()) {
        this->shrink();
    }
}

void BpTree::del(int key) {
    auto targetLeaf = this->root->search_leaf(key);
    auto targetPair = targetLeaf->search_key(key);
    //Not found.
    if (targetPair == nullptr) {
        return;
    }
    //Start deleting from leaf and do a bottom-up maintenance.
    else {
        targetLeaf->del(key);
    }
}

void BpTree::grow() {
    auto new_root = new(BpTree_Node);
    new_root->tree = this;
    new_root->m = this->m;
    new_root->is_leaf = false;
    new_root->children.push_back(this->root);
    this->root->parent = new_root;
    this->root = new_root;
}

void BpTree::shrink() {
    BpTree_Node* tmp = this->root;
    this->root = this->root->children.front();
    this->root->parent = nullptr;
    free(tmp);
}
