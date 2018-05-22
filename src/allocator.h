#ifndef TINYSTL_ALLOCATOR_H_
#define TINYSTL_ALLOCATOR_H_ 

#include <cstddef>
#include "alloc.h"
#include "construct.h"

namespace STL {
    
    // 标准空间配置器allocator
    template <class T, class Alloc>
    class allocator {
    public:
        typedef T           value_type;
        typedef T*          pointer;
        typedef const T*    const_pointer;
        typedef T&          reference;
        typedef const T&    const_reference;
        typedef size_t      size_type;
        typedef ptrdiff_t   difference_type;

        // rebind allocator of type U
        template <class U>
        struct rebind {
            typedef allocator<U, Alloc> other;
        };

        static T* allocate();
        static T* allocate(size_t n);
        static void deallocate(T *p);
        static void deallocate(T *p, size_t n);
    };

    template <class T, class Alloc>
    T* allocator<T, Alloc>::allocate() {
        return (T*)Alloc::allocate(sizeof(T));
    }
    
    template <class T, class Alloc>
    T* allocator<T, Alloc>::allocate(size_t n) {
        return 0 == n ? 0 : (T*)Alloc::allocate(sizeof(T) * n);
    }
    
    template <class T, class Alloc>
    void allocator<T, Alloc>::deallocate(T *p) {
        Alloc::deallocate(p, sizeof(T));
    }
    
    template <class T, class Alloc>
    void allocator<T, Alloc>::deallocate(T *p, size_t n) {
        if (0 != n) Alloc::deallocate(p, sizeof(T) * n);
    }

} /* namespace STL */

#endif
