#ifndef TINYSTL_LIST_H_
#define TINYSTL_LIST_H_ 

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
        using Self      = list_const_iterator<T>;
        using Node      = const list_node<T>;
        using iterator  = list_iterator<T>;
 
        using reference = typename STL::iterator<STL::bidirectional_iterator_tag, T>::reference;
        using pointer   = typename STL::iterator<STL::bidirectional_iterator_tag, T>::pointer;

        // 唯一成员，指向list的节点
        const Node* node;

        // constructor
        list_const_iterator() : node() {}
        list_const_iterator(const Node* x) : node(x) {}
        list_const_iterator(const iterator& x) : node(x.node) {}

       // iterator const_cast() const { return iterator(const_cast<Node*>(node)); }

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
        using const_reverse_iterator    = list_const_iterator<T>;
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
        
        // 初始化node，创建空节点
        void empty_init() {
            node = get_node();
            // node头尾都指向自己
            node->next = node;
            node->prev = node;
        }
        /**
         *  @param  args  用户data
         *
         *  为新节点分配空间，在上面构造args的拷贝
         */ 
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
        
        /** 
         *  析构一个节点并释放空间
         */ 
        void destroy_node(Node* p) {
            STL::destroy(&p->data);
            put_node(p);
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

        /**
         *  @brief  将[first, last)内所有节点移动到pos指向的节点之前
         */ 
        void transfer(iterator pos, iterator first, iterator last) {
            if (pos != last) {
                last.node->prev->next = pos.node;   // last前一个节点 后指向 pos 
                first.node->prev->next = last.node; // first前一个节点 后指向 last 
                pos.node->prev->next = first.node;  // pos前一个节点 后指向 first 
                Node* tmp = pos.node->prev;        // tmp = last前一个节点
                pos.node->prev = last.node->prev;   // pos 前指向 last前一个节点
                last.node->prev = first.node->prev; // last 前指向 first前一个节点
                first.node->prev = tmp;             // first 前指向 pos原来的前一个节点
            }
        }

    public:
        // 构造、拷贝、析构
        list() { empty_init(); }

        // iterator 
        iterator begin() noexcept { return iterator(node->next); }
        const_iterator begin() const noexcept { return const_iterator(node->next); }
        iterator end() noexcept { return iterator(node); }
        const_iterator end() const noexcept { return const_iterator(node); }

        // 
        bool empty() const { return node->next == node; }
        size_type size() const {
            return STL::distance(begin(), end());
        }

        // 
        reference front() { return *begin(); }
        reference back() { return *(--end()); }

        // 
       
        /**
         *  @brief  清除list所有节点
         */ 
        void clear() {
            Node* cur = node->next; // begin()
            while (cur != node) {
                Node* tmp = cur;
                cur = cur->next;
                destroy_node(tmp);
            }
            // 恢复原始状态
            node->next = node;
            node->prev = node;
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
