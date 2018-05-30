/*************************************************************************
    > File Name: main.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-05-28
*************************************************************************/

#include <iostream>
#include <list>
#include <vector>
#include "../test/print.h"
using namespace std;

int main() {
    vector<int> v1{1, 2, 3, 4, 5};
    Print(v1);
    vector<int> v2;
    v1.reserve(8);
    int i = 9;
    v1.insert(v1.end() - 1, i);
    Print(v1);
    return 0;
}
