/*************************************************************************
    > File Name: testSet.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-06-08
*************************************************************************/

#include <iostream>

#include "../src/set.h"
#include "print.h"

using STL::set;
using STL::advance;

int main() {
    set<int> s1{1, 12, 76, 135, 789, 4312}, s3{987, 123, 456};
    auto it1 = s1.begin(), it2 = s1.begin();
    advance(it1, 1);
    advance(it2, 4);
    Print(s1);
    cout << *s1.upper_bound(135) << endl;
    return 0;
}
