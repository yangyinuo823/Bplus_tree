//
// main.cpp
// COP5536 Adv. Data Struct.
// Created by Yinuo Yang
//

#include <iostream>
#include <fstream>
#include <string>
#include "b_plus_tree.hpp"

using namespace std;

int main(int argc, char** argv) {
    string fileName = argv[1];
    ifstream inFile;
    ofstream outFile;
    inFile.open(fileName, ios::in);
    outFile.open("output_file.txt", ios::out);
    string line;
    auto new_tree = new(BpTree);
    while (getline(inFile, line)) {
        if (!line.compare(0, 10, "Initialize")) {
            int left_pos = line.find('(');
            int right_pos = line.find(')');
            new_tree->m = stoi(line.substr(left_pos + 1, right_pos - left_pos - 1));
            new_tree->root->m = new_tree->m;
        }
        else if (!line.compare(0, 6, "Insert")) {
            int left_pos = line.find('(');
            int middle_pos = line.find(',');
            int right_pos = line.find(')');
            int new_key = stoi(line.substr(left_pos + 1, middle_pos - left_pos - 1));
            float new_value = stof(line.substr(middle_pos + 1, right_pos - middle_pos - 1));
            auto new_pair = new(Pair);
            new_pair->key = new_key;
            new_pair->value = new_value;
            new_tree->insert(new_pair);
        }
        else if (!line.compare(0, 6, "Delete")) {
            int left_pos = line.find('(');
            int right_pos = line.find(')');
            int del_key = stoi(line.substr(left_pos + 1, right_pos - left_pos - 1));
            new_tree->del(del_key);
        }
        else if (!line.compare(0, 6, "Search")) {
            int left_pos = line.find('(');
            int middle_pos = line.find(',');
            int right_pos = line.find(')');
            //Single search
            if (middle_pos == -1) {
                int search_key = stoi(line.substr(left_pos + 1, right_pos - left_pos - 1));
                Pair* search_pair = new_tree->search_key(search_key);
                if (search_pair == nullptr) {
                    outFile<<"Null"<<endl;
                }
                else {
                    outFile<<search_pair->value<<endl;
                }
            }
            //Range search
            else {
                int search_left_key = stoi(line.substr(left_pos + 1, middle_pos - left_pos - 1));
                int search_right_key = stoi(line.substr(middle_pos + 1, right_pos - middle_pos - 1));
                deque<Pair*>* search_deque = new_tree->search_range(search_left_key, search_right_key);
                if (search_deque->empty()) {
                    outFile<<"Null"<<endl;
                }
                else {
                    while (!search_deque->empty()) {
                        Pair* cur_pair = search_deque->front();
                        outFile<<cur_pair->value<<',';
                        search_deque->pop_front();
                    }
                    outFile<<endl;
                }
            }
        }

    }
    inFile.close();
    outFile.close();
}
