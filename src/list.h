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
    struct list_iterator {
        using Self              = list_iterator<T>;
        using Node              = list_node<T>;
 
        using iterator_category = STL::bidirectional_iterator_tag;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;
        using difference_type   = ptrdiff_t;

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
    struct list_const_iterator {
        using Self              = list_const_iterator<T>;
        using Node              = const list_node<T>;
        using iterator          = list_iterator<T>;
 
        using iterator_category = STL::bidirectional_iterator_tag;
        using value_type        = T;
        using pointer           = const T*;
        using reference         = const T&;
        using difference_type   = ptrdiff_t;

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

    };
    
} /* namespace STL */ 

#endif
