/*************************************************************************
    > File Name: testHashtable.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-06-11
*************************************************************************/
#include "../STL/hashtable.h"
#include "test_util.h"

using hashtable = STL::hashtable<int, int, std::hash<int>, std::_Identity<int>, std::equal_to<int>>;

void PrintHt(const hashtable& ht)
{
    cout << "load_factor: " << ht.load_factor() << endl;
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

// 构造/拷贝构造函数、拷贝赋值操作符
void test_case1()
{
    cout << "<test_case01>" << endl;
    
    hashtable ht1(50);
    for (int i = 0; i < 100; ++i)
        ht1.insert_unique(i);
    
    hashtable ht2(ht1), ht3(50);
    assert(Container_Equal(ht1, ht2));

    ht3 = ht1;
    assert(Container_Equal(ht1, ht3));
}

// 迭代器
void test_case2()
{
    cout << "<test_case02>" << endl;
    
    hashtable ht(50);
    int i;
    for (i = 0; i < 1000; ++i)
        ht.insert_unique(i);

    i = 0;
    for (hashtable::const_iterator it = ht.begin(); it != ht.end(); ++it, ++i)
        assert(*it == i);
}

// 容量
void test_case3()
{
    cout << "<test_case03>" << endl;
    
    hashtable ht(50);
    for (int i = 0; i < 1000; ++i)
        ht.insert_unique(i);
    assert(ht.size() == 1000);

    ht.clear();
    assert(ht.size() == 0);
}
   
// insert_unique()、insert_equal()
void test_case4()
{
    cout << "<test_case04>" << endl;
    
    STL::vector<int> v{1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
    hashtable ht1(50), ht2(50);
    
    ht1.insert_unique(v.begin(), v.end());
    ht2.insert_equal(v.begin(), v.end());
    Print(ht1, ht2);
    
    ht1.insert_unique(1);   ht1.insert_unique(999);
    ht2.insert_equal(1);   ht2.insert_equal(999);
    Print(ht1, ht2);
}

// erase()
void test_case5()
{
    cout << "<test_case05>" << endl;
    
    hashtable ht(50);
    for (int i = 0; i < 10; ++i)
        ht.insert_unique(i);
    for (int i = 0; i < 100; ++i)
        ht.insert_equal(999);

    assert(ht.erase(999) == 100);
    Print(ht);

    auto it = ht.begin();
    STL::advance(it, 3);
    ht.erase(it, ht.end());
    Print(ht);

    ht.erase(ht.begin());
    Print(ht);
}

// swap()
void test_case6()
{
    cout << "<test_case06>" << endl;
 
    hashtable ht1(50), ht2(50), ht3(50), ht4(50);
    for (int i = 0; i < 1000; ++i) {
        ht1.insert_equal(i);
        ht2.insert_equal(i);
        ht3.insert_equal(i * 2);
        ht4.insert_equal(i * 2);
    }
    assert(Container_Equal(ht1, ht2));
    assert(Container_Equal(ht3, ht4));
    assert(!Container_Equal(ht1, ht3));

    ht1.swap(ht3);
    assert(Container_Equal(ht1, ht4));
    assert(Container_Equal(ht2, ht3));
}

// find()、count()、equal_range()
void test_case7()
{
    cout << "<test_case07>" << endl;

    hashtable ht(50);
    for (int i = 0; i < 100; ++i)
        ht.insert_unique(i);
    for (int i = 0; i < 100; ++i)
        ht.insert_equal(5);

    assert(*ht.find(99) == 99);
    assert(ht.count(5) == 101);

    auto first = ht.equal_range(5).first;
    auto last = ht.equal_range(5).second;
    for ( ; first != last; ++first)
        assert(*first == 5);
}

// 桶接口
void test_case8()
{
    cout << "<test_case08>" << endl;

    hashtable ht(50);
    ht.insert_unique(59);
    ht.insert_unique(63);
    ht.insert_unique(108);
    ht.insert_unique(2);
    ht.insert_unique(53);
    ht.insert_unique(55);

    cout << "load_factor: " << ht.load_factor() << endl;
    cout << "bucket_count: " << ht.bucket_count() << endl;

    for (int i = 0; i < int(ht.bucket_count()); ++i) {
        int n = int(ht.bucket_size(i));
        if (n)
            printf("bucket[%02d] 有 %02d 个节点\n", i, n);
    }
}

// 比较符
void test_case9()
{
    cout << "<test_case09>" << endl;
 
    hashtable ht1(50), ht2(50), ht3(50);
    for (int i = 0; i < 1000; ++i) {
        ht1.insert_equal(i);
        ht2.insert_equal(i);
        ht3.insert_equal(i * 3);
    }

    assert(ht1 == ht2);
    assert(ht1 != ht3);
}

void test_all_cases()
{
    test_case1();
    test_case2();
    test_case3();
    test_case4();
    test_case5();
    test_case6();
    test_case7();
    test_case8();
    test_case9();
}

int main()
{
    test_all_cases();
    return 0;
}
