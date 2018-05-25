#ifndef TINYSTL_ITERATOR_H_
#define TINYSTL_ITERATOR_H_ 

#include <cstddef>

namespace STL {

    struct input_iterator_tag {};
    struct output_iterator_tag {};
    struct forward_iterator_tag : public input_iterator_tag {};
    struct bidirectional_iterator_tag : public forward_iterator_tag {};
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};

    // 迭代器都继承自下面这个iterator
    template <class Category, class T, class Distance = ptrdiff_t,
              class Pointer = T*, class Reference = T&>
    struct iterator {
        using iterator_category = Category;
        using value_type        = T;
        using pointer           = Pointer;
        using reference         = Reference;
        using difference_type   = Distance;
    };

    // traits 
    template <class Iter>
    struct iterator_traits {
        typedef typename Iter::iterator_category    iterator_category;
        typedef typename Iter::value_type           value_type;
        typedef typename Iter::difference_type      difference_type;
        typedef typename Iter::pointer              pointer;
        typedef typename Iter::reference            reference;
    }; 

    // 针对原生指针的traits
    template <class T>
    struct iterator_traits<T *> {
        typedef random_access_iterator_tag  iterator_category;
        typedef T                           value_type;
        typedef ptrdiff_t                   difference_type;
        typedef T*                          pointer;
        typedef T&                          reference;
    };

    // 针对原生const指针的traits
    template <class T>
    struct iterator_traits<const T *> {
        typedef random_access_iterator_tag  iterator_category;
        typedef T                           value_type;
        typedef ptrdiff_t                   difference_type;
        typedef const T*                    pointer;
        typedef const T&                    reference;
    };

    template <class Iter>
    inline typename iterator_traits<Iter>::iterator_category 
    iterator_category(const Iter&) {
        typedef typename iterator_traits<Iter>::iterator_category category;
        return category();
    }

    template <class Iter>
    inline typename iterator_traits<Iter>::difference_type *
    distance_type(const Iter&) {
        return static_cast<typename iterator_traits<Iter>::difference_type *>(0);
    }

    template <class Iter>
    inline typename iterator_traits<Iter>::value_type *
    value_type(const Iter&) {
        return static_cast<typename iterator_traits<Iter>::value_type *>(0);
    }
    
    // distance()
    template <class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type 
    _distance(InputIterator first, InputIterator last, input_iterator_tag) {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last) {
            ++first;
            ++n;
        }
        return n;
    }

    template <class RandomAccessIterator>
    inline typename iterator_traits<RandomAccessIterator>::difference_type 
    _distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
        return last - first;
    }

    /** 
     **  @brief 通用指针算数操作
     **  @param  first   输入迭代器
     **  @param  last    输入迭代器
     **  @return  它们之间的距离
     **
     **  对于random access iterator，只需要O(1)
     **/
    template <class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type 
    distance(InputIterator first, InputIterator last) {
        typedef typename iterator_traits<InputIterator>::iterator_category category;
        return _distance(first, last, category());
    }

} /* namespace STL */

#endif
