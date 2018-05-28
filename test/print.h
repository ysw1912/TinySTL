#ifndef PRINT_H_
#define PRINT_H_ 

#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::ostream;
using std::string;

struct foo {
    double d;
    string s;

    bool operator < (const foo& x) const {
        return d < x.d || (d == x.d && s < x.s);
    }
};

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

#endif
