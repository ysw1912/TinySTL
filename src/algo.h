#ifndef TINYSTL_ALGO_H_
#define TINYSTL_ALGO_H_

#include "algobase.h"

namespace STL {

    /**
     *  @brief  返回指向[first, last)中首个大于等于value的元素的迭代器
     *  若找不到则返回last
     *  
     *  第一版本用operator<比较，第二版本用给定的比较函数cmp
     */
    
    // 版本一 forward_iterator
    template <class FIt, class T, class Distance>
    FIt _lower_bound(FIt first, FIt last, const T& value, Distance*, STL::forward_iterator_tag) {
        Distance len = STL::distance(first, last);
        Distance half;
        FIt middle;
        while (len) {
            half = len >> 1;            // 除以2
            middle = first;
            STL::advance(middle, half); // middle指向中间
            if (*middle < value) {
                first = middle;
                ++first;
                len = len - half - 1;
            } else 
                len = half;
        }
        return first;
    }

    // 版本一 random_access_iterator
    template <class RIt, class T, class Distance>
    RIt _lower_bound(RIt first, RIt last, const T& value, Distance*, STL::random_access_iterator_tag) {
        Distance len = last - first;
        Distance half;
        RIt middle;
        while (len) {
            half = len >> 1;            // 除以2
            middle = first + half;      // middle指向中间
            if (*middle < value) {
                first = middle + 1;
                len = len - half - 1;
            } else 
                len = half;
        }
        return first;
    }

    // 版本一
    template <class FIt, class T>
    inline FIt lower_bound(FIt first, FIt last, const T& value)
    { return _lower_bound(first, last, value, STL::distance_type(first), STL::iterator_category(first)); }
    
    // 版本二 forward_iterator
    template <class FIt, class T, class Compare, class Distance>
    FIt _lower_bound(FIt first, FIt last, const T& value, Compare cmp, Distance*, STL::forward_iterator_tag) {
        Distance len = STL::distance(first, last);
        Distance half;
        FIt middle;
        while (len) {
            half = len >> 1;            // 除以2
            middle = first;
            STL::advance(middle, half); // middle指向中间
            if (cmp(*middle, value)) {
                first = middle;
                ++first;
                len = len - half - 1;
            } else 
                len = half;
        }
        return first;
    }

    // 版本二 random_access_iterator
    template <class RIt, class T, class Compare, class Distance>
    RIt _lower_bound(RIt first, RIt last, const T& value, Compare cmp, Distance*, STL::random_access_iterator_tag) {
        Distance len = last - first;
        Distance half;
        RIt middle;
        while (len) {
            half = len >> 1;            // 除以2
            middle = first + half;      // middle指向中间
            if (cmp(*middle, value)) {
                first = middle + 1;
                len = len - half - 1;
            } else 
                len = half;
        }
        return first;
    }

    // 版本二
    template <class FIt, class T, class Compare>
    inline FIt lower_bound(FIt first, FIt last, const T& value, Compare cmp)
    { return _lower_bound(first, last, value, cmp, STL::distance_type(first), STL::iterator_category(first)); }

} // namespace end 

#endif
