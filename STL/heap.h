#ifndef TINYSTL_HEAP_H_
#define TINYSTL_HEAP_H_ 

#include <utility>  // for std::move

#include "iterator.h"

namespace STL 
{
    template <class RandomAccessIterator, class Distance, class T, class Compare>
    void _push_heap(RandomAccessIterator first,
                    Distance holeIndex, Distance topIndex, T value,
                    Compare cmp)
    {
        // 找到holeIndex的父节点下标
        Distance parent = (holeIndex - 1) / 2;
        // 当尚未到达顶端，且父节点小于新增值value
        while (holeIndex > topIndex && cmp(*(first + parent), value)) {
            // 令洞值为父值
            *(first + holeIndex) = std::move(*(first + parent));
            // 更新洞节点和父节点
            holeIndex = parent;
            parent = (holeIndex - 1) / 2;
        }
        // 持续至满足max-heap特性
        *(first + holeIndex) = value;
    }

    /**
     *  @brief  将元素push进max-heap 
     *  @param  heap的底部容器vector的头尾迭代器
     *  
     *  函数调用时，新元素已插入到vector的尾端，即last - 1
     *  将其推入范围[first, last - 1)的合法max-heap中
     *  该操作完成后，[first, last)成为合法堆
     */ 
    template <class RandomAccessIterator, class Compare>
    inline void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare cmp)
    {
        using distance_type = typename STL::iterator_traits<RandomAccessIterator>::difference_type;
        using value_type    = typename STL::iterator_traits<RandomAccessIterator>::value_type;
        value_type value = *(last - 1);
        _push_heap(first, distance_type((last - first) - 1), distance_type(0), std::move(value), cmp);
    }

    template <class RandomAccessIterator, class Distance, class T, class Compare>
    void _adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len,
                      T value, Compare cmp)
    {
        const Distance topIndex = holeIndex;
        // 洞节点的右孩子
        Distance secondChild = 2 * (holeIndex + 1);
        while (secondChild < len) {
            // 让secondChild为洞节点的两个孩子较大者
            if (cmp(*(first + secondChild), *(first + (secondChild - 1))))
                --secondChild;
            // 令较大值为洞节点值
            *(first + holeIndex) = std::move(*(first + secondChild));
            // 洞下标下移
            holeIndex = secondChild;
            secondChild = 2 * (holeIndex + 1);
        }
        // len是偶数
        // 洞节点没有右孩子，只有左孩子
        if (secondChild == len) {
            // 令左孩子值为洞节点值
            *(first + holeIndex) = std::move(*(first + (secondChild - 1)));
            // 洞下标下移
            holeIndex = secondChild - 1;
        }
        // 不可直接*(first + holeIndex) = std::move(value) !!! 比如重新入堆
        _push_heap(first, holeIndex, topIndex, std::move(value), cmp);
    }

    template <class RandomAccessIterator, class Compare>
    inline void _pop_heap(RandomAccessIterator first, RandomAccessIterator last,
                          RandomAccessIterator result, Compare cmp)
    {
        using distance_type = typename STL::iterator_traits<RandomAccessIterator>::difference_type;
        using value_type    = typename STL::iterator_traits<RandomAccessIterator>::value_type;
        value_type value = std::move(*result);
        // vector尾值为首值，可用back()取出尾值（最大值）或pop_back()移除
        *result = std::move(*first);
        _adjust_heap(first, distance_type(0), distance_type(last - first), std::move(value), cmp);
    }

    /**
     *  @brief  将堆顶元素（根节点）从堆中弹出
     *  
     *  弹出的节点其实存于last - 1，可用vector::back()或pop_back()操作
     *  [first, last - 1)成为合法max-heap 
     */
    template <class RandomAccessIterator, class Compare>
    inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare cmp)
    {
        if (last - first > 1) {
            --last;
            _pop_heap(first, last, last, cmp);
        }
    }

    /**
     *  @brief  排序一个范围[first, last)的合法max-heap
     */
    template <class RandomAccessIterator, class Compare>
    void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare cmp)
    {
        // 将pop_heap()的if变为while
        // 每次loop极大值被放在尾端，并扣除尾端
        while (last - first > 1) {
            --last;
            _pop_heap(first, last, last, cmp);
        }
    }

    /**
     *  @brief  用来自范围[first, last)的元素构造一个max-heap 
     */ 
    template <class RandomAccessIterator, class Compare>
    inline void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare cmp)
    {
        using distance_type = typename STL::iterator_traits<RandomAccessIterator>::difference_type;
        using value_type    = typename STL::iterator_traits<RandomAccessIterator>::value_type;
        if (last - first < 2)   // 长度为0或1
           return; 
        const distance_type len = last - first;
        // 找出第一个需要重排的子树头部
        distance_type parent = (len - 2) / 2;
        while (true) {
            // 重排以parent为首的子树
            value_type value = std::move(*(first + parent));
            _adjust_heap(first, parent, len, std::move(value), cmp);
            // 走到根节点，结束
            if (parent == 0)
                return;
            --parent;
        }
    }
}

#endif 
