#ifndef TINYSTL_UNINITIALIZED_H_
#define TINYSTL_UNINITIALIZED_H_ 

#include "algobase.h"
#include "construct.h"
#include "type_traits.h"

namespace STL {
    
    /***************** uninitialized_copy ******************/ 

    // 函数声明
    template <class InputIterator, class ForwardIterator, class T>
    inline ForwardIterator 
    _uinit_copy(InputIterator first, InputIterator last, ForwardIterator result, T*);

        template <class InputIterator, class ForwardIterator>
    ForwardIterator 
    __uninit_copy(InputIterator first, InputIterator last, ForwardIterator result, STL::__true_type);
    
    template <class InputIterator, class ForwardIterator>
    ForwardIterator 
    __uninit_copy(InputIterator first, InputIterator last, ForwardIterator result, STL::__false_type);

    // uninitialized_copy()
    template <class InputIterator, class ForwardIterator>
    inline ForwardIterator 
    uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
        return _uinit_copy(first, last, result, STL::value_type(result));
    }

    // 判断result的value_type是否为POD
    template <class InputIterator, class ForwardIterator, class T>
    inline ForwardIterator 
    _uinit_copy(InputIterator first, InputIterator last, ForwardIterator result, T*) {
        typedef typename STL::__type_traits<T>::is_POD_type is_POD;
        return __uninit_copy(first, last, result, is_POD());
    }

    // 是POD
    template <class InputIterator, class ForwardIterator>
    ForwardIterator 
    __uninit_copy(InputIterator first, InputIterator last, ForwardIterator result, STL::__true_type) {
        return STL::copy(first, last, result);
    }
    
    // 不是POD
    template <class InputIterator, class ForwardIterator>
    ForwardIterator 
    __uninit_copy(InputIterator first, InputIterator last, ForwardIterator result, STL::__false_type) {
        ForwardIterator cur = result;
        // 一个一个地构造元素
        for ( ; first != last; ++first, ++cur) {
            STL::construct(&*cur, *first);
        }
        return cur;
    }

    

    /***************** uninitialized_fill ******************/
    
    /**************** uninitialized_fill_n *****************/

    // 函数声明
    template <class ForwardIterator, class Size, class T, class T1>
    inline ForwardIterator 
    _uninit_fill_n(ForwardIterator first, Size n, const T &x, T1*);

    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator 
    __uninit_fill_n(ForwardIterator first, Size n, const T &x, STL::__true_type);

    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator 
    __uninit_fill_n(ForwardIterator first, Size n, const T &x, STL::__false_type);

    // uninitialized_fill_n()
    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator 
    uninitialized_fill_n(ForwardIterator first, Size n, const T &x) {
        return _uninit_fill_n(first, n, x, STL::value_type(first));
    }

    // 判断first的value_type是否为POD
    template <class ForwardIterator, class Size, class T, class T1>
    inline ForwardIterator
    _uninit_fill_n(ForwardIterator first, Size n, const T &x, T1*) {
        typedef typename STL::__type_traits<T1>::is_POD_type is_POD;
        return __uninit_fill_n(first, n, x, is_POD());
    }
    
    // 是POD
    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator 
    __uninit_fill_n(ForwardIterator first, Size n, const T &x, STL::__true_type) {
        return STL::fill_n(first, n, x);
    }
    
    // 不是POD
    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator 
    __uninit_fill_n(ForwardIterator first, Size n, const T &x, STL::__false_type) {
        ForwardIterator cur = first;
        for ( ; n > 0; --n, ++cur) {
            STL::construct(&*cur, x);
        }
        return cur;
    }

} /* namespace STL */ 
#endif 
