#ifndef TINYSTL_LIST_H_
#define TINYSTL_LIST_H_ 

#include <initializer_list>
#include <type_traits>
#include <utility>

#include "algo.h"
#include "allocator.h"
#include "iterator.h"

namespace STL
{    
    // list的节点
    template <class T>
    struct list_node
    {
        list_node<T>* prev;
        list_node<T>* next;
        T data;

        template <class... Args>
        list_node(Args&&... args) : data(std::forward<Args>(args)...) { }
    };

    /**
     *  @brief  list::iterator
     *
     *  所有函数是操作符重载
     */ 
    template <class T>
    struct list_iterator : public STL::iterator<STL::bidirectional_iterator_tag, T>
    {
        using Self      = list_iterator<T>;
        using Node      = list_node<T>;

        using reference = typename STL::iterator<STL::bidirectional_iterator_tag, T>::reference;
        using pointer   = typename STL::iterator<STL::bidirectional_iterator_tag, T>::pointer;

        // 唯一成员，指向list的节点
        Node* node;

        // constructor
        list_iterator() : node() {}
        list_iterator(Node* x) : node(x) {}
        list_iterator(const list_iterator<T>& x) : node(x.node) {}

        Self M_const_cast() const { return *this; }

        // 对迭代器取值，取的是节点的数据值
        reference operator*() const { return node->data; }
        // 对迭代器的member access运算符
        pointer operator->() const { return &(node->data); }

        // 迭代器+1，前进一个节点
        Self& operator++()
        {
            node = node->next;
            return *this;
        }
        Self operator++(int)
        {
            Self tmp = *this;
            node = node->next;
            return tmp;
        }

        // 迭代器-1，后退一个节点
        Self& operator--()
        {
            node = node->prev;
            return *this;
        }
        Self operator--(int)
        {
            Self tmp = *this;
            node = node->prev;
            return tmp;
        }

        bool operator==(const Self& x) const { return node == x.node; }
        bool operator!=(const Self& x) const { return node != x.node; }
    };

    /**
     *  @brief  list::const_iterator
     *
     *  所有函数是操作符重载
     */ 
    template <class T>
    struct list_const_iterator : public STL::iterator<STL::bidirectional_iterator_tag, const T> 
    {
        using Base      = STL::iterator<STL::bidirectional_iterator_tag, const T>;
        using Self      = list_const_iterator<T>;
        using Node      = const list_node<T>;
        using iterator  = list_iterator<T>;
 
        using reference = typename Base::reference;
        using pointer   = typename Base::pointer;

        // 唯一成员，指向list的节点
        Node* node;

        // constructor
        list_const_iterator() : node() {}
        list_const_iterator(const Node* x) : node(x) {}
        list_const_iterator(const iterator& x) : node(x.node) {}

        iterator M_const_cast() const { return iterator(const_cast<list_node<T>*>(node)); }

        // 对迭代器取值，取的是节点的数据值
        reference operator*() { return node->data; }
        // 对迭代器的member access运算符
        pointer operator->() const { return &(static_cast<Node*>(node)->data); }

        // 迭代器+1，前进一个节点
        Self& operator++()
        {
            node = node->next;
            return *this;
        }
        Self operator++(int)
        {
            Self tmp = *this;
            node = node->next;
            return tmp;
        }

        // 迭代器-1，后退一个节点
        Self& operator--()
        {
            node = node->prev;
            return *this;
        }
        Self operator--(int)
        {
            Self tmp = *this;
            node = node->prev;
            return tmp;
        }

        bool operator==(const Self& x) const { return node == x.node; }
        bool operator!=(const Self& x) const { return node != x.node; }
    };

    template <class T, class Alloc = STL::pool_alloc>
    class list
    {
    public:
        using value_type                = T;
        using pointer                   = T*;
        using const_pointer             = const T*;
        using reference                 = T&;
        using const_reference           = const T&;
        using iterator                  = list_iterator<T>;
        using const_iterator            = list_const_iterator<T>;
        using size_type                 = size_t;
        using difference_type           = ptrdiff_t;

    protected:
        using Node                      = list_node<T>;
        using list_node_allocator       = STL::allocator<Node, Alloc>;

        Node* node;     // 用一个指针(指向end)，表示整个环状双向链表

    protected:
        // 配置一个节点并传回
        Node* get_node() { return list_node_allocator::allocate(); }

        // 释放一个节点
        void put_node(Node* p) { list_node_allocator::deallocate(p); } 
        
        // node头尾指向自己
        void init_node()
        {
            node->next = node;
            node->prev = node;
        }

        // 初始化空list
        void empty_init()
        {
            node = get_node();
            init_node();
        }

        // 为新节点分配空间，在上面构造args的拷贝
        template <class... Args>
        Node* create_node(Args&&... args)
        {
            Node* p = get_node();
            try {
                STL::construct(p, std::forward<Args>(args)...);
            } catch(...) {
                put_node(p);
                throw;
            }
            return p;
        }
        
        // 被范围ctor调用
        // first, last是iterator的情况
        template <class InputIterator>
        void initialize_dispatch(InputIterator first, InputIterator last, std::false_type)
        {
            for ( ; first != last; ++first)
                emplace_back(*first);
        }
        // first, last是integral的情况
        template <class Integer>
        void initialize_dispatch(Integer n, Integer x, std::true_type)
        {
            fill_initialize(static_cast<size_type>(n), x);
        }

        // 被list(n, x)调用，或者range constructor产生的歧义
        void fill_initialize(size_type n, const value_type& x)
        {
            for ( ; n; --n)
                push_back(x);
        }
 
        // 析构一个节点并释放空间
        void drop_node(Node* p)
        {
            STL::destroy(&p->data);
            put_node(p);
        }
        
        // 清除list所有节点
        void clear_nodes()
        {
            Node* cur = node->next; // begin()
            while (cur != node) {
                Node* tmp = cur;
                cur = cur->next;
                drop_node(tmp);
            }
        }
    
    public:
        // The Big Five

        /**
         *  @brief  constructor
         */ 
        list() { empty_init(); }

        explicit list(size_type n, const value_type& x = value_type())
        {
            empty_init();
            fill_initialize(n, x);
        }

        template <class InputIterator>
        list(InputIterator first, InputIterator last)
        {
            empty_init();
            // 区分参数类型是iterator还是integral
            using is_integral = typename std::is_integral<InputIterator>::type;
            initialize_dispatch(first, last, is_integral());
        } 

        list(std::initializer_list<value_type> l)
        { 
            empty_init();
            initialize_dispatch(l.begin(), l.end(), std::false_type());
        }

        /**
         *  @brief  copy constructor
         */ 
        list(const list& l)
        {
            empty_init();
            initialize_dispatch(l.begin(), l.end(), std::false_type());
        }

        /**
         *  @brief  move constructor
         */ 
        list(list&& x) noexcept
        {
            Node* const xnode = x.node;
            if (xnode->next == xnode) {
                empty_init();
            } else {
                node = get_node();
                Node* const node_copy = node;
                node_copy->next = xnode->next;
                node_copy->prev = xnode->prev;
                node_copy->next->prev = node_copy->prev->next = node_copy;
                x.empty_init();
            }
        }

        /**
         *  @brief  copy assignment
         */
        list& operator=(const list& x)
        {
            if (this != &x) {
                iterator first = begin(), last = end();
                const_iterator firstx = x.begin(), lastx = x.end();
                for ( ; first != last && firstx != lastx; ++first, ++firstx)
                    *first = *firstx;
                if (firstx == lastx)
                    erase(first, last);
                else 
                    insert(last, firstx, lastx);
            }
            return *this;
        }

        /**
         *  @brief  move assignment
         */
        list& operator=(list&& x)
        {
            clear();
            swap(x);
            return *this;
        }

        /**
         *  @brief  destructor
         *
         *  若data是指针，不能消除所指对象
         */ 
        ~list()
        {
            clear_nodes();
            STL::destroy(node);
        }

    public:
        // 元素访问
        reference front() { return *begin(); }
        const_reference front() const { return *begin(); }
        reference back() { return *iterator(--end()); }
        const_reference back() const { return *const_iterator(--end()); }

    public:
        // 迭代器
        iterator begin() noexcept { return iterator(node->next); }
        const_iterator begin() const noexcept { return const_iterator(node->next); }
        const_iterator cbegin() const noexcept { return const_iterator(node->next); }
        iterator end() noexcept { return iterator(node); }
        const_iterator end() const noexcept { return const_iterator(node); }
        const_iterator cend() const noexcept { return const_iterator(node); }

    public:
        // 容量 
        bool empty() const noexcept { return node->next == node; }
        size_type size() const noexcept { return STL::distance(begin(), end()); }
        size_type max_size() const noexcept { return list_node_allocator::max_size(); } 

    protected:
        // 在pos位置插入新节点x
        template <class... Args>
        iterator M_insert(iterator pos, Args&&... args)
        {
            Node* tmp = create_node(std::forward<Args>(args)...);
            tmp->next = pos.node;
            tmp->prev = (pos.node)->prev;
            (pos.node->prev)->next = tmp;
            pos.node->prev = tmp;
            return tmp;
        }

    public:
        // 修改器 
       
        /**
         *  @brief  移除list所有节点
         */ 
        void clear() noexcept
        {
            clear_nodes();
            // 恢复原始状态
            init_node();
        }

        /**
         *  @brief  在pos前插入值为x的节点
         *  @return  指向被插入节点的迭代器
         */
        iterator insert(const_iterator pos, const value_type& x)
        { return M_insert(pos.M_const_cast(), x); }

        /**
         *  @brief  在pos前插入n个值为x的节点
         *  @return  指向首个被插入节点的迭代器，若n==0则为pos
         */ 
        iterator insert(const_iterator pos, size_type n, const value_type& x)
        {
            if (n) {
                list tmp(n, x);
                iterator it = tmp.begin();
                splice(pos, tmp);
                return it;
            }
            return pos.M_const_cast();
        }

        /**
         *  @brief  在pos前插入来自范围[first, last)的节点
         *  @return  指向首个被插入节点的迭代器，若first==last则为pos
         */
        template <class InputIterator>
        iterator insert(const_iterator pos, InputIterator first, InputIterator last)
        {
            list tmp(first, last);
            if (!tmp.empty()) {
                iterator it = tmp.begin();
                splice(pos, tmp);
                return it;
            }
            return pos.M_const_cast();
        }

        /**
         *  @brief  移除迭代器pos所指节点
         */ 
        iterator erase(const_iterator pos)
        {
            Node* next_node = pos.node->next;
            Node* prev_node = pos.node->prev;
            prev_node->next = next_node;
            next_node->prev = prev_node;
            drop_node(pos.M_const_cast().node);
            return iterator(next_node);
        }

        /**
         *  @brief  移除范围[first, last)中的节点
         */ 
        iterator erase(const_iterator first, const_iterator last)
        {
            while (first != last)
                first = erase(first);
            return last.M_const_cast();
        }

        /**
         *  @brief  在list尾部插入元素x
         */ 
        void push_back(const value_type& x) { M_insert(end(), x); }

        /**
         *  @brief  在list尾部移入元素x
         */ 
        void push_back(value_type&& x) { M_insert(end(), std::move(x)); }

        /**
         *  @brief  在list尾部用args构造节点元素
         */ 
        template <class... Args>
        void emplace_back(Args&&... args) { M_insert(end(), std::forward<Args>(args)...); }

        /**
         *  @brief  移除尾节点
         */ 
        void pop_back() { erase(--end()); }

        /**
         *  @brief  在list头部插入元素x
         */ 
        void push_front(const value_type& x) { M_insert(begin(), x); }
    
        /**
         *  @brief  在list头部移入元素x
         */ 
        void push_front(value_type&& x) { M_insert(begin(), std::move(x)); }

        /**
         *  @brief  在list头部用args构造节点元素
         */ 
        template <class... Args>
        void emplace_front(Args&&... args) { M_insert(begin(), std::forward<Args>(args)...); }

        /**
         *  @brief  移除头节点
         */ 
        void pop_front() { erase(begin()); }

        /**
         *  @brief 
         */ 
        void resize(size_type new_size)
        { resize(new_size, value_type()); }

        void resize(size_type new_size, const value_type& x)
        {
            iterator i = begin();
            size_type len = 0;
            for ( ; i != end() && len < new_size; ++i, ++len);
            if (len == new_size)
                erase(i, end());
            else    // i == end()
                insert(end(), new_size - len, x);
        }

        /** 
         *  @brief  与链表x交换数据
         *
         *  全局函数STL::swap(list1, list2)特化为本函数
         */ 
        void swap(list& x)
        {
            STL::swap(node, x.node);
        }

    protected:
        // 将[first, last)内所有节点移动到pos指向的节点之前
        void transfer(iterator pos, iterator first, iterator last)
        {
            if (pos != last) {
                last.node->prev->next = pos.node;   // last前一个节点 后指向 pos 
                first.node->prev->next = last.node; // first前一个节点 后指向 last 
                pos.node->prev->next = first.node;  // pos前一个节点 后指向 first 
                Node* tmp = pos.node->prev;         // tmp = pos前一个节点
                pos.node->prev = last.node->prev;   // pos 前指向 last前一个节点
                last.node->prev = first.node->prev; // last 前指向 first前一个节点
                first.node->prev = tmp;             // first 前指向 pos原来的前一个节点
            }
        }

    public: 
        // 操作 
        
        /**
         *  @brief  将链表x合并到*this 
         *
         *  两个lists必须已经升序排序
         */
        void merge(list&& x)
        {
            if (this != &x) {
                iterator first = begin(), last = end();
                iterator firstx = x.begin(), lastx = x.end();
                while (first != last && firstx != lastx) {
                    if (*firstx < *first) {
                        iterator next = firstx;
                        transfer(first, firstx, ++next);
                        firstx = next;
                    } else {
                        ++first;
                    }
                }
                if (firstx != lastx) {
                    transfer(last, firstx, lastx);
                }
            }
        }

        void merge(list& x) { merge(std::move(x)); }
        
        // 自定义比较运算符
        template <class Compare>
        void merge(list&& x, Compare cmp)
        {
            if (this != &x) {
                iterator first = begin(), last = end();
                iterator firstx = x.begin(), lastx = x.end();
                while (first != last && firstx != lastx) {
                    if (cmp(*firstx, *first)) {
                        iterator next = firstx;
                        transfer(first, firstx, ++next);
                        firstx = next;
                    } else {
                        ++first;
                    }
                }
                if (firstx != lastx) {
                    transfer(last, firstx, lastx);
                }
            }
        }
        
        template <class Compare>
        void merge(list& x, Compare cmp) { merge(std::move(x), cmp); }

        /**
         *  @brief  将链表x接合于pos所指位置之前
         *
         *  x变为空链表，需要this != x
         */
        void splice(const_iterator pos, list&& x) noexcept
        {
            if (!x.empty())
                transfer(pos.M_const_cast(), x.begin(), x.end());
        }

        void splice(const_iterator pos, list& x) noexcept { splice(pos, std::move(x)); }

        /**
         *  @brief  将i所指元素接合于pos之前
         *
         *  pos和i可指向同一个list
         */
        void splice(const_iterator pos, list&&, const_iterator i) noexcept
        {
            iterator j = i.M_const_cast();
            ++j;
            if (pos == i || pos == j)   return;
            transfer(pos.M_const_cast(), i.M_const_cast(), j);
        }

        void splice(const_iterator pos, list& x, const_iterator i) noexcept { splice(pos, std::move(x), i); }

        /**
         *  @brief  将[first, last)内所有元素接合于pos之前
         *
         *  pos和[first, last)可指向同一个list，若pos在[first, last)之内则未定义
         */ 
        void splice(const_iterator pos, list&&, const_iterator first, const_iterator last) noexcept
        {
            if (first != last)
                transfer(pos.M_const_cast(), first.M_const_cast(), last.M_const_cast());
        }

        void splice(const_iterator pos, list& x, const_iterator first, const_iterator last) noexcept { splice(pos, std::move(x), first, last); }

        /*
         *  @brief  移除值为x的所有节点
         *
         *  用operator==比较元素与x
         */ 
        void remove(const value_type& x)
        {
            for (iterator first = begin(); first != end(); ) {
                iterator next = first;
                ++next;
                if (*first == x) erase(first);
                first = next;
            }
        }

        /**
         *  @brief  移除所有一元谓词p对它返回true的元素
         *
         *  e.g. list.remove_if([](value_type n){ return cmp(n, constant); });
         */ 
        template <class UnaryPredicate>
        void remove_if(UnaryPredicate p)
        {
            for (iterator first = begin(); first != end(); ) {
                iterator next = first;
                ++next;
                if (p(*first)) erase(first);
                first = next;
            }
        }

        /**
         *  @brief  将list逆序
         */ 
        void reverse() noexcept
        {
            // 空链表或仅有一个元素，直接返回
            if (node->next == node || node->next->next == node) return;
            iterator first = begin();
            ++first;
            while (first != end()) {
                iterator old = first;
                ++first;
                transfer(begin(), old, first);
            }
        }

        /**
         *  @brief  移除值相同的连续元素
         *
         *  用operator==比较元素 
         */ 
        void unique()
        {
            iterator first = begin();
            iterator last = end();
            if (first == last)  return; // 空链表
            for (iterator next = first; ++next != last; next = first) {
                if (*first == *next)
                    erase(next);
                else 
                    first = next;
            }
        }

        // 用二元谓词p比较元素
        template <class BinaryPredicate>
        void unique(BinaryPredicate p)
        {
            iterator first = begin();
            iterator last = end();
            if (first == last)  return; // 空链表
            for (iterator next = first; ++next != last; next = first) {
                if (p(*first, *next))
                    erase(next);
                else 
                    first = next;
            }
        }

        /**
         *  @brief  list排序，merge sort 
         *
         *  https://ysw1912.github.io/post/cc++/stl02/
         */ 
        void sort()
        {
            // 空链表或仅有一个元素，直接返回
            if (node->next == node || node->next->next == node) return;
            // 一些新的list作为数据缓冲区
            list carry;         // 每次取原list的头节点
            list bucket[64];    // 64个桶，bucket[i]的大小为2^i
            int fill = 0;       // 有元素的桶的最大index + 1
            while (!empty()) {
                carry.splice(carry.begin(), *this, begin());    // 取原list的头节点
                int i = 0;
                // index < fill的桶未满时，不断向桶内搬运节点，桶内有序
                // index < fill的桶全满时(i能==fill)，则将这些桶归并到carry，再存放到bucket[fill]
                while (i < fill && !bucket[i].empty()) {
                    carry.merge(bucket[i]);
                    ++i;
                }
                bucket[i].swap(carry);
                // bucket[fill]已满，增加下一个桶
                // 此时fill之前的桶可能未满
                if (i == fill)  ++fill;
            }
            // 将所有桶归并
            for (int i = 1; i < fill; ++i) {
                bucket[i].merge(bucket[i - 1]);
            }
            swap(bucket[fill - 1]);
        }

        // 自定义比较运算符
        template <class Compare>
        void sort(Compare cmp) {
            if (node->next == node || node->next->next == node) return;
            list carry;    
            list tmp[64];  
            int fill = 0;  
            while (!empty()) {
                carry.splice(carry.begin(), *this, begin());
                int i = 0;
                while (i < fill && !tmp[i].empty()) {
                    carry.merge(tmp[i], cmp);
                    ++i;
                }
                tmp[i].swap(carry);
                if (i == fill)  ++fill;
            }
            for (int i = 1; i < fill; ++i) {
                tmp[i].merge(tmp[i - 1], cmp);
            }
            swap(tmp[fill - 1]);
        }

    };

    template <class T, class Alloc>
    inline bool operator==(const list<T, Alloc>& x, const list<T, Alloc>& y)
    {
        using const_iterator = typename list<T, Alloc>::const_iterator;
        const_iterator end1 = x.end(), end2 = y.end();
        const_iterator i1 = x.begin(), i2 = y.begin();
        while (i1 != end1 && i2 != end2 && *i1 == *i2) {
            ++i1;
            ++i2;
        }
        return i1 == end1 && i2 == end2;
    }

    template <class T, class Alloc>
    inline bool operator!=(const list<T, Alloc>& x, const list<T, Alloc>& y)
    { return !(x == y); }
    
} /* namespace STL */ 

#endif
