#ifndef TINYSTL_ALGOBASE_H_
#define TINYSTL_ALGOBASE_H_ 

#include <cstring>

#include "iterator.h"
#include "type_traits.h"

namespace STL
{    
    /**
     *  @brief  将[first, last)内所有元素填入新值x
     */
    template <class ForwardIterator, class T>
    inline void fill(ForwardIterator first, ForwardIterator last, const T &x)
    {
        for ( ; first != last; ++first) {
            *first = x;
        }
    }        

    /**
     *  @brief  将first开始的n个元素填入新值x
     *  @return  迭代器，指向被填入的最后一个元素的下一个位置
     */ 
    template <class OutputIterator, class Size, class T>
    inline OutputIterator fill_n(OutputIterator first, Size n, const T &value)
    {
        for ( ; n > 0; --n, ++first) {
            *first = value;
        }
        return first;
    }

    /**
     *  @brief  交换两个对象a b的内容
     */ 
    template <class T>
    inline void swap(T& a, T& b)
    {
        T tmp = a;
        a = b;
        b = tmp;
    }

    // 以下是整组copy()函数

    /********** __copy_d() **********/

    template <class RandomAccessIterator, class OutputIterator, class Distance>
    inline OutputIterator __copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*)
    {
        for (Distance n = last - first; n > 0; --n, ++result, ++first) {
            *result = *first;
        }
        return result;
    }

    /********** __copy_t() **********/

    // 指针所指对象具备默认赋值操作符
    template <class T>
    inline T* __copy_t(T* first, T* last, T* result, STL::true_type)
    {
        memmove(result, first, sizeof(T) * (last - first));
        return result + (last - first);
    }

    // 指针所指对象不具备默认赋值操作符
    template <class T>
    inline T* __copy_t(T* first, T* last, T* result, STL::false_type)
    { return __copy_d(first, last, result, reinterpret_cast<ptrdiff_t *>(0)); } // 原生指针是RandomAccessIterator，交给__copy_d()

    template <class T>
    inline T* __copy_t(const T* first, const T* last, T* result, STL::true_type)
    {
        memmove(result, first, sizeof(T) * (last - first));
        return result + (last - first);
    }

    template <class T>
    inline T* __copy_t(const T* first, const T* last, T* result, STL::false_type)
    { return __copy_d(first, last, result, reinterpret_cast<ptrdiff_t *>(0)); } // 原生指针是RandomAccessIterator，交给__copy_d()

    /********** __copy() **********/
    
    // InputIterator 
    template <class InputIterator, class OutputIterator>
    inline OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, STL::input_iterator_tag)
    {
        for ( ; first != last; ++result, ++first) {
            *result = *first;
        }
        return result;
    }

    // RandomAccessIterator
    template <class RandomAccessIterator, class OutputIterator>
    inline OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, STL::random_access_iterator_tag)
    { return __copy_d(first, last, result, STL::distance_type(first)); }


    /********** _copy() **********/
    
    // 完全泛化，根据first的迭代器类型不同，使用不同的__copy()
    template <class InputIterator, class OutputIterator>
    struct _copy
    {
        OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result)
        { return __copy(first, last, result, STL::iterator_category(first)); }
    };
    
    // 偏特化T*, T*
    template <class T>
    struct _copy<T*, T*>
    {
        T* operator()(T* first, T* last, T* result)
        {
            // 指针所指是否具有默认的赋值操作符
            typedef typename STL::has_trivial_copy_assign<T> t;
            return __copy_t(first, last, result, t());
        }
    };
    
    // 偏特化const T*, T*
    template <class T>
    struct _copy<const T*, T*>
    {
        T* operator()(const T* first, const T* last, T* result)
        {
            typedef typename STL::has_trivial_copy_assign<T> t;
            return __copy_t(first, last, result, t());
        }
    };

    /********** copy() **********/

    // 完全泛化
    template <class InputIterator, class OutputIterator>
    inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
    { return _copy<InputIterator, OutputIterator>()(first, last, result); }
    
    // 以下是整组copy_backward()函数
    
    /********** __copy_backward() **********/
    
    // first是BidirectionalIterator
    template <class BidirectionalIterator1, class BidirectionalIterator2>
    inline BidirectionalIterator2 __copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result, STL::bidirectional_iterator_tag)
    {
        while (last != first) {
            *(--result) = *(--last);
        }
        return result;
    }
    
    /********** __copy_backward_t() **********/

    // 指针所指对象具备默认赋值操作符
    template <class T>
    inline T* __copy_backward_t(T* first, T* last, T* result, STL::true_type)
    {
        memmove(result - (last - first), first, sizeof(T) * (last - first));
        return result - (last - first);
    }

    // 指针所指对象不具备默认赋值操作符
    template <class T>
    inline T* __copy_backward_t(T* first, T* last, T* result, STL::false_type)
    { return __copy_backward(first, last, result, STL::random_access_iterator_tag()); } // 原生指针是RandomAccessIterator，交给__copy_backward()

    template <class T>
    inline T* __copy_backward_t(const T* first, const T* last, T* result, STL::true_type)
    {
        memmove(result - (last - first), first, sizeof(T) * (last - first));
        return result - (last - first);
    }

    template <class T>
    inline T* __copy_backward_t(const T* first, const T* last, T* result, STL::false_type)
    { return __copy_backward(first, last, result, STL::random_access_iterator_tag()); } // 原生指针是RandomAccessIterator，交给__copy_backward()

    /********** _copy_backward() **********/
    
    // 完全泛化，first的迭代器类型必须是BidirectionalIterator
    template <class BidirectionalIterator1, class BidirectionalIterator2>
    struct _copy_backward
    {
        BidirectionalIterator2 operator()(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result)
        { return __copy_backward(first, last, result, STL::iterator_category(first)); }
    };
    
    // 偏特化T*, T*
    template <class T>
    struct _copy_backward<T*, T*>
    {
        T* operator()(T* first, T* last, T* result)
        {
            // 指针所指是否具有默认的赋值操作符
            typedef typename STL::has_trivial_copy_assign<T> t;
            return __copy_backward_t(first, last, result, t());
        }
    };
    
    // 偏特化const T*, T*
    template <class T>
    struct _copy_backward<const T*, T*>
    {
        T* operator()(const T* first, const T* last, T* result)
        {
            typedef typename STL::has_trivial_copy_assign<T> t;
            return __copy_backward_t(first, last, result, t());
        }
    };
    /********** copy_backward() **********/
    
    // 完全泛化 
    template <class BidirectionalIterator1, class BidirectionalIterator2>
    inline BidirectionalIterator2 copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result)
    { return _copy_backward<BidirectionalIterator1, BidirectionalIterator2>()(first, last, result); }

    template <class IIt1, class IIt2>
    inline bool equal(IIt1 first1, IIt1 last1, IIt2 first2)
    {
        for ( ; first1 != last1; ++first1, ++first2)
            if (*first1 != *first2)
                return false;
        return true;
    }

} /* namespace STL */ 

#endif
