/*************************************************************************
    > File Name: testVector.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-05-22
*************************************************************************/
#include "../src/vector.h"
#include <iostream>
#include <string>
#include "print.h"
using namespace std;

int main() {
    STL::vector<foo> v1;
    for (int i = 1; i <= 9; ++i) {
        v1.push_back({double(i), string(i, i + '0')});
    }
    Print(v1);
    STL::vector<foo> v2(v1.end() - 2, v1.end());
    for (int i = 1; i <= 7; ++i) {
        v2.push_back({double(i + 20), string(i, i + '2')});
    }
    // Print(v2);
    v1.erase(v1.end() - 2);
    Print(v1);
    cout << *v1.erase(v1.cbegin() + 2, v1.cbegin() + 5) << endl;
    Print(v1);
    v1.erase(v1.end() - 1);
    Print(v1);
    return 0;
}
