/*************************************************************************
    > File Name: testTree.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-06-05
*************************************************************************/
#include "/usr/include/c++/5.4.0/bits/stl_tree.h"

#include "../STL/tree.h"
#include "../STL/vector.h"
#include "test_util.h"

using stdRbtree = std::_Rb_tree<int, int, std::_Identity<int>, std::less<int>>;
using myRbtree = STL::rb_tree<int, int, std::_Identity<int>, std::less<int>>;

Profiler::TimePoint Profiler::start;
Profiler::TimePoint Profiler::finish;
Profiler::SecDuration Profiler::duration;

// 判断红黑树相等，包括颜色比较
// 第一个参数为std::_Rb_tree，第二个参数为自己实现的STL::rb_tree 
bool Rbtree_Equal(const stdRbtree& rbt1, const myRbtree& rbt2) {
    auto first1 = rbt1.begin(), last1 = rbt1.end();
    auto first2 = rbt2.begin(), last2 = rbt2.end();
    for ( ; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (*first1 != *first2 || first1._M_node->_M_color ^ first2.node->color)
            return false;
    }
    return first1 == last1 && first2 == last2;
}

// 构造/拷贝/移动构造函数、拷贝赋值操作符
void test_case1()
{
    cout << "<test_case01>" << endl;
    
    stdRbtree rbt1;
    myRbtree rbt2;
    for (int i = 0; i < 1000; ++i) {
        int n = get_rand(1, 999);
        rbt1._M_insert_unique(n);
        rbt2.insert_unique(n);
    }
    assert(Rbtree_Equal(rbt1, rbt2));

    myRbtree rbt3(rbt2);
    assert(Rbtree_Equal(rbt1, rbt3));

    myRbtree rbt4(std::move(rbt3));
    assert(rbt3.size() == 0);
    assert(Rbtree_Equal(rbt1, rbt4));
    

    myRbtree rbt5;
    rbt5 = rbt4;
    assert(Rbtree_Equal(rbt1, rbt5));
}

// 迭代器
void test_case2()
{
    cout << "<test_case02>" << endl;
    
    myRbtree rbt;
    int i;
    for (i = 0; i < 1000; ++i)
        rbt.insert_unique(i);

    --i;
    for (myRbtree::const_iterator it = --rbt.end(); ; --it, --i) {
        assert(*it == i);
        if (it == rbt.begin())  break;
    }
}

// 容量
void test_case3()
{
    cout << "<test_case03>" << endl;
    
    myRbtree rbt;
    for (int i = 0; i < 1000; ++i)
        rbt.insert_unique(i);
    assert(rbt.size() == 1000);

    rbt.clear();
    assert(rbt.size() == 0);
}
   
// insert_unique()、insert_equal()
void test_case4()
{
    cout << "<test_case04>" << endl;
    
    STL::vector<int> v{1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
    stdRbtree rbt1, rbt2;
    myRbtree rbt3, rbt4;
    
    rbt1._M_insert_unique(v.begin(), v.end());
    rbt3.insert_unique(v.begin(), v.end());
    assert(Rbtree_Equal(rbt1, rbt3));
    
    rbt2._M_insert_equal(v.begin(), v.end());
    rbt4.insert_equal(v.begin(), v.end());
    assert(Rbtree_Equal(rbt2, rbt4));
   
    rbt1._M_insert_unique(999);
    rbt3.insert_unique(999);
    assert(Rbtree_Equal(rbt1, rbt3));

    rbt2._M_insert_equal(1);
    rbt4.insert_equal(1);
    assert(Rbtree_Equal(rbt2, rbt4));
}

// erase()
void test_case5()
{
    cout << "<test_case05>" << endl;
    
    stdRbtree rbt1;
    myRbtree rbt2;
    for (int i = 0; i < 1000; ++i) {
        int n = get_rand(1, 999);
        rbt1._M_insert_unique(n);
        rbt2.insert_unique(n);
    }
    assert(Rbtree_Equal(rbt1, rbt2));

    auto it1 = rbt1.begin();
    auto it2 = rbt2.begin();
    int offset = get_rand(0, rbt1.size() - 1);
    std::advance(it1, offset);
    STL::advance(it2, offset);

    it1 = rbt1.erase(it1);
    it2 = rbt2.erase(it2);
    assert(Rbtree_Equal(rbt1, rbt2));

    rbt1.erase(it1, rbt1.end());
    rbt2.erase(it2, rbt2.end());
    assert(Rbtree_Equal(rbt1, rbt2));

}

// swap()
void test_case6()
{
    cout << "<test_case06>" << endl;
 
    myRbtree rbt1, rbt2, rbt3, rbt4;
    for (int i = 0; i < 1000; ++i) {
        int n = get_rand(1, 999);
        rbt1.insert_equal(n);
        rbt2.insert_equal(n);
        rbt3.insert_equal(n * 2);
        rbt4.insert_equal(n * 2);
    }
    assert(Container_Equal(rbt1, rbt2));
    assert(Container_Equal(rbt3, rbt4));

    rbt1.swap(rbt3);
    assert(Container_Equal(rbt1, rbt4));
    assert(Container_Equal(rbt2, rbt3));
}

// find()、count()、equal_range()
void test_case7()
{
    cout << "<test_case07>" << endl;

    myRbtree rbt;
    for (int i = 0; i < 100; ++i)
        rbt.insert_unique(i);
    for (int i = 0; i < 100; ++i)
        rbt.insert_equal(5);

    assert(*rbt.find(99) == 99);
    assert(rbt.count(5) == 101);

    auto first = rbt.equal_range(5).first;
    auto last = rbt.equal_range(5).second;
    for ( ; first != last; ++first)
        assert(*first == 5);
}

// lower_bound()、upper_bound()
void test_case8()
{
    cout << "<test_case08>" << endl;
    
    STL::vector<int> v{1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
    myRbtree rbt;
    rbt.insert_equal(v.begin(), v.end());

    assert(*rbt.lower_bound(3) == 3);
    assert(*rbt.upper_bound(3) == 4);
}

// 比较符
void test_case9()
{
    cout << "<test_case09>" << endl;
 
    myRbtree rbt1, rbt2, rbt3;
    for (int i = 0; i < 1000; ++i) {
        rbt1.insert_equal(i);
        rbt2.insert_equal(i);
        rbt3.insert_equal(i * 3);
    }

    assert(rbt1 == rbt2);
    assert(rbt1 != rbt3);
}

// rb_verify()
void test_case10()
{
    cout << "<test_case10>" << endl;
    
    myRbtree rbt;
    for (int i = 0; i < 1000; ++i)
        rbt.insert_unique(get_rand(1, 999));

    assert(rbt.rb_verify());
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
    test_case10();
}

// 性能测试
void test_performance()
{
    cout << "<test_performance>" << endl;
    
    int max = 1000000;
    STL::vector<int> v;
    for (int i = 0; i < max; ++i)
        v.push_back(get_rand(-max, max));

    stdRbtree rbt1;
    Profiler::Start();
    for (int i = 0; i < max; ++i)
        rbt1._M_insert_unique(v[i]);
    Profiler::Finish();
    Profiler::dumpDuration();
    
    myRbtree rbt2;
    Profiler::Start();
    for (int i = 0; i < max; ++i)
        rbt2.insert_unique(v[i]);
    Profiler::Finish();
    Profiler::dumpDuration();
}

int main()
{
    test_all_cases();
    test_performance();
    return 0;
}
