/*************************************************************************
    > File Name: main.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-05-17
*************************************************************************/

#include <iostream>
#include <algorithm>
#include <deque>
using namespace std;

template <class T>
struct display {
    void operator() (const T &x) {
        cout << x << ' ';
    }
};

int main() {
    int a[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    deque<int> d(a, a + 9);

    deque<int>::iterator first = d.begin() + 2;
    deque<int>::iterator last = d.end() - 2;
    deque<int>::iterator result = d.begin() + 4;
    cout << *first << ' ' << *last << ' ' << *result << endl;

    copy(first, last, result);
    for_each(d.begin(), d.end(), display<int>());
    cout << endl;
    return 0;
}
