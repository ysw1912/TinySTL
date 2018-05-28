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
        using iterator_category = typename Iter::iterator_category;
        using value_type        = typename Iter::value_type;
        using pointer           = typename Iter::pointer;
        using reference         = typename Iter::reference;
        using difference_type   = typename Iter::difference_type;
    }; 

    // 针对原生指针的traits
    template <class T>
    struct iterator_traits<T *> {
        using iterator_category = random_access_iterator_tag;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;
        using difference_type   = ptrdiff_t;
    };

    // 针对原生const指针的traits
    template <class T>
    struct iterator_traits<const T *> {
        using iterator_category = random_access_iterator_tag;
        using value_type        = T;
        using pointer           = const T*;
        using reference         = const T&;
        using difference_type   = ptrdiff_t;
    };

    template <class Iter>
    inline typename iterator_traits<Iter>::iterator_category 
    iterator_category(const Iter&) {
        using category = typename iterator_traits<Iter>::iterator_category;
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
    
    // 以下为整组advance()函数
    template <class InputIterator, class Distance>
    inline void _advance(InputIterator& i, Distance n, input_iterator_tag) {
       while (n--)  ++i;
    } 

    template <class BidirectionalIterator, class Distance>
    inline void _advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
        if (n > 0)
            while (n--) ++i;
        else 
            while (n++) --i;
    }
    
    template <class RandomAccessIterator, class Distance>
    inline void _advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag) {
        i += n;
    }

    /**
     *  @brief  将迭代器i增加n
     */ 
    template <class InputIterator, class Distance>
    inline void advance(InputIterator& i, Distance n) {
        _advance(i, n, iterator_category(i));
    }

    // 以下为整组distance()函数
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
     **  @brief  计算迭代器first到last的距离
     **/
    template <class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type 
    distance(InputIterator first, InputIterator last) {
        using category = typename iterator_traits<InputIterator>::iterator_category;
        return _distance(first, last, category());
    }

} /* namespace STL */

#endif
