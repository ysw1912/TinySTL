#ifndef TINYSTL_CONSTRUCT_H_
#define TINYSTL_CONSTRUCT_H_

#include <new>
#include <utility>  // for std::forward
#include "type_traits.h"

namespace STL {

    // construct 
#if __cplusplus >= 201103L
    template <class T1, class... Args>
    void construct(T1 *p, Args&&... args) {
        new(static_cast<void *>(p)) T1(std::forward<Args>(args)...);
    }
#else 
    template <class T1, class T2>
    void construct(T1 *p, const T2 &value) {
        new(static_cast<void *>(p)) T1(value);
    }
#endif 

    // destory版本一
    template <class T>
    void destroy(T *pointer) {
        pointer->~T();
    }
    // destory版本二
    template <class ForwardIterator>
    void destroy(ForwardIterator first, ForwardIterator last) {
        typedef typename __type_traits<ForwardIterator>::has_trivial_destructor trivial_destructor; 
        _destory(first, last, trivial_destructor());
    }

    template <class ForwardIterator>
    inline void _destory(ForwardIterator, ForwardIterator, __true_type) {}

    template <class ForwardIterator>
    inline void _destory(ForwardIterator first, ForwardIterator last, __false_type) {
        for ( ; first < last; ++first) {
            destroy(&*first);
        }
    }
} /* namespace STL */

#endif
