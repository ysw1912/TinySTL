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

// insert()、erase()、emplace()
void test_case5()
{
    cout << "<test_case05>" << endl;

    myDeq<int> d1, d3;
    stdDeq<int> d2, d4;
    for (int i = 1; i <= 1000; ++i) {
        d3.push_back(i);
        d4.push_back(i);
    }
    assert(Container_Equal(d3, d4));

    // 4种insert()
    d1.insert(d1.end(), 1);     d1.insert(d1.end(), 1, 2);
    d2.insert(d2.end(), 1);     d2.insert(d2.end(), 1, 2);
    d1.insert(d1.end(), {998, 999, 1000});
    d2.insert(d2.end(), {998, 999, 1000});
    d1.insert(d1.end() - 3, d3.begin() + 2, d3.end() - 3);
    d2.insert(d2.end() - 3, d4.begin() + 2, d4.end() - 3);
    
    assert(Container_Equal(d1, d2));
    assert(Container_Equal(d1, d3));

    // 2种erase
    d1.erase(d1.begin() + d1.size() / 2);
    d2.erase(d2.begin() + d2.size() / 2);
    d1.erase(d1.begin() + d1.size() / 4, d1.end());
    d2.erase(d2.begin() + d2.size() / 4, d2.end());
    assert(Container_Equal(d1, d2));

    d1.emplace(d1.begin() + d1.size() / 2, 123456);
    d2.emplace(d2.begin() + d2.size() / 2, 123456);
    assert(Container_Equal(d1, d2));
}

// push_back()、emplace_back()、pop_back()
// push_front()、emplace_front()、pop_front()
void test_case6()
{
    cout << "<test_case06>" << endl;

    myDeq<Widget> d1;
    stdDeq<Widget> d2;
    init_Widget_deque(d1);
    init_Widget_deque(d2);
    Print(d1, d2);
    assert(Container_Equal(d1, d2));

    d1.push_back({12.3, "aaa"});
    d2.push_back({12.3, "aaa"});
    d1.emplace_back(0, "ysw");
    d2.emplace_back(0, "ysw");

    d1.push_front({45.6, "bbb"});
    d2.push_front({45.6, "bbb"});
    d1.emplace_front(99.9, "wayne");
    d2.emplace_front(99.9, "wayne");

    Print(d1, d2);
    assert(Container_Equal(d1, d2));

    d1.pop_back();
    d2.pop_back();
    d1.pop_front();
    d2.pop_front();

    Print(d1, d2);
    assert(Container_Equal(d1, d2));
}

// 操作符
void test_case7()
{
    cout << "<test_case07>" << endl;

    myDeq<Widget> d1, d2, d3;
    init_Widget_deque(d1); 
    init_Widget_deque(d2); 
    assert(d1 == d2);
    assert(d1 != d3);
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
}

// 性能测试
void test_performance()
{
    cout << "<test_performance>" << endl;

    myDeq<int> d1;
    Profiler::Start();
    for (int i = 0; i < 10000000; ++i) {
        d1.push_front(i);
        d1.push_back(i);
    }
    Profiler::Finish();
    Profiler::dumpDuration();
    
    stdDeq<int> d2;
    Profiler::Start();
    for (int i = 0; i < 10000000; ++i) {
        d2.push_front(i);
        d2.push_back(i);
    }
    Profiler::Finish();
    Profiler::dumpDuration();
    
    Profiler::Start();
    for (int i = 0; i < 10000000; ++i) {
        d1.pop_front();
        d1.pop_back();
    }
    Profiler::Finish();
    Profiler::dumpDuration();
    
    Profiler::Start();
    for (int i = 0; i < 10000000; ++i) {
        d2.pop_front();
        d2.pop_back();
    }
    Profiler::Finish();
    Profiler::dumpDuration();
}
 
int main()
{
    test_all_cases();
    test_performance();
    return 0;
}
