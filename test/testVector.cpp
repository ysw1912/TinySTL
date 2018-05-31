/*************************************************************************
    > File Name: testVector.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-05-22
*************************************************************************/
#include "../src/vector.h"
#include <iostream>
#include <string>
#include <vector>
#include "print.h"
using namespace std;

int main() {
    STL::vector<foo> v1, v2;
    for (int i = 1; i <= 5; ++i) {
        v1.push_back({double(i), string(2, i % 10 + '0')});
    }
    foo t = {99, "99"};
    v1.emplace(v1.begin(), double(99), "999");
    v2.emplace(v2.begin(), t);
    Print(v1); 
    Print(v2);
    v1.swap(v2);
    Print(v1); 
    Print(v2);

    return 0;
}
