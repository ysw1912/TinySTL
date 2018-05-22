/*************************************************************************
    > File Name: testVector.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-05-22
*************************************************************************/
#include "vector.h"
#include <iostream>
#include <cstring>
using namespace std;

struct foo {
    int a;
    int b;
    char *s;

    ~foo() {
        if (s) {
            delete[] s;
            cout << "delete[] s" << endl;
        }
        cout << "析构" << endl;
    }
};

int main() {
    char a[] = "ysw";
    STL::vector<foo> v;
    for (int i = 0; i < 6; ++i) {
        foo t;
        t.a = i;    t.b = i + 1;
        t.s = new char[4];
        strcpy(t.s, a);
        v.push_back(t);
    }
    for (int i = 0; i < 6; ++i) {
        cout << v[i].a << ' ' << v[i].b << ' ' << v[i].s << endl;
    }
    cout << endl;
    v.~vector();
    return 0;
}
