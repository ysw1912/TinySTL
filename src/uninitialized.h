#ifndef TINYSTL_UNINITIALIZED_H_
#define TINYSTL_UNINITIALIZED_H_ 

#include "algobase.h"
#include "construct.h"
#include "type_traits.h"

namespace STL {
    
    /***************** uninitalized_copy ******************/ 
    template <class InputIterator, class ForwardIterator>
    inline ForwardIterator 
    uninitalized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
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

    

    /***************** uninitalized_fill ******************/
    
    /**************** uninitalized_fill_n *****************/

} /* namespace STL */ 
#endif
