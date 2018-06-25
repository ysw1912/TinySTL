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
        // 为节点分配buffer空间
        pointer allocate_node()
        { return data_allocator::allocate(buffer_size()); }
        // 将节点所指buffer释放
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


        // 用value初始化[start, finish)的元素
        void fill_initialize(const value_type& value)
        {
            Map_pointer cur;
            try {
                for (cur = start.node; cur < finish.node; ++cur)
                    STL::uninitialized_fill(*cur, *cur + buffer_size(), value);
                // 最后一个buffer单独设初值
                STL::uninitialized_fill(finish.first, finish.cur, value);
            } catch(...) {
                STL::destroy(start, iterator(*cur, cur));
                throw;
            }
        }

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
                STL::destroy(start, iterator(*cur_node, cur_node));
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
         *  @brief  destructor
         */ 
        ~deque()
        { destroy_data(begin(), end()); }

    public:
        // 元素访问
        reference operator[](size_type n)
        { return start[difference_type(n)]; }
        reference front() { return *start; }
        reference back()
        {
            iterator tmp = finish;
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
        void erase_at_end(iterator pos)
        {
            destroy_data(pos, end());
            destroy_nodes(pos.node + 1, finish.node + 1);
            finish = pos;
        }
        
        // 重新调整map的空间分配
        void reallocate_map(size_type nodes_to_add, bool add_at_front)
        {
            const size_type old_num_nodes = finish.node + 1 - start.node;
            const size_type new_num_nodes = old_num_nodes + nodes_to_add;
            Map_pointer new_nstart;
            if (map_size > 2 * new_num_nodes) { // map_size足够
                new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
                if (new_nstart < start.node)
                    STL::copy(start.node, finish.node + 1, new_nstart);
                else 
                    STL::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
            } else {    // 重新分配更大的map
                size_type new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;
                Map_pointer new_map = allocate_map(new_map_size);
                new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
                STL::copy(start.node, finish.node + 1, new_nstart);
                deallocate_map(map, map_size);
                map = new_map;
                map_size = new_map_size;
            }
            start.set_node(new_nstart);
            finish.set_node(new_nstart + old_num_nodes - 1);
        }

        // 
        void reserve_map_at_back(size_type nodes_to_add = 1)
        {
            if (nodes_to_add + 1 > map_size - (finish.node - map))
                reallocate_map(nodes_to_add, false);
        }

        // 仅当finish.cur == finish.last - 1时调用
        template <class... Args>
        void push_back_aux(Args&&... args)
        {
            reserve_map_at_back();
            *(finish.node + 1) = allocate_node();
            try {
                STL::construct(finish.cur, std::forward<Args>(args)...);
                finish.set_node(finish.node + 1);
                finish.cur = finish.first;
            } catch(...) {
                deallocate_node(*(finish.node + 1));
                throw;
            }
        }

    public:
        // 修改器
        
        /** 
         *  @brief  移除deque所有元素
         */ 
        void clear() noexcept 
        { erase_at_end(begin()); }

        /**
         *  @brief  在deque尾构造新元素
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


    };
} /* namespace end */

#endif
