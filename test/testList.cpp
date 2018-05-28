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
    STL::list<foo> l1, l2;
    for (int i = 1; i <= 4; ++i) {
        l1.push_back({double(i), string(i, i + '0')});
        l2.push_back({double(i), string(i, i + '1')});
    }
    l1.splice(l1.end(), l2);
    Print(l1);
    l1.sort();
    Print(l1);
    return 0;
}
