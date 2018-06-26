#ifndef TINYSTL_DEQUE_H_
#define TINYSTL_DEQUE_H_ 

#include <algorithm>    // for std::max
#include <initializer_list>

#include "allocator.h"
#include "iterator.h"
#include "uninitialized.h"

namespace STL 
{
    /**
     *  @brief  决定缓冲区内元素个数
     *  @param  size  一个元素的大小
     */ 
#ifndef DEQUE_BUF_SIZE
#define DEQUE_BUF_SIZE 512
#endif 

    inline size_t deque_buf_size(size_t size)
    {
        return (size < DEQUE_BUF_SIZE ? size_t(DEQUE_BUF_SIZE / size) : size_t(1));
    }

    /**
     *  @brief  deque::iterator
     */
    template <class T, class Ref, class Ptr>
    struct deque_iterator
    {
        using iterator          = deque_iterator<T, T&, T*>;
        using const_iterator    = deque_iterator<T, const T&, const T*>;
        using Elt_pointer       = T*;
        using Map_pointer       = T**;
        using Self              = deque_iterator;

        static size_t buffer_size() noexcept 
        { return deque_buf_size(sizeof(T)); }

        using iterator_category = STL::random_access_iterator_tag;
        using value_type        = T;
        using pointer           = Ptr;
        using reference         = Ref;
        using size_type         = size_t;
        using difference_type   = ptrdiff_t;
        
        Elt_pointer cur;    // buffer中现行元素
        Elt_pointer first;  // buffer头
        Elt_pointer last;   // buffer尾，不存放元素
        Map_pointer node;   // 指向控制中心

        deque_iterator(Elt_pointer x, Map_pointer y) noexcept 
        : cur(x), first(*y), last(*y + buffer_size()), node(y) { }

        deque_iterator() noexcept 
        : cur(), first(), last(), node() { }

        deque_iterator(const iterator& x) noexcept 
        : cur(x.cur), first(x.first), last(x.last), node(x.node) { }

        iterator M_const_cast() const noexcept 
        { return iterator(cur, node); }

        reference operator*() const noexcept 
        { return *cur; }

        pointer operator->() const noexcept 
        { return cur; }

        Self& operator++() noexcept 
        {
            ++cur;
            if (cur == last) {
                set_node(node + 1);
                cur = first;
            }
            return *this;
        }

        Self operator++(int) noexcept 
        {
            Self tmp = *this;
            ++*this;
            return tmp;
        }

        Self& operator--() noexcept 
        {
            if (cur == first) {
                set_node(node - 1);
                cur = last;
            }
            --cur;
            return *this;
        }

        Self operator--(int) noexcept 
        {
            Self tmp = *this;
            --*this;
            return tmp;
        }

        // 以下实现随机存取
        Self& operator+=(difference_type n) noexcept 
        {
            const difference_type offset = n + (cur - first);
            if (offset >= 0 && offset < difference_type(buffer_size()))
                cur += n;
            else {  // 目标位置不在同一buffer
                const difference_type node_offset = 
                    offset > 0 ? offset / difference_type(buffer_size())
                               : -difference_type((-offset - 1) / buffer_size()) - 1;
                set_node(node + node_offset);
                cur = first + (offset - node_offset * difference_type(buffer_size()));
            }
            return *this;
        }

        Self operator+(difference_type n) const noexcept 
        {
            Self tmp = *this;
            return tmp += n;
        }

        Self& operator-=(difference_type n) noexcept 
        { return *this += -n; }

        Self operator-(difference_type n) const noexcept 
        {
            Self tmp = *this;
            return tmp -= n;
        }

        reference operator[](difference_type n) const noexcept 
        { return *(*this + n); }

        // 前进/后退时跳到下/上一个buffer，cur应由调用者立即设置
        void set_node(Map_pointer new_node) noexcept 
        {
            node = new_node;
            first = *new_node;
            last = first + difference_type(buffer_size());
        }

        bool operator==(const Self& x) const noexcept 
        { return cur == x.cur; }

        bool operator!=(const Self& x) const noexcept
        { return !(*this == x); }

        bool operator<(const Self& x) const noexcept 
        { return node == x.node ? cur < x.cur : node < x.node; }

        bool operator>(const Self& x) const noexcept 
        { return x < *this; }

        difference_type operator-(const Self& x) const noexcept 
        {   // 中间的n个buffer + 两头
            return difference_type(buffer_size()) * (node - x.node - 1)
                 + (cur - first)
                 + (x.last - x.cur);
        }
    };

    template <class T, class Alloc = STL::pool_alloc>
    class deque 
    {
    public:
        using iterator          = deque_iterator<T, T&, T*>;
        using const_iterator    = deque_iterator<T, const T&, const T*>;

    protected:
        using Map_pointer       = typename iterator::Map_pointer;

        Map_pointer map;
        size_t map_size;
        iterator start;     // 第一个buffer的第一个元素
        iterator finish;    // 最后一个buffer的最后一个元素的下一位置

    public:
        using value_type        = T;
        using pointer           = T*;
        using const_pointer     = const T*;
        using reference         = T&;
        using const_reference   = const T&;
        using size_type         = size_t;
        using difference_type   = ptrdiff_t;

    protected:
        using data_allocator    = STL::allocator<value_type, Alloc>;
        using map_allocator     = STL::allocator<pointer, Alloc>;

        static size_t buffer_size() noexcept 
        { return deque_buf_size(sizeof(T)); }

    protected:
        // 分配buffer空间，返回T*
        pointer allocate_node()
        { return data_allocator::allocate(buffer_size()); }
        // 将T*所指buffer释放
        void deallocate_node(pointer p)
        { data_allocator::deallocate(p, buffer_size()); }

        // 为map中每个节点分配buffer空间
        void create_nodes(Map_pointer nstart, Map_pointer nfinish)
        {
            Map_pointer cur;
            try {
                for (cur = nstart; cur < nfinish; ++cur)
                    *cur = allocate_node();
            } catch(...) {
                destroy_nodes(nstart, cur);
                throw;
            }
        }
        // 释放map中每个节点的buffer空间
        void destroy_nodes(Map_pointer nstart, Map_pointer nfinish)
        {
            for (Map_pointer n = nstart; n < nfinish; ++n)
                deallocate_node(*n);
        }

        enum { initial_map_size = 8 };

        // 为map分配n个节点
        Map_pointer allocate_map(size_type n)
        { return map_allocator::allocate(n); }
        // 将p指向的map开始的n个节点释放
        void deallocate_map(Map_pointer p, size_type n)
        { map_allocator::deallocate(p, n); }

        // 初始化map
        void initialize_map(size_type num_elements)
        {
            // 所需节点数，若刚好整除则多分配一个节点
            const size_type num_nodes = num_elements / buffer_size() + 1;
            // 一个map管理至少8个节点，最多num_nodes + 2（前后预留备用）
            map_size = std::max(size_type(initial_map_size), size_type(num_nodes + 2));
            map = allocate_map(map_size);
            // 令nstart和nfinish指向map全部节点的中间区段
            // 使头尾两端可扩充空间一样大
            Map_pointer nstart = map + (map_size - num_nodes) / 2;
            Map_pointer nfinish = nstart + num_nodes;
            try {
                create_nodes(nstart, nfinish);
            } catch(...) {
                deallocate_map(map, map_size);
                map = Map_pointer();
                map_size = 0;
                throw;
            }
            // 为deque内的start和finish设值
            start.set_node(nstart);
            start.cur = start.first;
            finish.set_node(nfinish - 1);
            finish.cur = finish.first + num_elements % buffer_size();
        }


        // 用value初始化deque中[start, finish)的元素
        void fill_initialize(const value_type& value)
        {
            Map_pointer cur;
            try {
                for (cur = start.node; cur < finish.node; ++cur)
                    STL::uninitialized_fill(*cur, *cur + buffer_size(), value);
                // 最后一个buffer单独设初值
                STL::uninitialized_fill(finish.first, finish.cur, value);
            } catch(...) {
                destroy_data(start, iterator(*cur, cur));
                throw;
            }
        }

        // 被initialize_dispatch(first, last, false_type)调用
        // 用[first, last)元素初始化deque
        // input_iterator_tag版本
        template <class InputIterator>
        void range_initialize(InputIterator first, InputIterator last, STL::input_iterator_tag)
        {
            initialize_map(0);
            try {
                for (; first != last; ++first)
                    emplace_back(*first);
            } catch(...) {
                clear();
                throw;
            }
        }
        // forward_iterator_tag版本
        template <class ForwardIterator>
        void range_initialize(ForwardIterator first, ForwardIterator last, STL::forward_iterator_tag)
        {
            const size_type n = STL::distance(first, last);
            initialize_map(n);
            Map_pointer cur_node;
            try {
                for (cur_node = start.node; cur_node < finish.node; ++cur_node) {
                    ForwardIterator mid = first;
                    STL::advance(mid, buffer_size());
                    STL::uninitialized_copy(first, mid, *cur_node);
                    first = mid;
                }
                STL::uninitialized_copy(first, last, finish.first);
            } catch(...) {
                destroy_data(start, iterator(*cur_node, cur_node));
                throw;
            }
        }

        // 被deque(first, last)调用
        // first, last是integral的情况
        template <class Integer>
        void initialize_dispatch(Integer n, Integer x, std::true_type)
        {
            initialize_map(static_cast<size_type>(n));
            fill_initialize(x);
        }
        // first, last是iterator的情况
        template <class InputIterator>
        void initialize_dispatch(InputIterator first, InputIterator last, std::false_type)
        {
            using category = typename STL::iterator_traits<InputIterator>::iterator_category;
            range_initialize(first, last, category());
        }

        // 交换deque数据
        void swap_data(deque& x) noexcept 
        {
            STL::swap(start, x.start);
            STL::swap(finish, x.finish);
            STL::swap(map, x.map);
            STL::swap(map_size, x.map_size);
        }

        // 析构[first, last)的元素
        void destroy_data(iterator first, iterator last)
        {
            for (Map_pointer node = first.node + 1; node < last.node; ++node)
                STL::destroy(*node, *node + buffer_size());
            // first和last是否在同一个buffer
            if (first.node != last.node) {
                STL::destroy(first.cur, first.last);
                STL::destroy(last.first, last.cur);
            } else 
                STL::destroy(first.cur, last.cur);
        }

    public:
        // The big five
        
        /**
         *  @brief  constructor
         */ 
        deque()
        : map(), map_size(0), start(), finish()
        { initialize_map(0); }

        explicit deque(size_type n, const value_type& value = value_type())
        : map(), map_size(0), start(), finish()
        {
            initialize_map(n);    
            fill_initialize(value);
        }

        deque(std::initializer_list<value_type> l)
        : map(), map_size(0), start(), finish()
        { range_initialize(l.begin(), l.end(), STL::random_access_iterator_tag()); }

        template <class InputIterator>
        deque(InputIterator first, InputIterator last)
        : map(), map_size(0), start(), finish()
        {
            using is_integral = typename std::is_integral<InputIterator>::type;
            initialize_dispatch(first, last, is_integral());    
        }

        /**
         *  @brief  copy constructor
         */ 
        deque(const deque& x)
        : map(), map_size(0), start(), finish()
        {
            initialize_map(x.size());
            STL::uninitialized_copy(x.begin(), x.end(), start);
        }

        /**
         *  @brief  move constructor
         */ 
        deque(deque&& x)
        : map(), map_size(0), start(), finish()
        {
            initialize_map(0);
            if (x.map)
                swap_data(x);
        }

        /** 
         *  @brief  copy assignment operator 
         */ 
        deque& operator=(const deque& x)
        {
            if (&x != this) {
                const size_type len = size();
                if (len >= x.size())
                    erase_at_end(STL::copy(x.begin(), x.end(), start));
                else {
                    const_iterator mid = x.begin() + difference_type(len);
                    STL::copy(x.begin(), mid, start);
                    insert(finish, mid, x.end());
                }
            }
            return *this;
        }

        /**
         *  @brief  move assignment operator 
         */ 
        deque& operator=(deque&& x) 
        {
            deque tmp;
            swap_data(tmp);
            swap_data(x);
            return *this;
        }

        /**
         *  @brief  destructor
         */ 
        ~deque()
        { destroy_data(begin(), end()); }

    public:
        // 元素访问
        reference operator[](size_type n) noexcept 
        { return start[difference_type(n)]; }

        const_reference operator[](size_type n) const noexcept 
        { return start[difference_type(n)]; }
        
        reference at(size_type n)
        {
            if (n >= size())
                throw;
            return (*this)[n];
        }

        const_reference at(size_type n) const 
        {
            if (n >= size())
                throw;
            return (*this)[n];
        }

        reference front() noexcept { return *start; }
        
        const_reference front() const noexcept { return *start; }
        
        reference back() noexcept 
        {
            iterator tmp = finish;
            --tmp;
            return *tmp;
        }

        const_reference back() const noexcept 
        {
            const_iterator tmp = end();
            --tmp;
            return *tmp;
        }

    public:
        // 迭代器
        iterator begin() noexcept { return start; }
        const_iterator begin() const noexcept { return start; }
        const_iterator cbegin() const noexcept { return start; }
        iterator end() { return finish; }
        const_iterator end() const noexcept { return finish; }
        const_iterator cend() const noexcept { return finish; }

    public:
        // 容量
        bool empty() const noexcept { return finish == start; }
        size_type size() const noexcept { return finish - start; }
        size_type max_size() const noexcept { return data_allocator::max_size(); }

    protected:

        // 被erase(q1, q2), resize(), clear(), operator=调用
        // 移除pos开始的所有元素
        void erase_at_end(iterator pos)
        {
            // 将[pos, finish)所有元素析构
            destroy_data(pos, end());
            // 释放pos.node之后的所有buffer空间
            destroy_nodes(pos.node + 1, finish.node + 1);
            // pos成为新的finish
            finish = pos;
        }
        
        // 移除位于pos的元素
        iterator M_erase(iterator pos)
        {
            iterator next = pos;
            ++next;
            const difference_type elems_before = pos - start;  // pos前元素个数
            // pos之前的元素较少
            if (static_cast<size_type>(elems_before) < size() / 2) {
                // 后移pos之前的元素
                if (pos != start)
                    STL::copy_backward(start, pos, next);
                // 去除最前一个元素
                pop_front();
            }
            // pos之后的元素较少
            else {
                // 前移pos之后的元素
                if (next != finish)
                    STL::copy(next, finish, pos);
                // 去除最后一个元素
                pop_back();
            }
            return start + elems_before;
        }

        // 移除范围[first, last)的元素
        iterator M_erase(iterator first, iterator last)
        {
            if (first == last)
                return first;
            // 移除区间就是整个deque
            else if (first == start && last == finish) {
                clear();
                return finish;
            }
            else {
                const difference_type n = last - first;
                const difference_type elems_before = first - start;
                // 移除区间之前的元素较少
                if (static_cast<size_type>(elems_before) <= (size() - n) / 2) {
                    // 后移前方元素
                    if (first != start)
                        STL::copy_backward(start, first, last);
                    // erase_at_begin(start + n);
                    iterator new_start = start + n;
                    destroy_data(start, new_start); // 将前面多余元素析构
                    // 将多余buffer释放
                    destroy_nodes(start.node, new_start.node);
                    start = new_start;
                }
                // 移除区间之后的元素较少
                else {
                    if (last != finish)
                        STL::copy(last, finish, first);
                    erase_at_end(finish - n);
                }
                return start + elems_before;
            }
        }

    protected:

        // 在deque前端留出new_elems个元素的空间
        void new_elements_at_front(size_type new_elems)
        {
            if (max_size() - size() < new_elems)
                throw;
            // 预留map空间
            const size_type new_nodes = (new_elems + buffer_size() - 1) / buffer_size();
            reserve_map_at_front(new_nodes);
            size_type i;
            try {
                for (i = 1; i <= new_nodes; ++i)
                    *(start.node - i) = allocate_node();
            } catch(...) {
                for (size_type j = 1; j < i; ++j)
                    deallocate_node(*(start.node - j));
                throw;
            }
        }

        // 在deque尾端留出new_elems个元素的空间
        void new_elements_at_back(size_type new_elems)
        {
            if (max_size() - size() < new_elems)
                throw;
            // 预留map空间
            const size_type new_nodes = (new_elems + buffer_size() - 1) / buffer_size();
            reserve_map_at_back(new_nodes);
            size_type i;
            try {
                for (i = 1; i <= new_nodes; ++i)
                    *(finish.node + i) = allocate_node();
            } catch(...) {
                for (size_type j = 1; j < i; ++j)
                    deallocate_node(*(finish.node - j));
                throw;
            }
        }

        // 在start前预留n个元素的空间
        // 若第一个buffer头部剩余空间不够，则新增buffer
        iterator reserve_elements_at_front(size_type n)
        {
            const size_type vacancies = start.cur - start.first;
            if (n > vacancies)
                new_elements_at_front(n - vacancies);
            return start - difference_type(n);
        }
        
        // 在start前预留n个元素的空间
        // 若第一个buffer头部剩余空间不够，则新增buffer
        iterator reserve_elements_at_back(size_type n)
        {
            const size_type vacancies = (finish.last - finish.cur) - 1;
            if (n > vacancies)
                new_elements_at_back(n - vacancies);
            return finish + difference_type(n);
        }

        // 在pos前插入新元素，返回指向新插元素的迭代器
        template <class... Args>
        iterator insert_aux(iterator pos, Args&&... args)
        {
            value_type x_copy(std::forward<Args>(args)...);
            // iterator::operator-算出插入点之前的元素个数
            difference_type index = pos - start;
            if (static_cast<size_type>(index) < size() / 2) {   // 插入点前元素较少
                // 在deque最前端插入与第一个元素同值的元素
                push_front(std::move(front()));
                // 将pos前的元素前移
                iterator front1 = start;
                ++front1;
                iterator front2 = front1;
                ++front2;
                pos = start + index;
                iterator pos1 = pos;
                ++pos1;
                STL::copy(front2, pos1, front1);
            } else {
                // 在deque最尾端插入与最后一个元素同值的元素
                push_back(std::move(back()));
                // 将pos后的元素后移
                iterator back1 = finish;
                --back1;
                iterator back2 = back1;
                --back2;
                pos = start + index;
                STL::copy_backward(pos, back2, back1);
            }
            *pos = std::move(x_copy);
            return pos;
        }

        // 被fill_insert(pos, n, x)调用
        // 在pos前插入x的n个副本，插入点不在deque最前端或最尾端
        void insert_aux(iterator pos, size_type n, const value_type& x)
        {
            // 插入点之前的元素个数
            const difference_type elems_before = pos - start;
            const size_type length = size();
            value_type x_copy = x;
            // 插入点之前元素较少，前移这部分元素
            if (static_cast<size_type>(elems_before) < length / 2) {
                // 前移前在deque头留出空间
                iterator new_start = reserve_elements_at_front(n);
                iterator old_start = start;
                pos = start + elems_before;
                try {
                    // 插入点之前元素个数 大于等于 新增元素个数
                    if (elems_before >= difference_type(n)) {
                        iterator start_n = start + difference_type(n);
                        STL::uninitialized_copy(start, start_n, new_start);
                        start = new_start;
                        STL::copy(start_n, pos, old_start);
                        STL::fill(pos - difference_type(n), pos, x_copy);
                    } else {
                        STL::uninitialized_copy(start, pos, new_start);
                        STL::uninitialized_fill(new_start + elems_before, start, x_copy);
                        start = new_start;
                        STL::fill(old_start, pos, x_copy);
                    }
                } catch(...) {
                    destroy_nodes(new_start.node, start.node);
                    throw;
                }
            }
            // 插入点之后元素较少，后移这部分元素
            else {
                // 后移前在deque尾留出空间
                iterator new_finish = reserve_elements_at_back(n);
                iterator old_finish = finish;
                const difference_type elems_after = difference_type(length) - elems_before;
                pos = finish - elems_after;
                try {
                    // 插入点之后元素个数 大于等于 新增元素个数
                    if (elems_after >= difference_type(n)) {
                        iterator finish_n = finish - difference_type(n);
                        STL::uninitialized_copy(finish_n, finish, finish);
                        finish = new_finish;
                        STL::copy_backward(pos, finish_n, old_finish);
                        STL::fill(pos, pos + difference_type(n), x_copy);
                    } else {
                        STL::uninitialized_copy(pos, finish, pos + n);
                        STL::uninitialized_fill(finish, pos + n, x_copy);
                        finish = new_finish;
                        STL::fill(pos, old_finish, x_copy);
                    }
                } catch(...) {
                    destroy_nodes(finish.node + 1, new_finish.node + 1);
                    throw;
                }
            }
        }

        // 被range_insert_aux(pos, first, last, forward_iterator_tag)调用
        // 在pos前插入来自[first, last)的元素，插入点不在deque最前端或最尾端
        template <class ForwardIterator>
        void insert_aux(iterator pos, ForwardIterator first, ForwardIterator last, size_type n)
        {
            // 插入点之前的元素个数
            const difference_type elems_before = pos - start;
            const size_type length = size();
            // 插入点之前元素较少，前移这部分元素
            if (static_cast<size_type>(elems_before) < length / 2) {
                // 前移前在deque头留出空间
                iterator new_start = reserve_elements_at_front(n);
                iterator old_start = start;
                pos = start + elems_before;
                try {
                    // 插入点之前元素个数 大于等于 新增元素个数
                    if (elems_before >= difference_type(n)) {
                        iterator start_n = start + difference_type(n);
                        STL::uninitialized_copy(start, start_n, new_start);
                        start = new_start;
                        STL::copy(start_n, pos, old_start);
                        STL::copy(first, last, pos - difference_type(n));
                    } else {
                        ForwardIterator mid = first;
                        STL::advance(mid, difference_type(n) - elems_before);
                        STL::uninitialized_copy(start, pos, new_start);
                        STL::uninitialized_copy(first, mid, new_start + elems_before);
                        start = new_start;
                        STL::copy(mid, last, old_start);
                    }
                } catch(...) {
                    destroy_nodes(new_start.node, start.node);
                    throw;
                }
            }
            // 插入点之后元素较少，后移这部分元素
            else {
                // 后移前在deque尾留出空间
                iterator new_finish = reserve_elements_at_back(n);
                iterator old_finish = finish;
                const difference_type elems_after = difference_type(length) - elems_before;
                pos = finish - elems_after;
                try {
                    // 插入点之后元素个数 大于等于 新增元素个数
                    if (elems_after >= difference_type(n)) {
                        iterator finish_n = finish - difference_type(n);
                        STL::uninitialized_copy(finish_n, finish, finish);
                        finish = new_finish;
                        STL::copy_backward(pos, finish_n, old_finish);
                        STL::copy(first, last, pos);
                    } else {
                        ForwardIterator mid = first;
                        STL::advance(mid, elems_after);
                        STL::uninitialized_copy(pos, finish, new_finish - elems_after);
                        STL::uninitialized_copy(mid, last, finish);
                        finish = new_finish;
                        STL::copy(first, mid, pos);
                    }
                } catch(...) {
                    destroy_nodes(finish.node + 1, new_finish.node + 1);
                    throw;
                }
            }
        }
        
        // 在pos前插入x的n个副本
        void fill_insert(iterator pos, size_type n, const value_type& x)
        {
            // 若插入点是deque最前端
            if (pos.cur == start.cur) {
                iterator new_start = reserve_elements_at_front(n);
                try {
                    STL::uninitialized_fill(new_start, start, x);
                    start = new_start;
                } catch(...) {
                    destroy_nodes(new_start.node, start.node);
                    throw;
                }
            }
            // 若插入点是deque最尾端
            else if (pos.cur == finish.cur) {
                iterator new_finish = reserve_elements_at_back(n);
                try {
                    STL::uninitialized_fill(finish, new_finish, x);
                    finish = new_finish;
                } catch(...) {
                    destroy_nodes(finish.node + 1, new_finish.node + 1);
                    throw;
                }
            }
            else    // 交给insert_aux(pos, n, x) 
                insert_aux(pos, n, x);
        } 

        // 被insert_dispatch(pos, first, last, false_type)调用
        // 在pos前插入来自范围[first, last)的元素
        // input_iterator_tag版本
        template <class InputIterator>
        void range_insert_aux(iterator pos, InputIterator first, InputIterator last, STL::input_iterator_tag)
        {
            for (; first != last; ++first)
                emplace(pos, *first);
        }
        // forward_iterator_tag版本
        template <class ForwardIterator>
        void range_insert_aux(iterator pos, ForwardIterator first, ForwardIterator last, STL::forward_iterator_tag)
        {
            const size_type n = STL::distance(first, last);
            if (pos.cur == start.cur) {
                iterator new_start = reserve_elements_at_front(n);
                try {
                    STL::uninitialized_copy(first, last, new_start);
                    start = new_start;
                } catch(...) {
                    destroy_nodes(new_start.node, start.node);
                    throw;
                }
            }
            else if (pos.cur == finish.cur) {
                iterator new_finish = reserve_elements_at_back(n);
                try {
                    STL::uninitialized_copy(first, last, finish);
                    finish = new_finish;
                } catch(...) {
                    destroy_nodes(finish.node + 1, new_finish.node + 1);
                    throw;
                }
            }
            else 
                insert_aux(pos, first, last, n);
        }
        
        // 被insert(cpos, first, last)调用
        // first, last是integral的情况
        template <class Integer>
        void insert_dispatch(iterator pos, Integer n, Integer x, std::true_type)
        { fill_insert(pos, n, x); }
        // first, last是iterator的情况
        template <class InputIterator>
        void insert_dispatch(iterator pos, InputIterator first, InputIterator last, std::false_type)
        {
            using category = typename STL::iterator_traits<InputIterator>::iterator_category;
            range_insert_aux(pos, first, last, category());
        }

    protected:

        // 重新调整map的空间分配，可能会更换map
        void reallocate_map(size_type nodes_to_add, bool add_at_front)
        {
            const size_type old_num_nodes = finish.node - start.node + 1;
            const size_type new_num_nodes = old_num_nodes + nodes_to_add;
            
            Map_pointer new_nstart;
            // map_size足够大
            if (map_size > 2 * new_num_nodes) {
                // 在map前端或尾端多留出nodes_to_add个空间
                new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
                // 将map内元素后移/前移
                if (new_nstart < start.node)
                    STL::copy(start.node, finish.node + 1, new_nstart);
                else 
                    STL::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
            }
            // map_size不够，需要重新分配更大的空间给map
            else {
                size_type new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;
                Map_pointer new_map = allocate_map(new_map_size);
                new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
                // 拷贝原map
                STL::copy(start.node, finish.node + 1, new_nstart);
                // 释放原map
                deallocate_map(map, map_size);
                // 设置新map起始地址与大小
                map = new_map;
                map_size = new_map_size;
            }
            // 重设start和finish迭代器
            start.set_node(new_nstart);
            finish.set_node(new_nstart + old_num_nodes - 1);
        }

        // 根据deque的map尾部剩余空间重新分配map 
        void reserve_map_at_back(size_type nodes_to_add = 1)
        {
            // 若map尾部无备用空间
            if (nodes_to_add + 1 > map_size - (finish.node - map))
                reallocate_map(nodes_to_add, false);
        }

        // 根据deque的map前端剩余空间重新分配map 
        void reserve_map_at_front(size_type nodes_to_add = 1)
        {
            // 若map前端无备用空间
            if (nodes_to_add > size_type(start.node - map))
                reallocate_map(nodes_to_add, true);
        }

        // 仅当finish.cur == finish.last - 1时调用
        // 即最后一个buffer尾端只有1个备用空间
        template <class... Args>
        void push_back_aux(Args&&... args)
        {
            // 若符合条件则修改map
            reserve_map_at_back();
            // 配置一个新buffer至deque尾部
            *(finish.node + 1) = allocate_node();
            try {
                // 在倒数第二个buffer的最后位置构造新增值
                STL::construct(finish.cur, std::forward<Args>(args)...);
                // 将finish指向最后一个buffer的第一个元素前
                finish.set_node(finish.node + 1);
                finish.cur = finish.first;
            } catch(...) {
                deallocate_node(*(finish.node + 1));
                throw;
            }
        }

        // 仅当start.cur == start.first时调用
        // 即第一个buffer前端没有备用空间
        template <class... Args>
        void push_front_aux(Args&&... args)
        {
            // 若符合条件则修改map
            reserve_map_at_front();
            // 配置一个新buffer至deque开头
            *(start.node - 1) = allocate_node();
            try {
                // 将start指向最后一个buffer的最后一个元素前
                start.set_node(start.node - 1);
                start.cur = start.last - 1;
                // 在第一个buffer的最后位置构造新增值
                STL::construct(start.cur, std::forward<Args>(args)...);
            } catch(...) {
                ++start;
                deallocate_node(*(start.node - 1));
                throw;
            }
        }

    public:
        // 修改器
        
        /** 
         *  @brief  移除deque所有元素
         *
         *  最终保留start所在buffer，但其内容已为空
         */ 
        void clear() noexcept 
        { erase_at_end(begin()); }

        /**
         *  @brief  在pos前插入x
         *  @return  被插入x的迭代器
         */ 
        iterator insert(const_iterator pos, const value_type& x)
        {
            // 若插入点是deque最前端
            if (pos.cur == start.cur) {
                push_front(x);
                return start;
            }
            // 若插入点是deque最尾端
            else if (pos.cur == finish.cur) {
                push_back(x);
                iterator tmp = finish;
                --tmp;
                return tmp;
            }
            // 交给insert_aux(pos, x)
            else 
                return insert_aux(pos.M_const_cast(), x);
        }

        iterator insert(const_iterator pos, value_type&& x)
        { return emplace(pos, std::move(x)); }

        /**
         *  @brief  在pos前插入x的n个副本
         *  @return  指向首个被插入元素的迭代器
         */ 
        iterator insert(const_iterator pos, size_type n, const value_type& x)
        {
            difference_type offset = pos - cbegin();
            fill_insert(pos.M_const_cast(), n, x);
            return begin() + offset;
        }

        /**
         *  @brief  在pos前插入来自范围[first, last)的元素
         *  @return  指向首个被插入元素的迭代器
         */ 
        template <class InputIterator>
        iterator insert(const_iterator pos, InputIterator first, InputIterator last)
        {
            difference_type offset = pos - cbegin();
            using is_integral = typename std::is_integral<InputIterator>::type;
            insert_dispatch(pos.M_const_cast(), first, last, is_integral());
            return begin() + offset;
        }

        /**
         *  @brief  在pos前插入来自initializer_list的元素
         *  @return  指向首个被插入元素的迭代器
         */
        iterator insert(const_iterator pos, std::initializer_list<value_type> l)
        {
            difference_type offset = pos - cbegin();
            range_insert_aux(pos.M_const_cast(), l.begin(), l.end(), STL::random_access_iterator_tag());
            return begin() + offset;
        }

        /**
         *  @brief  在pos前构造新元素
         *  @return  指向新增元素
         */ 
        template <class... Args>
        iterator emplace(const_iterator pos, Args&&... args)
        {
            // 若插入点是deque最前端
            if (pos.cur == start.cur) {
                emplace_front(std::forward<Args>(args)...);
                return start;
            }
            // 若插入点是deque最尾端
            else if (pos.cur == finish.cur) {
                emplace_back(std::forward<Args>(args)...);
                iterator tmp = finish;
                --tmp;
                return tmp;
            }
            // 交给insert_aux()
            else 
                return insert_aux(pos.M_const_cast(), std::forward<Args>(args)...);
        }

        /**
         *  @brief  移除位于pos的元素
         *  @return  指向下一个元素的迭代器
         */ 
        iterator erase(const_iterator pos)
        { return M_erase(pos.M_const_cast()); }

        /**
         *  @brief  移除范围[first, last)中的元素
         *  @return  指向原来last所指元素的迭代器
         */ 
        iterator erase(const_iterator first, const_iterator last)
        { return M_erase(first.M_const_cast(), last.M_const_cast()); }

        /**
         *  @brief  将x添加至deque尾部
         */ 
        void push_back(const value_type& x)
        {
            // 最后一个buffer尾端有大于1个备用空间
            if (finish.cur != finish.last - 1) {
                STL::construct(finish.cur, x);
                ++finish.cur;
            } else  // 最后一个buffer尾端只有1个备用空间
                push_back_aux(x);
        }

        /**
         *  @brief  将x移动至deque尾部
         */ 
        void push_back(value_type&& x)
        { emplace_back(std::move(x)); }

        /**
         *  @brief  在deque尾部构造新元素
         */ 
        template <class... Args>
        void emplace_back(Args&&... args)
        {
            if (finish.cur != finish.last - 1) {
                STL::construct(finish.cur, std::forward<Args>(args)...);
                ++finish.cur;
            } else 
                push_back_aux(std::forward<Args>(args)...);
        }

        /**
         *  @brief  移除deque最后一位元素
         */ 
        void pop_back() noexcept 
        {
            // 最后一个buffer有大于等于1个元素
            if (finish.cur != finish.first) {
                --finish.cur;
                STL::destroy(finish.cur);
            }
            // 最后一个buffer没有元素
            else {
                deallocate_node(finish.first);  // 释放最后一个buffer
                // 使finish指向上一个buffer的最后一个元素
                finish.set_node(finish.node - 1);
                finish.cur = finish.last - 1;
                // 将该元素析构
                STL::destroy(finish.cur);
            }
        }

        /**
         *  @brief  将x添加至deque头部
         */ 
        void push_front(const value_type& x)
        {
            // 第一个buffer前端有备用空间
            if (start.cur != start.first) {
                STL::construct(start.cur - 1, x);
                --start.cur;
            } else  // 第一个buffer前端没有备用空间
                push_front_aux(x);
        }

        /**
         *  @brief  将x移动至deque头部
         */ 
        void push_front(value_type&& x)
        { emplace_front(std::move(x)); }

        /**
         *  @brief  在deque头部构造新元素
         */ 
        template <class... Args>
        void emplace_front(Args&&... args)
        {
            if (start.cur != start.first) {
                STL::construct(start.cur - 1, std::forward<Args>(args)...);
                --start.cur;
            } else 
                push_front_aux(std::forward<Args>(args)...);
        }
        
        /**
         *  @brief  移除deque第一位元素
         */ 
        void pop_front() noexcept 
        {
            // 第一个buffer有大于等于2个元素
            if (start.cur != start.last - 1) {
                STL::destroy(start.cur);
                ++start.cur;
            }
            // 第一个buffer仅有1个元素
            else {
                STL::destroy(start.cur);    // 析构这唯一的元素
                deallocate_node(start.first);  // 释放第一个buffer
                // 使start指向下一个buffer的第一个元素
                start.set_node(start.node + 1);
                start.cur = finish.first;
            }
        }

        /**
         *  @brief  重设deque大小以容纳new_size个元素
         *
         *  若当前大小小于new_size，则后附额外值为x的元素
         */ 
        void resize(size_type new_size)
        {
            const size_type len = size();
            if (new_size > len)
                insert(finish, new_size - len, value_type());
            else if (new_size < len)
                erase_at_end(start + difference_type(new_size));
        }

        void resize(size_type new_size, const value_type& x)
        {
            const size_type len = size();
            if (new_size > len)
                insert(finish, new_size - len, x);
            else if (new_size < len)
                erase_at_end(start + difference_type(new_size));
        }

        /**
         *  @brief  与deque x交换内容
         */ 
        void swap(deque& x)
        { swap_data(x); }

    };

    template <class T, class Alloc>
    inline bool operator==(const deque<T, Alloc>& x, const deque<T, Alloc>& y)
    { return x.size() == y.size() && STL::equal(x.begin(), x.end(), y.begin()); }

    template <class T, class Alloc>
    inline bool operator!=(const deque<T, Alloc>& x, const deque<T, Alloc>& y)
    { return !(x == y); }

} /* namespace end */

#endif
