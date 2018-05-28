#ifndef TINYSTL_LIST_H_
#define TINYSTL_LIST_H_ 

#include <initializer_list>
#include <type_traits>

#include "algobase.h"
#include "allocator.h"
#include "iterator.h"

namespace STL {
    
    // list的节点结构
    template <class T>
    struct list_node {
        list_node<T>* prev;
        list_node<T>* next;
        T data;
    };

    /**
     *  @brief  list::iterator
     *
     *  所有函数是操作符重载
     */ 
    template <class T>
    struct list_iterator : public STL::iterator<STL::bidirectional_iterator_tag, T> {
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

       // Self const_cast() const { return *this; }

        // 对迭代器取值，取的是节点的数据值
        reference operator*() const { return node->data; }
        // 对迭代器的member access运算符
        pointer operator->() const { return &(node->data); }

        // 迭代器+1，前进一个节点
        Self& operator++() {
            node = node->next;
            return *this;
        }
        Self operator++(int) {
            Self tmp = *this;
            node = node->next;
            return tmp;
        }

        // 迭代器-1，后退一个节点
        Self& operator--() {
            node = node->prev;
            return *this;
        }
        Self operator--(int) {
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
    struct list_const_iterator : public STL::iterator<STL::bidirectional_iterator_tag, const T> {
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

       // iterator const_cast() const { return iterator(const_cast<Node*>(node)); }

        // 对迭代器取值，取的是节点的数据值
        reference operator*() { return node->data; }
        // 对迭代器的member access运算符
        pointer operator->() const { return &(static_cast<Node*>(node)->data); }

        // 迭代器+1，前进一个节点
        Self& operator++() {
            node = node->next;
            return *this;
        }
        Self operator++(int) {
            Self tmp = *this;
            node = node->next;
            return tmp;
        }

        // 迭代器-1，后退一个节点
        Self& operator--() {
            node = node->prev;
            return *this;
        }
        Self operator--(int) {
            Self tmp = *this;
            node = node->prev;
            return tmp;
        }

        bool operator==(const Self& x) const { return node == x.node; }
        bool operator!=(const Self& x) const { return node != x.node; }
    };

    template <class T, class Alloc = STL::pool_alloc>
    class list {
    public:
        using value_type                = T;
        using pointer                   = T*;
        using const_pointer             = const T*;
        using reference                 = T&;
        using const_reference           = const T&;
        using iterator                  = list_iterator<T>;
        using const_iterator            = list_const_iterator<T>;
        // using const_reverse_iterator    = size_t;
        using size_type                 = size_t;
        using difference_type           = ptrdiff_t;

    protected:
        using Node                      = list_node<T>;
        using list_node_allocator       = STL::allocator<Node, Alloc>;

        Node* node;     // 用一个指针(指向end)，表示整个环状双向链表

        // 配置一个节点并传回
        Node* get_node() { return list_node_allocator::allocate(); }

        // 释放一个节点
        void put_node(Node* p) { list_node_allocator::deallocate(p); } 
        
        // 初始化list，node头尾指向自己
        void init() {
            node->next = node;
            node->prev = node;
        }

        // 初始化空list
        void empty_init() {
            node = get_node();
            init();
        }

        // 为新节点分配空间，在上面构造args的拷贝
#if __cplusplus < 201103L
        Node* create_node(const value_type& x) {
            Node* p = get_node();
            try {
                STL::construct(&p->data, x);
            } catch (...) {
                put_node(p);
                throw;
            }
            return p;
        }
#else 
        Node* create_node(const value_type& x) {
            Node* p = get_node();
            try {
                STL::construct(&p->data, x);
            } catch (...) {
                put_node(p);
                throw;
            }
            return p;
        }
#endif
        
        // range constructor
        template <class InputIterator>
        void initialize_dispatch(InputIterator first, InputIterator last, std::false_type) {
            for ( ; first != last; ++first)
#if __cplusplus >= 201103L
                push_back(*first);
#else 
                push_back(*first);
#endif 
        }

        // 消除歧义
        template <class Integer>
        void initialize_dispatch(Integer n, Integer x, std::true_type) {
            fill_initialize(static_cast<size_type>(n), x);
        }

        // 被list(n, x)调用，或者range constructor产生的歧义
        void fill_initialize(size_type n, const value_type& x) {
            for ( ; n; --n)
                push_back(x);
        }
 
        // 析构一个节点并释放空间
        void destroy_node(Node* p) {
            STL::destroy(&p->data);
            put_node(p);
        }
        
        // 清除list所有节点
        void clear_nodes() {
            Node* cur = node->next; // begin()
            while (cur != node) {
                Node* tmp = cur;
                cur = cur->next;
                destroy_node(tmp);
            }
        }

        // 在pos位置插入新节点x
#if __cplusplus < 201103L
        iterator insert(iterator pos, const value_type& x) {
            Node* tmp = create_node(x);
            tmp->next = pos.node;
            tmp->prev = pos.node->prev;
            (pos.node)->next = tmp;
            pos.node->prev = tmp;
            return tmp;
        }
#else 
        iterator insert(iterator pos, const value_type& x) {
            Node* tmp = create_node(x);
            tmp->next = pos.node;
            tmp->prev = (pos.node)->prev;
            (pos.node->prev)->next = tmp;
            pos.node->prev = tmp;
            return tmp;
        }
#endif 

        // 将[first, last)内所有节点移动到pos指向的节点之前
        void transfer(iterator pos, iterator first, iterator last) {
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
        // 构造、拷贝、析构

        /**
         *  @brief  构造函数
         */ 
        list() { empty_init(); }

        explicit list(size_type n, const value_type& x = value_type()) {
            empty_init();
            fill_initialize(n, x);
        }

        list(std::initializer_list<value_type> l) { 
            empty_init();
            initialize_dispatch(l.begin(), l.end(), std::false_type());
        }

        /**
         *  @brief  拷贝构造函数
         */ 
        list(const list& l) {
            empty_init();
            initialize_dispatch(l.begin(), l.end(), std::false_type());
        }

        /**
         *  @brief  析构函数
         */ 
        ~list() {
            clear_nodes();
            STL::destroy(node);
        }

        // 元素访问
        reference front() noexcept { return *begin(); }
        const_reference front() const noexcept { return *begin(); }
        reference back() { return *iterator(--end()); }
        const_reference back() const noexcept { return *const_iterator(--end()); }

        // iterator 
        iterator begin() noexcept { return node->next; }
        const_iterator begin() const noexcept { return node->next; }
        iterator end() noexcept { return node; }
        const_iterator end() const noexcept { return node; }

        // 容量 
        bool empty() const noexcept { return node->next == node; }
        size_type size() const noexcept { return STL::distance(begin(), end()); }
        size_type max_size() const noexcept { return list_node_allocator::max_size(); } 


        // 修改 
       
        /**
         *  @brief  清除list所有节点
         */ 
        void clear() {
            clear_nodes();
            // 恢复原始状态
            init();
        }

        /**
         *  @brief  移除迭代器pos所指节点
         */ 
        iterator erase(iterator pos) {
            Node* next_node = pos.node->next;
            Node* prev_node = pos.node->prev;
            prev_node->next = next_node;
            next_node->prev = prev_node;
            destroy_node(pos.node);
            return (iterator)next_node;
        }
        
        /**
         *  @brief  移除尾节点
         */ 
        void pop_back() { erase(--end()); }

        /**
         *  @brief  移除头节点
         */ 
        void pop_front() { erase(begin()); }

        /**
         *  @brief  在list尾部插入元素x
         */ 
        void push_back(const value_type& x) { insert(end(), x); }

        /**
         *  @brief  在list头部插入元素x
         */ 
        void push_front(const value_type& x) { insert(begin(), x); }
        
        /*
         *  @brief  移除值为x的所有节点
         */ 
        void remove(const value_type& x) {
            for (iterator first = begin(); first != end(); ) {
                iterator next = first;
                ++next;
                if (*first == x) erase(first);
                first = next;
            }
        }

        /**
         *  @brief  将list逆序
         */ 
        void reverse() {
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
         *  @brief  将链表x合并到*this 
         *
         *  两个lists必须先递增排序
         */
        void  
#if __cplusplus >= 201103L
        merge(list& x) {
#else 
        merge(list& x) {
#endif 
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

        /** 
         *  @brief  与链表x交换数据
         *
         *  全局函数STL::swap(list1, list2)特化为本函数
         */ 
        void swap(list& x) {
            STL::swap(node, x.node);
        }

        /**
         *  @brief  list排序，merge sort 
         *
         *  ！！！！！很有趣的算法！！！！！
         *
         *  https://www.zhihu.com/question/31478115
         */ 
        void sort() {
            // 空链表或仅有一个元素，直接返回
            if (node->next == node || node->next->next == node) return;
            // 一些新的list作为数据缓冲区
            list carry;     // 每次取源list的头节点
            list tmp[64];   // 64个桶，tmp[i]的大小为2^i
            int fill = 0;   // 有元素的桶的最大index + 1
            while (!empty()) {
                carry.splice(carry.begin(), *this, begin());    // 取源list的头节点
                int i = 0;
                // index < fill的桶未满时，不断向桶内搬运节点，桶内有序
                // index < fill的桶全满时(i能==fill)，则将这些桶归并到carry，再存放到tmp[fill]
                while (i < fill && !tmp[i].empty()) {
                    carry.merge(tmp[i]);
                    ++i;
                }
                tmp[i].swap(carry);
                // index < fill的桶全满，增加下一个桶
                if (i == fill)  ++fill;
            }
            // 将所有桶归并
            for (int i = 1; i < fill; ++i) {
                tmp[i].merge(tmp[i - 1]);
            }
            swap(tmp[fill - 1]);
        }


        /**
         *  @brief  将链表x接合于pos所指位置之前
         *
         *  x变为空链表，需要this != x
         */
        void 
#if __cplusplus >= 201103L
        splice(iterator pos, list& x) {
#else 
        splice(iterator pos, list& x) {
#endif 
            if (!x.empty())
                transfer(pos, x.begin(), x.end());
        }

        /**
         *  @brief  将i所指元素接合于pos之前
         *
         *  pos和i可指向同一个list
         */
        void 
#if __cplusplus >= 201103L
        splice(iterator pos, list&, iterator i) {
#else 
        splice(iterator pos, list&, iterator i) {
#endif 
            iterator j = i;
            ++j;
            if (pos == i || pos == j)   return;
            transfer(pos, i, j);
        }

        /**
         *  @brief  将[first, last)内所有元素接合于pos之前
         *
         *  pos和[first, last)可指向同一个list，若pos在[first, last)之内则未定义
         */ 
        void 
#if __cplusplus >= 201103L
        splice(iterator pos, list&, iterator first, iterator last) {
#else 
        splice(iterator pos, list&, iterator first, iterator last) {
#endif 
            if (first != last)
                transfer(pos, first, last);
        }

        /**
         *  @brief  移除值相同的连续元素 
         */ 
        void unique() {
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


    };
    
} /* namespace STL */ 

#endif
