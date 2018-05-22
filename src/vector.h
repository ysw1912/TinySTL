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
        void push_back(const T &x);
        void pop_back();
        iterator erase(iterator pos);

    protected:
        void fill_initialize(size_type n, const value_type &x);
        iterator allocate_and_fill(size_type n, const value_type &x);
        void deallocate() {
            if (start)  data_allocator::deallocate(start, end_of_storage - start);
        }
        void insert_aux(iterator pos, const value_type &x);
    };

    // 析构函数
    template <class T, class Alloc>
    vector<T, Alloc>::~vector() {
        STL::destroy(start, finish);
        deallocate();   // protected成员函数
    }

    // 将元素插入尾端
    template <class T, class Alloc>
    void vector<T, Alloc>::push_back(const T &x) {
        if (finish != end_of_storage) {
            STL::construct(finish, x);
            ++finish;
        } else {
            insert_aux(finish, x);
        }
    }

    // 删除尾端元素
    template <class T, class Alloc>
    void vector<T, Alloc>::pop_back() {
        --finish;
        destroy(finish);
    }

    // 清除某位置上元素
    template <class T, class Alloc>
    typename vector<T, Alloc>::iterator 
    vector<T, Alloc>::erase(iterator pos) {
        if (pos + 1 != finish) {
            STL::copy(pos + 1, finish, pos);
        }
        --finish;
        destroy(finish);
        return pos;
    }

    /********** protected member function **********/

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

    // 在pos处插入元素x
    template <class T, class Alloc>
    void vector<T, Alloc>::insert_aux(iterator pos, const value_type &x) {
        if (finish != end_of_storage) { // 有备用空间 
            STL::construct(finish, *(finish - 1)); // 在备用空间起始处，用当前最后一个元素构造初值
            ++finish;
            STL::copy_backward(pos, finish - 2, finish - 1);
            *pos = x;
        } else {    // 无备用空间
            // 若原大小为0，则配置1个元素大小
            // 否则，配置为原大小的2倍
            const size_type old_size = size();
            const size_type len = old_size != 0 ? 2 * old_size : 1;

            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;
            try {
                // 将原数据pos之前的内容拷贝到新空间
                new_finish = STL::uninitialized_copy(start, pos, new_start);
                // 为插入元素设初值x
                STL::construct(new_finish, x);
                ++new_finish;
                // 将原数据pos之后的内容拷贝到新空间
                new_finish = STL::uninitialized_copy(pos, finish, new_finish);
            }
            catch(...) {
                // commit or rollback
                STL::destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw;
            }

            // 析构原vector
            STL::destroy(start, finish);
            deallocate();

            // 调整迭代器，指向新vector
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }

} /* namespace STL */

#endif
