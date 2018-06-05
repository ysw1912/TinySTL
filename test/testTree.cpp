/*************************************************************************
    > File Name: testTree.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-06-05
*************************************************************************/

#include <iostream>
#include <functional>

#include "../src/tree.h"
#include "print.h"

using namespace std;

template <class T>
struct identity : public unary_function<T, T> {
    const T& operator()(const T& x) const { return x; }
};

template <class Key, class Val, class KeyOfValue, class Compare>
void PrintRb(const STL::rb_tree<Key, Val, KeyOfValue, Compare> rbt) {
    cout << "size: " << rbt.size() << "  { ";
    for (auto it = rbt.begin(); it != rbt.end(); ++it) {
        cout << *it << '(' << it.node->color << ") ";
    }
    cout << '}' << endl;
}

int main() {
    STL::rb_tree<int, int, identity<int>, less<int>> itree;
    itree.insert_equal(10);
    itree.insert_unique(7);
    itree.insert_unique(8);
    itree.insert_unique(15);
    itree.insert_unique(5);
    itree.insert_unique(6);
    itree.insert_unique(11);
    itree.insert_unique(13);
    itree.insert_unique(12);

    STL::rb_tree<int, int, identity<int>, less<int>> itree2;
    PrintRb(itree);
    PrintRb(itree2);
    itree2.swap(itree);
    PrintRb(itree);
    PrintRb(itree2);
    itree2.swap(itree);


    itree2.insert_unique(1000);
    itree2.insert_unique(200);
    itree2.insert_unique(500);

    PrintRb(itree);
    PrintRb(itree2);
    itree2.swap(itree);
    PrintRb(itree);
    PrintRb(itree2);
    return 0;
}
