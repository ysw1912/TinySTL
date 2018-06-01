/*************************************************************************
    > File Name: testList.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-05-25
*************************************************************************/

#include <iostream>
#include <cstdlib>
#include <ctime>

#include "../src/list.h"
#include "print.h"

using namespace std;

int main() {
    STL::list<foo> l1, l2;
    for (int i = 1; i <= 4; ++i) {
        l1.push_back({double(i), string(i, i + '0')});
        l2.push_back({double(i), string(i, i + '1')});
    }
    Print(l1);  Print(l2);
    auto it2 = l2.cbegin();
    STL::advance(it2, 3);
    l1.splice(l1.cend(), l2, l2.begin(), it2);
    l2.emplace_back(9999, "88888");
    l2.emplace_front(11111, "yswysw");
    Print(l1);  Print(l2);
    cout << "------------------------" << endl;
    STL::list<int> l, ll;
    srand(time(0));
    for (int i = 0; i < 15; ++i) {
        l.push_back(rand() % 100 + 1);
        ll.push_back(rand() % 100 + 1);
    }
    Print(l); Print(ll);
    l.sort(); ll.sort();
    Print(l); Print(ll);
    l.merge(std::move(ll));
    Print(l); Print(ll);
    return 0;
}
