/*************************************************************************
    > File Name: testList.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-05-25
*************************************************************************/

#include <iostream>

#include "../src/list.h"
#include "print.h"

using namespace std;

int main() {
    STL::list<int> l;
    for (int k = 1; k <= 4; ++k) {
        for (int i = 0; i < k; ++i) {
            l.push_back(k);
        }
        Print(l);
    }
    l.remove(3);
    Print(l);
    l.unique();
    Print(l);
    l.clear();
    Print(l);
    return 0;
}
