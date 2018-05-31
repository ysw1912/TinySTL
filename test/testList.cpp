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
    STL::list<foo> l1;
    for (int i = 1; i <= 4; ++i) {
        l1.push_back({double(i), string(i, i + '0')});
    }
    Print(l1);
    STL::list<foo> l2(std::move(l1));
    Print(l1);
    Print(l2);
    cout << "------------------------" << endl;
    STL::list<int> l;
    srand(time(0));
    for (int i = 0; i < 100; ++i) {
        l.push_back(rand() % 100 + 1);
    }
    Print(l);
    l.sort();
    Print(l);
    return 0;
}
