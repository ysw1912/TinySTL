#ifndef TINYSTL_ALGOBASE_H_
#define TINYSTL_ALGOBASE_H_ 

#include <cstring>
#include "iterator.h"
#include "type_traits.h"

namespace STL {
    
    // fill_n
    // 将first开始的n个元素填入新值x，返回的迭代器指向被填入的最后一个元素的下一个位置
    template <class OutputIterator, class Size, class T>
    inline OutputIterator fill_n(OutputIterator first, Size n, const T &value) {
        for ( ; n > 0; --n, ++first) {
            *first = value;
        }
        return first;
    }

    // copy 极其复杂

    /********** __copy_d() **********/

    template <class RandomAccessIterator, class OutputIterator, class Distance>
    inline OutputIterator __copy_d(RandomAccessIterator first, RandomAccessIterator last, 
            OutputIterator result, Distance*) {
        for (Distance n = last - first; n > 0; --n, ++result, ++first) {
            *result = *first;
        }
        return result;
    }

    /********** __copy_t() **********/

    // 指针所指对象具备默认赋值操作符
    template <class T>
    inline T* __copy_t(T* first, T* last, T* result, STL::true_type) {
        memmove(result, first, sizeof(T) * (last - first));
        return result + (last - first);
    }

    // 指针所指对象不具备默认赋值操作符
    template <class T>
    inline T* __copy_t(T* first, T* last, T* result, STL::false_type) {
        // 原生指针是RandomAccessIterator，交给__copy_d()
        return __copy_d(first, last, result, reinterpret_cast<ptrdiff_t *>(0));
    }

    template <class T>
    inline T* __copy_t(const T* first, const T* last, T* result, STL::true_type) {
        memmove(result, first, sizeof(T) * (last - first));
        return result + (last - first);
    }

    template <class T>
    inline T* __copy_t(const T* first, const T* last, T* result, STL::false_type) {
        // 原生指针是RandomAccessIterator，交给__copy_d()
        return __copy_d(first, last, result, reinterpret_cast<ptrdiff_t *>(0));
    }

    /********** __copy() **********/
    
    // InputIterator 
    template <class InputIterator, class OutputIterator>
    inline OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, STL::input_iterator_tag) {
        for ( ; first != last; ++result, ++first) {
            *result = *first;
        }
        return result;
    }

    // RandomAccessIterator
    template <class RandomAccessIterator, class OutputIterator>
    inline OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, STL::random_access_iterator_tag) {
        return __copy_d(first, last, result, STL::distance_type(first));
    }


    /********** _copy() **********/
    
    // 完全泛化，根据first的迭代器类型不同，使用不同的__copy()
    template <class InputIterator, class OutputIterator>
    struct _copy {
        OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
            return __copy(first, last, result, STL::iterator_category(first));
        }
    };
    
    // 偏特化T*, T*
    template <class T>
    struct _copy<T*, T*> {
        T* operator()(T* first, T* last, T* result) {
            // 指针所指是否具有默认的赋值操作符
            typedef typename STL::has_trivial_copy_assign<T> t;
            return __copy_t(first, last, result, t());
        }
    };
    
    // 偏特化const T*, T*
    template <class T>
    struct _copy<const T*, T*> {
        T* operator()(const T* first, const T* last, T* result) {
            typedef typename STL::has_trivial_copy_assign<T> t;
            return __copy_t(first, last, result, t());
        }
    };

    /********** copy() **********/

    // 完全泛化
    template <class InputIterator, class OutputIterator>
    inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
        return _copy<InputIterator, OutputIterator>()(first, last, result);
    }
    
    // copy_backward
    
    /********** __copy_backward() **********/
    
    // first是BidirectionalIterator
    template <class BidirectionalIterator1, class BidirectionalIterator2>
    inline BidirectionalIterator2 __copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result, STL::bidirectional_iterator_tag) {
        while (last != first) {
            *(--result) = *(--last);
        }
        return result;
    }
    
    /********** __copy_backward_t() **********/

    // 指针所指对象具备默认赋值操作符
    template <class T>
    inline T* __copy_backward_t(T* first, T* last, T* result, STL::true_type) {
        memmove(result - (last - first), first, sizeof(T) * (last - first));
        return result - (last - first);
    }

    // 指针所指对象不具备默认赋值操作符
    template <class T>
    inline T* __copy_backward_t(T* first, T* last, T* result, STL::false_type) {
        // 原生指针是RandomAccessIterator，交给__copy_backward()
        return __copy_backward(first, last, result, reinterpret_cast<ptrdiff_t *>(0));
    }

    template <class T>
    inline T* __copy_backward_t(const T* first, const T* last, T* result, STL::true_type) {
        memmove(result - (last - first), first, sizeof(T) * (last - first));
        return result - (last - first);
    }

    template <class T>
    inline T* __copy_backward_t(const T* first, const T* last, T* result, STL::false_type) {
        // 原生指针是RandomAccessIterator，交给__copy_backward()
        return __copy_backward(first, last, result, reinterpret_cast<ptrdiff_t *>(0));
    }

    /********** _copy_backward() **********/
    
    // 完全泛化，first的迭代器类型必须是BidirectionalIterator
    template <class BidirectionalIterator1, class BidirectionalIterator2>
    struct _copy_backward {
        BidirectionalIterator2 operator()(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result) {
            return __copy_backward(first, last, result, STL::iterator_category(first));
        }
    };
    
    // 偏特化T*, T*
    template <class T>
    struct _copy_backward<T*, T*> {
        T* operator()(T* first, T* last, T* result) {
            // 指针所指是否具有默认的赋值操作符
            typedef typename STL::has_trivial_copy_assign<T> t;
            return __copy_backward_t(first, last, result, t());
        }
    };
    
    // 偏特化const T*, T*
    template <class T>
    struct _copy_backward<const T*, T*> {
        T* operator()(const T* first, const T* last, T* result) {
            typedef typename STL::has_trivial_copy_assign<T> t;
            return __copy_backward_t(first, last, result, t());
        }
    };
    /********** copy_backward() **********/
    
    // 完全泛化 
    template <class BidirectionalIterator1, class BidirectionalIterator2>
    inline BidirectionalIterator2 copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result) {
        return _copy_backward<BidirectionalIterator1, BidirectionalIterator2>()(first, last, result);    
    }

} /* namespace STL */ 

#endif
