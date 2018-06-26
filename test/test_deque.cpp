/*************************************************************************
    > File Name: test_vector.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-05-22
*************************************************************************/
#include <deque>

#include "../STL/deque.h"
#include "profiler.h"
#include "test_util.h"

template <class T>
using stdDeq = std::deque<T>;

template <class T>
using myDeq = STL::deque<T>;

template <class Container>
void init_Widget_deque(Container& d)
{
    d.clear();
    for (int i = 1; i <= 4; ++i) {
        d.emplace_back(i * 0.5, string(i, i + '0'));
    }
}

// 构造/拷贝构造/移动构造函数、拷贝/移动赋值操作符
void test_case1()
{
    cout << "<test_case01>" << endl;
    
    myDeq<Widget> d0;
    myDeq<Widget> d1(10);
    myDeq<Widget> d2(10, {123, "ysw"});
    Print(d0, d1, d2);
    
    myDeq<string> d3{"aaa", "bbb", "ccc", "ddd"};
    myDeq<string> d4(d3.begin(), d3.end());
    Print(d3, d4);
    assert(Container_Equal(d3, d4));

    myDeq<string> d5(d3);
    myDeq<string> d6(d4);
    assert(Container_Equal(d5, d6));

    myDeq<string> d7(std::move(d3));
    myDeq<string> d8(std::move(d4));
    Print(d3, d4);
    assert(Container_Equal(d7, d8));
    
    d3 = d7;              
    d4 = std::move(d8);   
    Print(d3, d4);
    assert(Container_Equal(d3, d4));
}

// 元素访问
void test_case2()
{
    cout << "<test_case02>" << endl;

    myDeq<Widget> d1;
    stdDeq<Widget> d2;
    init_Widget_deque(d1); 
    init_Widget_deque(d2); 
    Print(d1, d2);
    assert(Container_Equal(d1, d2));  
    
    d1.front() = d2.front() = {12.3, "aaa"};
    d1[1] = d2[1] = {45.6, "bbb"};
    d1.at(2) = d2.at(2) = {78.9, "ccc"};
    d1.back() = d2.back() = {0, "ysw"};
    Print(d1, d2);
    assert(Container_Equal(d1, d2));  
}

// 迭代器
void test_case3()
{
    cout << "<test_case03>" << endl;

    myDeq<int> d;
    int i;
    for (i = 0; i < 1000; ++i)
        d.push_back(i);
    
    i = 0;
    for (myDeq<int>::iterator it = d.begin(); it != d.end(); ++it, ++i) {
        assert(*it == i);
        *it = i * 2;
    }
    
    --i;
    for (myDeq<int>::const_iterator cit = d.cend() - 1; ; --cit, --i) {
        assert(*cit == i * 2);
        if (cit == d.cbegin())  break;
    }
}

// 容量
void test_case4() 
{
    cout << "<test_case04>" << endl;

    myDeq<int> d(100);
    assert(d.size() == 100);
    
    d.push_back(0);
    assert(d.size() == 101);

    d.resize(5);
    assert(d.size() == 5);

    d.clear();
    assert(d.empty());
    assert(d.size() == 0);

    d.resize(10, 6);
    assert(d.size() == 10);
    Print(d);
}
/*
// push_back()、emplace_back()、emplace()、pop_back()
void test_case5()
{
    cout << "<test_case05>" << endl;

    myVec<Widget> v1;
    stdVec<Widget> v2;
    init_Widget_vector(v1);
    init_Widget_vector(v2);
    Print(v1, v2);
    assert(Container_Equal(v1, v2));

    v1.push_back({12.3, "aaa"});    v1.emplace_back(45.6, "bbb"); 
    v2.push_back({12.3, "aaa"});    v2.emplace_back(45.6, "bbb");
    v1.emplace(v1.begin(), 0, "ysw");
    v2.emplace(v2.begin(), 0, "ysw");
    v1.pop_back();
    v2.pop_back();
    Print(v1, v2);
    assert(Container_Equal(v1, v2));
}

// insert()、erase()
void test_case6()
{
    cout << "<test_case06>" << endl;

    myVec<int> v1, v3;
    stdVec<int> v2, v4;
    for (int i = 1; i <= 1000; ++i) {
        v3.push_back(i);
        v4.push_back(i);
    }
    assert(Container_Equal(v3, v4));

    // 4种insert()
    v1.insert(v1.end(), 1);     v1.insert(v1.end(), 1, 2);
    v2.insert(v2.end(), 1);     v2.insert(v2.end(), 1, 2);
    v1.insert(v1.end(), {998, 999, 1000});
    v2.insert(v2.end(), {998, 999, 1000});
    v1.insert(v1.end() - 3, v3.begin() + 2, v3.end() - 3);
    v2.insert(v2.end() - 3, v4.begin() + 2, v4.end() - 3);
    
    assert(Container_Equal(v1, v2));
    assert(Container_Equal(v1, v3));

    // 2种erase
    v1.erase(v1.begin() + v1.size() / 2);
    v2.erase(v2.begin() + v2.size() / 2);
    v1.erase(v1.begin() + v1.size() / 4, v1.end());
    v2.erase(v2.begin() + v2.size() / 4, v2.end());
    assert(Container_Equal(v1, v2));
}

// 友元函数
void test_case7()
{
    cout << "<test_case07>" << endl;

    myVec<Widget> v1, v2;
    init_Widget_vector(v1); 
    init_Widget_vector(v2); 
    Print(v1, v2);
    assert(v1 == v2);
    assert(!(v1 != v2));
}
*/ 
void test_all_cases()
{
    test_case1();
    test_case2();
    test_case3();
    test_case4();
//    test_case5();
//    test_case6();
//    test_case7();
}
/*
// 性能测试
void test_performance()
{
    cout << "<test_performance>" << endl;

    myVec<int> v1;
    Profiler::Start();
    for (int i = 0; i < 10000000; ++i)
        v1.push_back(i);
    Profiler::Finish();
    Profiler::dumpDuration();
    
    stdVec<int> v2;
    Profiler::Start();
    for (int i = 0; i < 10000000; ++i)
        v2.push_back(i);
    Profiler::Finish();
    Profiler::dumpDuration();
}
*/ 
int main()
{
    test_all_cases();
   //  test_performance();
    return 0;
}
