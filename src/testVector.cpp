/*************************************************************************
    > File Name: testVector.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-05-22
*************************************************************************/
#include "vector.h"
#include <iostream>

using namespace std;

int main() {
    int n = 9;
    STL::vector<int> v(n, 3);
    STL::vector<int> v2(v);
    for (int i = 0; i < 9; ++i) {
        cout << v2[i] << ' ';
    }
    cout << endl;
    cout << v2.capacity() << endl;
    v.~vector();
    return 0;
}
