#ifndef TINYSTL_TYPE_TRAITS_H_
#define TINYSTL_TYPE_TRAITS_H_ 

namespace STL
{

    template <class T, T v>
    struct integral_constant
    {
        static constexpr T value = v;
        typedef T                       value_type;
        typedef integral_constant<T, v> type;
        constexpr operator value_type() const { return value; }
    };

    template <class T, T v>
    constexpr T integral_constant<T, v>::value;

    typedef integral_constant<bool, true>   true_type;
    typedef integral_constant<bool, false>  false_type;
    
    template <class T>
    struct has_trivial_default_constructor
    : public integral_constant<bool, __has_trivial_constructor(T)> {};
    
    template <class T>
    struct has_trivial_copy_constructor
    : public integral_constant<bool, __has_trivial_copy(T)> {};
    
    template <class T>
    struct has_trivial_copy_assign
    : public integral_constant<bool, __has_trivial_assign(T)> {};
    
    template <class T>
    struct has_trivial_destructor 
    : public integral_constant<bool, __has_trivial_destructor(T)> {};
    
    template <class T>
    struct is_pod
    : public integral_constant<bool, __is_pod(T)> {};
    
} /* namespace STL */ 

#endif
