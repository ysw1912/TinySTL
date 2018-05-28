/*************************************************************************
    > File Name: main.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-05-28
*************************************************************************/

#include <iostream>
#include <list>
#include "../test/print.h"
using namespace std;

int main() {
    list<int> l1{1, 2, 3, 4, 5};
    list<int> l2{9, 10, 11, 12, 13};
    Print(l1);
    Print(l2);
    l1.swap(l2);
    Print(l1);
    Print(l2);
    return 0;
}
