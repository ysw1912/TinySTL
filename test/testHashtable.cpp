/*************************************************************************
    > File Name: testHashtable.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-06-11
*************************************************************************/

#include <iostream>
#include "../src/hashtable.h"

using std::cout;
using std::endl;
using hashtable = STL::hashtable<int, int, std::hash<int>, std::_Identity<int>, std::equal_to<int>>;

void PrintHt(const hashtable& ht) {
    cout << "bucket_count: " << ht.bucket_count() << endl;
    cout << "size: " << ht.size() << "  { ";
    auto it = ht.begin();
    for (int i = 0; i < int(ht.size()); ++i, ++it)
        cout << *it << ' ';
    cout << '}' << endl;

    for (int i = 0; i < int(ht.bucket_count()); ++i) {
        int n = int(ht.bucket_size(i));
        if (n)
            printf("bucket[%02d] 有 %02d 个节点\n", i, n);
    }
}


int main() {
    hashtable ht(50);
    ht.insert_unique(59);
    ht.insert_unique(63);
    ht.insert_unique(108);
    ht.insert_unique(2);
    ht.insert_unique(53);
    ht.insert_unique(55);
    
    for (int i = 0; i <= 47; ++i) {
        ht.insert_equal(i);
    }
    PrintHt(ht);
    
    ht.erase(108);
    PrintHt(ht);

    return 0;
}
