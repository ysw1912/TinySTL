/*************************************************************************
    > File Name: testVector.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-05-22
*************************************************************************/
#include "../src/vector.h"
#include <iostream>
#include <string>
using namespace std;

struct aaa {
    int a;
    string s;
};

ostream& operator<< (ostream &os, const aaa &x) {
    os << x.a << ' ' << x.s;
    return os;
}

template <class T>
void Print(const STL::vector<T> &v) {
    cout << "capacity: " << v.capacity() << "  { ";
    for (size_t i = 0; i < v.size(); ++i) {
        cout << v[i] << ' ';
    }
    cout << '}' << endl;
}

int main() {
    STL::vector<aaa> v1;
    for (int i = 0; i < 5; ++i) {
        aaa t = {i + 1, "yyf"};
        v1.push_back(t);
        Print<aaa>(v1);
    }
    aaa t = {0, "ysw"};
    v1.insert(v1.begin() + 1, 4, t);
    Print<aaa>(v1);
    v1.erase(v1.begin() + 2, v1.begin() + 4);
    Print<aaa>(v1);
    return 0;
}
