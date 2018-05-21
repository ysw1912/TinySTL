/*************************************************************************
    > File Name: main.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-05-17
*************************************************************************/

#include <iostream>
#include "construct.h"
using namespace std;

struct A {
    A() {
        cout << "A ctor!" << endl;
    }
    ~A() {
        cout << "A dtor!" << endl;
    }
};

int main() {
    A *a = (A *)malloc(3 * sizeof(A));
    for (int i = 0; i < 3; ++i) {
        STL::construct(a + i);
    }
    STL::destroy(a + 1, a + 3);
    return 0;
}
