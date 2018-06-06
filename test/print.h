#ifndef PRINT_H_
#define PRINT_H_ 

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
using std::cout;
using std::endl;
using std::ostream;
using std::string;

struct foo {
    double d;
    string s;

    foo(double dd, string ss) : d(dd), s(ss) { }

    bool operator < (const foo& x) const {
        return d < x.d || (d == x.d && s < x.s);
    }
};

auto cmp = [](const foo& a, const foo& b) {
    return a.d > b.d || (a.d == b.d && a.s > b.s); };

auto eq = [](const foo& a, const foo& b) { return a.d == b.d; };

ostream& operator<<(ostream& os, const foo& x) {
    os << '(' << x.d << ' ' << x.s << ')';
    return os;
}

template <class Container>
void Print(Container c) {
    cout << "size: " << c.size() << "  { ";
    for (auto it = c.begin(); it != c.end(); ++it) {
        cout << *it << ' ';
    }
    cout << '}' << endl;
}

// 获得[a, b]的随机整数
inline int get_rand(int a, int b) {
    return std::rand() % (b - a + 1) + a;
}

#endif
