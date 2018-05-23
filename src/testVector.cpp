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

template <class T>
void Print(const STL::vector<T> &v) {
    for (size_t i = 0; i < v.size(); ++i) {
        cout << v[i] << ' ';
    }
    cout << endl;
}

int main() {
    STL::vector<int> v1 = {1, 2, 3, 4, 5};
    v1.insert(v1.begin() + 1, (size_t)5, 9);
    Print<int>(v1);
    return 0;
}
