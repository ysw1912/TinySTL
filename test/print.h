#ifndef PRINT_H_
#define PRINT_H_ 

#include <iostream>
using std::cout;
using std::endl;

template <class Container>
void Print(Container c) {
    cout << "size: " << c.size() << "  { ";
    for (auto it = c.begin(); it != c.end(); ++it) {
        cout << *it << ' ';
    }
    cout << '}' << endl;
}

#endif
