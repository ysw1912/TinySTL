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
        typedef Category    iterator_category;
        typedef T           value_type;
        typedef Distance    difference_type;
        typedef Pointer     pointer;
        typedef Reference   reference;
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

} /* namespace STL */

#endif
