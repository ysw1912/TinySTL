#ifndef TINYSTL_VECTOR_H_
#define TINYSTL_VECTOR_H_ 

#include "allocator.h"
#include "uninitialized.h"

namespace STL {
    template <class T, class Alloc = STL::pool_alloc>
    class vector {
    public:
        // vector类型定义
        typedef T           value_type;
        typedef T*          pointer;
        typedef T*          iterator;
        typedef T&          reference;
        typedef size_t      size_type;
        typedef ptrdiff_t   difference_type;

    protected:
        typedef STL::allocator<T, Alloc> data_allocator;
        iterator start;           // 使用空间的头
        iterator finish;          // 使用空间的尾
        iterator end_of_storage;  // 可用空间的尾

    public:
        // 构造、拷贝、析构
        vector() : start(0), finish(0), end_of_storage(0) {}
        explicit vector(size_type n) { fill_initialize(n, T()); }
        vector(size_type n, const value_type &x) { fill_initialize(n, x); }
        ~vector();

        iterator begin() { return start; }
        iterator end() { return finish; }
        
        // 访问元素
        reference operator[](size_type n) { return *(begin() + n); }
        reference front() { return *begin(); }
        reference back() { return *(end() - 1); }
        
        // 容量
        size_type size() const { return static_cast<size_type>(finish - start); }
        size_type capacity() const { return static_cast<size_type>(end_of_storage - start); }
        bool empty() const { return start == finish; }

        // 修改容器

    protected:
        void deallocate() {
            if (start)  data_allocator::deallocate(start, end_of_storage - start);
        }
        void fill_initialize(size_type n, const value_type &x);
        iterator allocate_and_fill(size_type n, const value_type &x);
    };

    // 析构函数
    template <class T, class Alloc>
    vector<T, Alloc>::~vector() {
        STL::destroy(start, finish);
        deallocate();   // protected成员函数
    }

    // 填充并初始化
    template <class T, class Alloc>
    void vector<T, Alloc>::fill_initialize(size_type n, const value_type &x) {
        start = allocate_and_fill(n, x);
        finish = start + n;
        end_of_storage = finish;
    }

    // allocator分配内存，并fill_n填充
    template <class T, class Alloc>
    typename vector<T, Alloc>::iterator
    vector<T, Alloc>::allocate_and_fill(size_type n, const value_type &x) {
        iterator result = data_allocator::allocate(n);
        STL::uninitialized_fill_n(result, n, x);
        return result;
    }


} /* namespace STL */

#endif
