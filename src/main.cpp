/*************************************************************************
    > File Name: main.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-05-17
*************************************************************************/

#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

template <class T>
struct display {
    void operator() (const T &x) {
        cout << x << ' ';
    }
};

int main() {
    vector<int> v;
    for (int i = 1; i < 5; ++i)
        v.push_back(i + 1);
    v.insert(v.begin() + 1, 5, 9);
    std::for_each(v.begin(), v.end(), display<int>());
    return 0;
}
