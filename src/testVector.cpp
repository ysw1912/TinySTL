/*************************************************************************
    > File Name: testVector.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-05-22
*************************************************************************/
#include "vector.h"
#include <iostream>

using namespace std;

struct foo {
    int a;
    int b;

    foo() { cout << "ctor" << endl; }
    ~foo() { cout << "dtor" << endl; }
};

int main() {
    STL::vector<foo> v;
    for (int i = 0; i < 6; ++i) {
        foo t;
        t.a = i;    t.b = i + 1;
        v.push_back(t);
    }
    for (int i = 0; i < 6; ++i) {
        cout << v[i].a << ' ' << v[i].b << endl;
    }
    cout << endl;
    return 0;
}
