/*************************************************************************
    > File Name: testTree.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-06-05
*************************************************************************/

#include <iostream>
#include <functional>
#include "/usr/include/c++/5.4.0/bits/stl_tree.h"

#include "../src/tree.h"
#include "print.h"

using std::_Rb_tree;

using namespace std;

template <class T>
struct identity : public unary_function<T, T> {
    const T& operator()(const T& x) const { return x; }
};

template <class Key, class Val, class KeyOfValue, class Compare>
void PrintRb(const STL::rb_tree<Key, Val, KeyOfValue, Compare> rbt) {
    int count = 0;
    printf("size: %02d  { ", int(rbt.size()));
    for (auto it = rbt.begin(); it != rbt.end(); ++it) {
        cout << '[' << count << ']' << *it << '(' << (it.node->color? "黑" : "红") << ") ";
        ++count;
    }
    cout << '}' << endl;
}

template <class Key, class Val, class KeyOfValue, class Compare>
void PrintRb(const _Rb_tree<Key, Val, KeyOfValue, Compare> rbt) {
    int count = 0;
    printf("size: %02d  { ", int(rbt.size()));
    for (auto it = rbt.begin(); it != rbt.end(); ++it) {
        cout << '[' << count << ']' << *it << '(' << (it._M_node->_M_color? "黑" : "红") << ") ";
        ++count;
    }
    cout << '}' << endl;
}
const int N = 12;

int main() {
    std::srand(std::time(0));
    STL::rb_tree<int, int, std::_Identity<int>, less<int>> rbt;
    _Rb_tree<int, int, std::_Identity<int>, less<int>> Rbt;
    for (int i = 0; i < N; ++i) {
        int val = get_rand(1, 99);
        rbt.insert_unique(val);
        Rbt._M_insert_unique(val);
        printf("插入%02d          ", val);
        PrintRb(rbt);
        printf("                ");
        PrintRb(Rbt);
    }
    cout << "初始状态        ";   PrintRb(rbt);
    int size = rbt.size();
    for (int i = 0; i < size; i++) {
        auto it = rbt.begin();
        auto It = Rbt.begin();
        int offset = get_rand(0, rbt.size() - 1);
        STL::advance(it, offset);
        std::advance(It, offset);
        printf("移除第%02d个节点  ", offset);
        rbt.erase(it);
        PrintRb(rbt);
        Rbt.erase(It);
        printf("                ");
        PrintRb(Rbt);
    }
    return 0;
}
