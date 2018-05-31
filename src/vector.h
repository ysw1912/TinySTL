#ifndef TINYSTL_VECTOR_H_
#define TINYSTL_VECTOR_H_ 

#include <initializer_list>
#include <type_traits>
#include <utility>

#include "allocator.h"
#include "uninitialized.h"

namespace STL {
    template <class T, class Alloc = STL::pool_alloc>
    class vector {
    public:
        using value_type        = T;
        using iterator          = T*;
        using const_iterator    = const T*;
        using pointer           = T*;
        using const_pointer     = const T*;
        using reference         = T&;
        using const_reference   = const T&;
        using size_type         = size_t;
        using difference_type   = ptrdiff_t;

    protected:
        using data_allocator    = STL::allocator<T, Alloc>;
        iterator start;           // 使用空间的头
        iterator finish;          // 使用空间的尾
        iterator end_of_storage;  // 可用空间的尾

    protected:
        // 分配n个value_type的空间，并填充x进去
        // 被fill_initialize调用
        iterator allocate_and_fill(size_type n, const value_type &x) {
            iterator result = data_allocator::allocate(n);
            STL::uninitialized_fill_n(result, n, x);
            return result;
        }

        // 为vector分配内存，并填充n个x
        void fill_initialize(size_type n, const value_type &x) {
            start = allocate_and_fill(n, x);
            finish = start + n;
            end_of_storage = finish;
        }

        // 用[first, last)范围元素初始化vector
        template <class ForwardIterator>
        void range_initialize(ForwardIterator first, ForwardIterator last, STL::forward_iterator_tag) {
            const difference_type n = STL::distance(first, last);
            start = data_allocator::allocate(n);
            end_of_storage = start + n;
            finish = STL::uninitialized_copy(first, last, start);
        }

        // 获得n字节内存，并将[first, last)范围元素拷贝进去
        // 被range_assign调用
        template <class ForwardIterator>
        pointer allocate_and_copy(size_type n, ForwardIterator first, ForwardIterator last) {
            pointer result = data_allocator::allocate(n);
            try {
                STL::uninitialized_copy(first, last, result);
                return result;
            } catch(...) {
                data_allocator::deallocate(result, n);
                throw;
            }
        }

        // 清除[pos, finish)的元素
        // 被fill_assign, range_assign调用
        void erase_at_end(iterator pos) {
            STL::destroy(pos, finish);
            finish = pos;
        }
        
        // 赋n个x值给vector
        void fill_assign(size_type n, const value_type& x) {
            if (n > capacity()) {
                vector tmp(n, x);
                swap_data(tmp);
            } else if (n > size()) {
                STL::fill(start, finish, x);
                finish = STL::uninitialized_fill_n(finish, n - size(), x);
            } else {
                erase_at_end(STL::fill_n(start, n, x));
            }
        }
        
        // 被assign(first, last)调用
        // first, last是integral的情况
        template <class Integer>
        void assign_dispatch(Integer n, Integer x, std::true_type) {
            fill_assign(n, x);
        }
        // first, last是iterator的情况
        template <class InputIterator>
        void assign_dispatch(InputIterator first, InputIterator last, std::false_type) {
            using category = typename STL::iterator_traits<InputIterator>::iterator_category;
            range_assign(first, last, category());
        }

        // 被第二种assign_dispatch调用
        // InputIterator的情况
        template <class InputIterator>
        void range_assign(InputIterator first, InputIterator last, STL::input_iterator_tag) {
            iterator cur(start);
            for ( ; first != last && cur != finish; ++cur, ++first)
                *cur = *first;
            if (first == last)
                erase_at_end(cur);
            else  
                insert(finish, first, last);
        }
        // ForwardIterator的情况
        template <class ForwardIterator>
        void range_assign(ForwardIterator first, ForwardIterator last, STL::forward_iterator_tag) {
            const size_type len = STL::distance(first, last);
            if (len > capacity()) {
                pointer tmp(allocate_and_copy(len, first, last));
                STL::destroy(start, finish);
                deallocate();
                start = tmp;
                finish = end_of_storage = start + len;
            } else if (len > size()) {
                ForwardIterator mid = first;
                STL::advance(mid, size());
                STL::copy(first, mid, start);
                finish = uninitialized_copy(mid, last, finish);
            } else {
                erase_at_end(STL::copy(first, last, start));
            }
        }

        // 将vector x移动赋值给vector
        void move_assign(vector&& x) {
            vector tmp;
            swap_data(tmp);
            swap_data(x);
        }

        // 将vector与x交换数据
        void swap_data(vector& x) {
            STL::swap(start, x.start);
            STL::swap(finish, x.finish);
            STL::swap(end_of_storage, x.end_of_storage);
        }

        // 释放vector [start, end_of_storage)的内存
        void deallocate() {
            if (start)  data_allocator::deallocate(start, end_of_storage - start);
        }

    public:
        // 构造、拷贝、移动、赋值、析构
        
        /**
         *  @brief  constructor
         */ 
        vector() : start(0), finish(0), end_of_storage(0) {}
        
        explicit vector(size_type n, const value_type &x = value_type()) { fill_initialize(n, x); }
        
        template <class InputIterator>
        vector(InputIterator first, InputIterator last) { range_initialize(first, last, STL::random_access_iterator_tag()); } 

        vector(std::initializer_list<value_type> l) { range_initialize(l.begin(), l.end(), STL::random_access_iterator_tag()); }
        
        /** 
         *  @brief  copy constructor
         */ 
        vector(const vector &x) { range_initialize(x.begin(), x.end(), STL::random_access_iterator_tag()); }

        /**
         *  @brief  move constructor
         */ 
        vector(vector&& x) noexcept {
            start = finish = end_of_storage = 0;
            swap_data(x);
        }

        /**
         *  @brief  copy assignment operator 
         */
        vector& operator=(const vector &v) {
            if (this != &v) {
                range_initialize(v.begin(), v.end(), STL::random_access_iterator_tag());
            }
            return *this;
        }

        /**
         *  @brief  move assignment operator 
         */ 
        vector& operator=(vector&& x) noexcept {
            move_assign(std::move(x));
            return *this;
        }
        
        /**
         *  @brief  将n个x赋值给vector
         */ 
        void assign(size_type n, const value_type& x) { fill_assign(n, x); }

        /**
         *  @brief  将[first, last)范围元素赋值给vector
         *
         *  需要区分参数类型是iterator还是integral
         */ 
        template <class InputIterator>
        void assign(InputIterator first, InputIterator last) {
            using is_integral = typename std::is_integral<InputIterator>::type;
            assign_dispatch(first, last, is_integral());
        }

        /**
         *  @brief  destructor
         */
        ~vector() {
            STL::destroy(start, finish);
            deallocate();
        }

    protected:
        // 下标越界检查，只用于at()
        void range_check(size_type n) const {
           if (n >= size())
              throw;
        }
    public: 
        // 元素访问
        reference at(size_type n) {
            range_check(n);
            return (*this)[n];
        }
        const_reference at(size_type n) const {
            range_check(n);
            return (*this)[n];
        }
        reference operator[](size_type n) { return *(start + n); }
        const_reference operator[](size_type n) const { return *(start + n); }
        reference front() { return *start; }
        const_reference front() const { return *start; }
        reference back() { return *(finish - 1); }
        const_reference back() const { return *(finish - 1); }
        
        /**
         *  @brief  返回指向vector第一个元素的指针
         */ 
        pointer data() { return start; }
        const_pointer data() const noexcept { return start; } 

    public:
        // 迭代器
        iterator begin() noexcept { return iterator(start); }
        const_iterator begin() const noexcept { return const_iterator(start); }
        const_iterator cbegin() const noexcept { return const_iterator(start); }
        iterator end() noexcept { return iterator(finish); }
        const_iterator end() const noexcept { return const_iterator(finish); }
        const_iterator cend() const noexcept { return const_iterator(finish); }
    
    public: 
        // 容量
        bool empty() const noexcept { return start == finish; }
        size_type size() const noexcept { return static_cast<size_type>(finish - start); }
        size_type max_size() const noexcept { return data_allocator::max_size(); }
        
        /**
         *  @brief  增加vector的容量到n，仅在n大于当前的capacity()时，才分配新存储 
         */ 
        void reserve(size_type n) {
            if (n > max_size())
                throw;
            if (n > capacity()) {
                const size_type old_size = size();
                pointer tmp(allocate_and_copy(n, start, finish));
                STL::destroy(start, finish);
                deallocate();
                start = tmp;
                finish = start + old_size;
                end_of_storage = start + n; 
            }
        }
         
        size_type capacity() const noexcept { return static_cast<size_type>(end_of_storage - start); }

    protected:
        // 在pos前插入args
        // 被insert(const_iterator, const value_type&)调用
        template <class... Args>
        void insert_aux(iterator pos, Args&&... args) {
            if (finish != end_of_storage) { // 有备用空间 
                STL::construct(finish, std::move(*(finish - 1))); // 在备用空间起始处，用当前最后一个元素构造初值
                ++finish;
                STL::copy_backward(pos, finish - 2, finish - 1);
                *pos = value_type(std::forward<Args>(args)...);
            } else {    // 无备用空间
                // 若原大小为0，则配置1个元素大小
                // 否则，配置为原大小的2倍
                const size_type old_size = size();
                const size_type len = old_size != 0 ? 2 * old_size : 1;
                const size_type elems_before = pos - start;
                pointer new_start(data_allocator::allocate(len));
                pointer new_finish(new_start);
                try {
                    // 为插入元素设初值x
                    STL::construct(new_start + elems_before, std::forward<Args>(args)...);

                    new_finish = pointer();
                    // 将原数据pos之前的内容拷贝到新空间
                    new_finish = STL::uninitialized_copy(start, pos, new_start);
                    ++new_finish;
                    // 将原数据pos之后的内容拷贝到新空间
                    new_finish = STL::uninitialized_copy(pos, finish, new_finish);
                }
                catch(...) {
                    // commit or rollback
                    if (!new_finish)
                        STL::destroy(new_start + elems_before);
                    else 
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

        // 在pos前插入x的n个拷贝
        // 被insert(const_iterator, size_type, const value_type&), 第一个insert_dispatch()调用
        void fill_insert(iterator pos, size_type n, const value_type& x) {
            if (n != 0) {
                if (size_type(end_of_storage - finish) >= n) {  // "备用空间"大于等于"新增元素个数"
                    value_type x_copy = x;
                    // 插入点之后的现有元素个数
                    const size_type elems_after = finish - pos;
                    pointer old_finish(finish);
                    if (elems_after > n) {  // "pos后元素个数"大于"新增元素个数"
                        STL::uninitialized_copy(finish - n, finish, finish);
                        finish += n;
                        STL::copy_backward(pos, old_finish - n, old_finish);
                        STL::fill(pos, pos + n, x_copy);
                    } else {    // "pos后元素个数"小于等于"新增元素个数"
                        finish = STL::uninitialized_fill_n(finish, n - elems_after, x_copy);
                        STL::uninitialized_copy(pos, old_finish, finish);
                        finish += elems_after;
                        STL::fill(pos, old_finish, x_copy);
                    }
                } else {    // "备用空间"小于"新增元素个数"
                    // 新长度 = 2倍旧长度 or 旧长度 + 新增元素个数
                    const size_type old_size = size();
                    const size_type len = old_size + (old_size > n ? old_size : n);
                    const size_type elems_before = pos - begin();
                    // 配置新的vector空间
                    pointer new_start(data_allocator::allocate(len));
                    pointer new_finish(new_start);
                    try {
                        // 将新增元素(n个x)填入vector
                        new_finish = STL::uninitialized_fill_n(new_start, n, x);

                        new_finish = pointer();
                        // 将旧vector插入点之前的元素复制
                        new_finish = STL::uninitialized_copy(start, pos, new_start);
                        new_finish += n;
                        // 将旧vector插入点之后的元素复制
                        new_finish = STL::uninitialized_copy(pos, finish, new_finish);
                    } catch(...) {
                        // commit or rollback
                        if (!new_finish)
                            STL::destroy(new_start + elems_before, new_start + elems_before + n);
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

        // 被insert(pos, first, last)调用
        // first, last是integral的情况
        template <class Integer>
        void insert_dispatch(iterator pos, Integer n, Integer x, std::true_type) {
            fill_insert(pos, n, x);
        }
        // first, last是iterator的情况
        template <class InputIterator>
        void insert_dispatch(iterator pos, InputIterator first, InputIterator last, std::false_type) {
            using category = typename STL::iterator_traits<InputIterator>::iterator_category;
            range_insert(pos, first, last, category());
        }

        // 被第二种insert_dispatch()调用
        // InputIterator的情况
        template <class InputIterator>
        void range_insert(iterator pos, InputIterator first, InputIterator last, STL::input_iterator_tag) {
            for ( ; first != last; ++first) {
                insert_aux(pos, *first);
                ++pos;
            }
        }
        // ForwardIterator的情况
        template <class ForwardIterator>
        void range_insert(iterator pos, ForwardIterator first, ForwardIterator last, STL::forward_iterator_tag) {
            if (first != last) {
                const size_type n = STL::distance(first, last); // 新增元素个数
                if (size_type(end_of_storage - finish) >= n) {
                    const size_type elems_after = finish - pos;
                    pointer old_finish(finish);
                    if (elems_after > n) {  // "pos后元素个数"大于"新增元素个数"
                        STL::uninitialized_copy(finish - n, finish, finish);    // ->move 
                        finish += n;
                        STL::copy_backward(pos, old_finish - n, old_finish);    // ->move 
                        STL::copy(first, last, pos);
                    } else {    // "pos后元素个数"小于等于"新增元素个数"
                        ForwardIterator mid = first;
                        STL::advance(mid, elems_after);
                        STL::uninitialized_copy(mid, last, finish);
                        finish += n - elems_after;
                        STL::uninitialized_copy(pos, old_finish, finish);   // ->move 
                        finish += elems_after;
                        STL::copy(first, mid, pos);
                    }
                } else {    // "备用空间"小于"新增元素个数"
                    // 新长度 = 2倍旧长度 or 旧长度 + 新增元素个数
                    const size_type old_size = size();
                    const size_type len = old_size + (old_size > n ? old_size : n);
                    // 配置新的vector空间
                    iterator new_start(data_allocator::allocate(len));
                    iterator new_finish(new_start);
                    try {
                        // 将旧vector插入点之前的元素复制
                        new_finish = STL::uninitialized_copy(start, pos, new_start);    // ->move 
                        // 将新增元素(n个x)填入vector
                        new_finish = STL::uninitialized_copy(first, last, new_finish);
                        // 将旧vector插入点之后的元素复制
                        new_finish = STL::uninitialized_copy(pos, finish, new_finish);  // ->move 
                    } catch(...) {
                        // commit or rollback
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
        
        // size()==capacity()时，向vector尾部插入args
        template <class... Args>
        void emplace_back_aux(Args&&... args) {
            // 若原大小为0，则配置1个元素大小
            // 否则，配置为原大小的2倍
            const size_type old_size = size();
            const size_type len = old_size != 0 ? 2 * old_size : 1;
            pointer new_start(data_allocator::allocate(len));
            pointer new_finish(new_start);
            try {
                // 在vector尾部构造初值x
                STL::construct(new_start + size(), std::forward<Args>(args)...);

                new_finish = pointer();
                // 将原数据内容拷贝到新空间
                new_finish = STL::uninitialized_copy(start, finish, new_start);    // ->move 
                ++new_finish;
            } catch(...) {
                // commit or rollback
                if (!new_finish)
                    STL::destroy(new_start + size());
                else 
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

    public:
        // 修改容器
    
        /**
         *  @brief  清除vector中所有元素
         *
         *  clear()并不会使capacity减少
         */ 
        void clear() noexcept { erase(start, finish); }

        /**
         *  @brief  在pos前插入x
         *  @return  指向被插入x的迭代器
         */ 
        iterator insert(const_iterator pos, const value_type& x) {
            const size_type n = pos - start;
            if (finish != end_of_storage && pos == finish) {
                STL::construct(finish, x);
                ++finish;
            } else {
                // 将const_iterator转为const iterator
                const iterator cpos = begin() + (pos - cbegin());
                if (finish != end_of_storage) {
                    value_type x_copy = x;
                    insert_aux(cpos, std::move(x_copy));
                } else {
                    insert_aux(cpos, x);
                }
            }
            return iterator(start + n);
        }

        /**
         *  @brief  在pos前插入x的右值引用
         *  @return  指向被插入x的迭代器
         */ 
        iterator insert(const_iterator pos, value_type&& x) { return emplace(pos, std::move(x)); }

        /**
         *  @brief  在pos前插入x的n个拷贝
         *  @return  指向首个被插入元素的迭代器，若n==0则为pos
         */ 
        iterator insert(const_iterator pos, size_type n, const value_type& x) {
            difference_type offset = pos - cbegin();
            fill_insert(begin() + offset, n, x);
            return start + offset;
        }

        /** 
         *  @brief  在pos前插入[first, last)范围元素的拷贝
         *  @return  指向首个被插入元素的迭代器，若first==last则为pos
         *
         *  需要区分参数类型是iterator还是integral
         */
        template <class InputIterator>
        iterator insert(const_iterator pos, InputIterator first, InputIterator last) {
            using is_integral = typename std::is_integral<InputIterator>::type;
            difference_type offset = pos - cbegin();
            insert_dispatch(begin() + offset, first, last, is_integral());
            return start + offset;
        }

        /**
         *  @brief  在pos前构造元素args
         */ 
        template <class... Args>
        iterator emplace(const_iterator pos, Args&&... args) {
            const size_type n = pos - start;
            if (finish != end_of_storage && pos == finish) {
                STL::construct(finish, std::forward<Args>(args)...);
                ++finish;
            } else {
                insert_aux(begin() + (pos - cbegin()), std::forward<Args>(args)...);
            }
            return iterator(start + n);
        }

        /**
         *  @brief  移除pos指向的元素
         *  @return  返回pos迭代器
         */ 
        iterator erase(const_iterator pos) {
            const iterator cpos = begin() + (pos - cbegin());
            if (cpos + 1 != finish) {
                STL::copy(cpos + 1, finish, cpos);
            }
            --finish;
            destroy(finish);
            return cpos;
        }

        /** 
         *  @brief  移除[first, last)中的所有元素
         *  @return  返回迭代器last
         *
         *  erase()并不会使capacity减少
         */ 
        iterator erase(const_iterator first, const_iterator last) {
            const iterator cfirst = begin() + (first - cbegin());
            const iterator clast = begin() + (last - cbegin());
            if (cfirst != clast)
                erase_at_end(STL::copy(clast, finish, cfirst));
            return cfirst;
        }

        /**
         *  @brief  将x插入vector尾部
         */ 
        void push_back(const value_type& x) {
            if (finish != end_of_storage) {
                STL::construct(finish, x);
                ++finish;
            } else {
                emplace_back_aux(x);
            }
        }

        /**
         *  @brief  移动x进vector尾部
         */ 
        void push_back(value_type&& x) { emplace_back(std::move(x)); }

        /**
         *  @brief  添加新元素args到vector尾部
         */ 
        template <class... Args>
        void emplace_back(Args&&... args) {
            if (finish != end_of_storage) {
                STL::construct(finish, std::forward<Args>(args)...);
                ++finish;
            } else {
                emplace_back_aux(std::forward<Args>(args)...);
            }
        }

        /**
         *  @brief  移除vector尾部元素
         */ 
        void pop_back() {
            --finish;
            destroy(finish);
        }
       
        /**
         *  @brief  和vector x交换数据内容
         */ 
        void swap(vector& x) {
            swap_data(x);
        } 
    };

} /* namespace STL */

#endif
