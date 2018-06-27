/*************************************************************************
    > File Name: test_vector.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-06-27
*************************************************************************/
#include <vector>
#include <algorithm>

#include "../STL/vector.h"
#include "../STL/queue.h"
#include "profiler.h"
#include "test_util.h"

template <class T>
using stdVec = std::vector<T>;

template <class T>
using myVec = STL::vector<T>;

// make_heap()、push_heap()、pop_heap()、sort_heap()
void test_case1()
{
    cout << "<test_case01>" << endl;

    myVec<int> v1{0, 1, 2, 3, 4, 8, 9, 3, 5};
    stdVec<int> v2{0, 1, 2, 3, 4, 8, 9, 3, 5};
    assert(Container_Equal(v1, v2));
    std::less<int> lt;

    STL::make_heap(v1.begin(), v1.end(), lt);
    std::make_heap(v2.begin(), v2.end(), lt);
    assert(Container_Equal(v1, v2));

    v1.push_back(7);
    v2.push_back(7);
    STL::push_heap(v1.begin(), v1.end(), lt);
    std::push_heap(v2.begin(), v2.end(), lt);
    assert(Container_Equal(v1, v2));
    
    STL::pop_heap(v1.begin(), v1.end(), lt);
    std::pop_heap(v2.begin(), v2.end(), lt);
    v1.pop_back();
    v2.pop_back();
    assert(Container_Equal(v1, v2));

    STL::sort_heap(v1.begin(), v1.end(), lt);
    std::sort_heap(v2.begin(), v2.end(), lt);
    assert(Container_Equal(v1, v2));
    
    STL::make_heap(v1.begin(), v1.end(), lt);
    std::make_heap(v2.begin(), v2.end(), lt);
    assert(Container_Equal(v1, v2));
}

// priority_queue
void test_case2()
{
    cout << "<test_case02>" << endl;
    
    myVec<int> v{0, 1, 2, 3, 4, 8, 9, 3, 5};
    STL::priority_queue<int> pq(v.begin(), v.end());
    assert(pq.size() == 9);

    while (!pq.empty()) {
        cout << pq.top() << ' ';
        pq.pop();
    }
    cout << endl;
}

void test_all_cases()
{
    test_case1();
    test_case2();
}

// 性能测试
void test_performance()
{
    cout << "<test_performance>" << endl;
    const int max = 1000000;

    myVec<int> v1;
    stdVec<int> v2;
    for (int i = 0; i != max; ++i) {
        int n = get_rand(-max, max);
        v1.push_back(n);
        v2.push_back(n);
    }
    assert(Container_Equal(v1, v2));
    
    std::less<int> lt;
    
    Profiler::Start();
    STL::make_heap(v1.begin(), v1.end(), lt);
    Profiler::Finish();
    cout << " my make_heap()  "; Profiler::dumpDuration();
    
    Profiler::Start();
    std::make_heap(v2.begin(), v2.end(), lt);
    Profiler::Finish();
    cout << "std make_heap()  "; Profiler::dumpDuration();
    
    assert(Container_Equal(v1, v2));
    
    Profiler::Start();
    STL::sort_heap(v1.begin(), v1.end(), lt);
    Profiler::Finish();
    cout << " my sort_heap()  "; Profiler::dumpDuration();
    
    Profiler::Start();
    std::sort_heap(v2.begin(), v2.end(), lt);
    Profiler::Finish();
    cout << "std sort_heap()  "; Profiler::dumpDuration();
    
    assert(Container_Equal(v1, v2));
}

int main()
{
    test_all_cases();
    test_performance();
    return 0;
}
