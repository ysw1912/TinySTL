#ifndef TINYSTL_ALLOCATOR_H_
#define TINYSTL_ALLOCATOR_H_ 

#include <cstddef>
#include "alloc.h"
#include "construct.h"

namespace STL {
    
    // 标准空间配置器allocator
    template <class T>
    class allocator {
    public:
        typedef T           value_type;
        typedef T*          pointer;
        typedef const T*    const_pointer;
        typedef T&          reference;
        typedef const T&    const_reference;
        typedef size_t      size_type;
        typedef ptrdiff_t   difference_type;

    public:
        // rebind allocator of type U
        template <class U>
        struct rebind {
            typedef allocator<U> other;
        };

    };

} /* namespace STL */

#endif
