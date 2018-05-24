#ifndef TINYSTL_VECTOR_H_
#define TINYSTL_VECTOR_H_ 

#include <initializer_list>
#include <type_traits>

#include "allocator.h"
#include "uninitialized.h"

namespace STL {
    template <class T, class Alloc = STL::pool_alloc>
    class vector {
    public:
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

    protected:
        void fill_initialize(size_type n, const value_type &x);
        iterator allocate_and_fill(size_type n, const value_type &x);

        template <class ForwardIterator>
        void range_initialize(ForwardIterator first, ForwardIterator last, STL::forward_iterator_tag);

        void deallocate() {
            if (start)  data_allocator::deallocate(start, end_of_storage - start);
        }

        void insert_aux(iterator pos, const value_type &x);

        // 定义在下
        void fill_insert(iterator pos, size_type n, const value_type &x);

        template <class InputIterator>
        void range_insert(iterator pos, InputIterator first, InputIterator last, STL::input_iterator_tag);

        template <class ForwardIterator>
        void range_insert(iterator pos, ForwardIterator first, ForwardIterator last, STL::forward_iterator_tag);

        template <class Integer>
        void insert_dispatch(iterator pos, Integer n, Integer x, std::true_type) {
            fill_insert(pos, n, x);
        }

        template <class InputIterator>
        void insert_dispatch(iterator pos, InputIterator first, InputIterator last, std::false_type) {
            typedef typename STL::iterator_traits<InputIterator>::iterator_category category;
            range_insert(pos, first, last, category());
        }

    public:
        // 构造、拷贝、析构
        
        /**
         *  @brief  构造函数
         */ 
        vector() : start(0), finish(0), end_of_storage(0) {}
        explicit vector(size_type n) { fill_initialize(n, T()); }
        vector(size_type n, const value_type &x) { fill_initialize(n, x); }
        vector(std::initializer_list<value_type> l) { range_initialize(l.begin(), l.end(), STL::random_access_iterator_tag()); }
        
        /** 
         *  @brief  拷贝构造函数
         */ 
        vector(const vector &v) { range_initialize(v.begin(), v.end(), STL::random_access_iterator_tag()); }

        /**
         *  @brief  赋值操作符
         */
        vector& operator = (const vector &v) {
            if (this != &v) {
                range_initialize(v.begin(), v.end(), STL::random_access_iterator_tag());
            }
            return *this;
        }

        /**
         *  @brief  析构函数
         */
        ~vector() {
            STL::destroy(start, finish);
            deallocate();
        }
    
        // 元素访问
        reference operator[](size_type n) const noexcept { return *(start + n); }
        reference front() const noexcept { return *start; }
        reference back() const noexcept { return *(finish - 1); }
        
        // 迭代器
        iterator begin() const noexcept { return start; }
        iterator end() const noexcept { return finish; }
        
        // 容量
        bool empty() const noexcept { return start == finish; }
        size_type size() const noexcept { return static_cast<size_type>(finish - start); }
        size_type capacity() const noexcept { return static_cast<size_type>(end_of_storage - start); }

        // 修改容器
        
        iterator insert(iterator pos, const value_type &x);

        /**
         *  @brief  向pos位置插入n个x的拷贝
         *  @return 
         */ 
        void insert(iterator pos, size_type n, const value_type &x) { fill_insert(pos, n, x); }

        /** C++1.0
         *  @brief  向pos位置插入[first, last)范围的拷贝
         *  @return  指向被插入数据的iterator
         *
         *  根据first和last的类型是否是integral，本函数可能与
         *  void insert(iterator, size_type, const value_type&)混淆
         *  需要注意区分
         */
        template <class InputIterator>
        void insert(iterator pos, InputIterator first, InputIterator last) {
            typedef typename std::is_integral<InputIterator>::type is_integral;
            insert_dispatch(pos, first, last, is_integral());
        }

        void push_back(const T &x);
        void pop_back();
        iterator erase(iterator pos);
    };

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


    template <class T, class Alloc>
    template <class ForwardIterator>
    void vector<T, Alloc>::range_initialize(ForwardIterator first, ForwardIterator last, STL::forward_iterator_tag) {
        const difference_type n = STL::distance(first, last);
        start = data_allocator::allocate(n);
        end_of_storage = start + n;
        finish = STL::uninitialized_copy(first, last, start);
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

    // 从pos开始，插入n个元素，元素初值为x
    template <class T, class Alloc>
    void vector<T, Alloc>::fill_insert(iterator pos, size_type n, const value_type &x) {
        if (n != 0) {
            if (size_type(end_of_storage - finish) >= n) {  // "备用空间"大于等于"新增元素个数"
                value_type x_copy = x;
                // 插入点之后的现有元素个数
                const size_type elems_after = finish - pos;
                // old_finish变量在源码中有使用
                // iterator old_finish(finish);
                if (elems_after > n) {  // "pos后元素个数"大于"新增元素个数"
                    STL::copy_backward(pos, finish, finish + n);
                    STL::fill(pos, pos + n, x_copy);
                    finish += n;
                    // 以下为源码方法
                    // STL::uninitialized_copy(finish - n, finish, finish);
                    // finish += n;
                    // STL::copy_backward(pos, old_finish - n, old_finish);
                    // STL::fill(pos, pos + n, x_copy);
                } else {    // "pos后元素个数"小于等于"新增元素个数"
                    STL::uninitialized_copy(pos, finish, pos + n);
                    STL::fill(pos, pos + n, x_copy);
                    finish += n;
                    // 以下为源码方法
                    // finish = STL::uninitialized_fill_n(finish, n - elems_after, x_copy);
                    // STL::uninitialized_copy(pos, old_finish, finish);
                    // finish += elems_after;
                    // STL::fill(pos, old_finish, x_copy);
                }
            } else {    // "备用空间"小于"新增元素个数"
                // 新长度 = 2倍旧长度 or 旧长度 + 新增元素个数
                const size_type old_size = size();
                const size_type len = old_size + (old_size > n ? old_size : n);
                // 配置新的vector空间
                iterator new_start(data_allocator::allocate(len));
                iterator new_finish(new_start);
                try {
                    new_finish = iterator();
                    // 将旧vector插入点之前的元素复制
                    new_finish = STL::uninitialized_copy(start, pos, new_start);
                    // 将新增元素(n个x)填入vector
                    new_finish = STL::uninitialized_fill_n(new_finish, n, x);
                    // 将旧vector插入点之后的元素复制
                    new_finish = STL::uninitialized_copy(pos, finish, new_finish);
                } catch (...) {
                    // commit or rollback
                    if (!new_finish)
                        STL::destroy(new_start, new_start + (pos - start));
                    else 
                        STL::destroy(new_start, new_finish);
                    data_allocator::deallocate(new_start, len);
                    throw;
                }
                // 释放旧vector
                STL::destroy(start, finish);
                deallocate();
                // 调整标记
                start = new_start;
                finish = new_finish;
                end_of_storage = start + len;
            }
        }
    }

    template <class T, class Alloc>
    template <class InputIterator>
    void vector<T, Alloc>::range_insert(iterator pos, InputIterator first, InputIterator last, STL::input_iterator_tag) {
        for ( ; first != last; ++first) {
            pos = insert(pos, *first);
            ++pos;
        }    
    }

    template <class T, class Alloc>
    template <class ForwardIterator>
    void vector<T, Alloc>::range_insert(iterator pos, ForwardIterator first, ForwardIterator last, STL::forward_iterator_tag) {
        if (first != last) {
            const size_type n = STL::distance(first, last); // 新增元素个数
            if (size_type(end_of_storage - finish) >= n) {
                const size_type elems_after = finish - pos;
                if (elems_after > n) {  // "pos后元素个数"大于"新增元素个数"
                    STL::copy_backward(pos, finish, finish + n);
                    STL::copy(first, last, pos);
                    finish += n;
                } else {    // "pos后元素个数"小于等于"新增元素个数"
                    STL::uninitialized_copy(pos, finish, pos + n);
                    STL::copy(first, last, pos);
                    finish += n;
                }
            } else {    // "备用空间"小于"新增元素个数"
                // 新长度 = 2倍旧长度 or 旧长度 + 新增元素个数
                const size_type old_size = size();
                const size_type len = old_size + (old_size > n ? old_size : n);
                // 配置新的vector空间
                iterator new_start(data_allocator::allocate(len));
                iterator new_finish(new_start);
                try {
                    new_finish = iterator();
                    // 将旧vector插入点之前的元素复制
                    new_finish = STL::uninitialized_copy(start, pos, new_start);
                    // 将新增元素(n个x)填入vector
                    new_finish = STL::uninitialized_fill_n(new_finish, n, x);
                    // 将旧vector插入点之后的元素复制
                    new_finish = STL::uninitialized_copy(pos, finish, new_finish);
                } catch (...) {
                    // commit or rollback
                    if (!new_finish)
                        STL::destroy(new_start, new_start + (pos - start));
                    else 
                        STL::destroy(new_start, new_finish);
                    data_allocator::deallocate(new_start, len);
                    throw;
                }
                // 释放旧vector
                STL::destroy(start, finish);
                deallocate();
                // 调整标记
                start = new_start;
                finish = new_finish;
                end_of_storage = start + len;
            }
        }
    }

} /* namespace STL */

#endif
