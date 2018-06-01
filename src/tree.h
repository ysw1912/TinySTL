#ifndef TINYSTL_TREE_H_ 
#define TINYSTL_TREE_H_ 

#include <memory>

#include "allocator.h"
#include "iterator.h"

namespace STL {

    enum rb_tree_color { red = false, black = true };

    struct rb_tree_node_base {
        using base_ptr          = rb_tree_node_base*;
        using const_base_ptr    = const rb_tree_node_base*;

        rb_tree_color color;    // 节点颜色
        base_ptr parent;        // 父节点
        base_ptr left;          // 左节点
        base_ptr right;         // 右节点
        
        static base_ptr minimum(base_ptr x) {   // RB-tree最小值节点
            while (x->left) x = x->left;
            return x;
        }

        static const_base_ptr minimum(const_base_ptr x) {  
            while (x->left) x = x->left;
            return x;
        }

        static base_ptr maximum(base_ptr x) {   // RB-tree最大值节点
            while (x->right) x = x->right;
            return x;
        }

        static const_base_ptr maximum(const_base_ptr x) {  
            while (x->right) x = x->right;
            return x;
        }
    };

    template <class Val>
    struct rb_tree_node : public rb_tree_node_base {
        using link_type = rb_tree_node<Val>*;
        Val value_field;    // 节点值
        
        Val* valptr() { return std::addressof(value_field); }
        const Val* valptr() const { return std::addressof(value_field); }
    };

    // 返回rb_tree的后继节点
    rb_tree_node_base* rb_tree_increment(rb_tree_node_base* x) noexcept {
        rb_tree_node_base* x_copy = x;
        if (x_copy->right) { // 有右子节点，则找出右子树的最小节点
            x_copy = x_copy->right;
            while (x_copy->left)
                x_copy = x_copy->left;
        } else {    // 无右子节点
            rb_tree_node_base* y = x_copy->parent;
            while (x_copy == y->right) {    // 沿父节点向上找，直到现行节点x_copy是y的左子节点
                x_copy = y;
                y = y->parent;
            }
            if (x_copy->right != y)
                x_copy = y;
        }
        return x_copy;
    }

    const rb_tree_node_base* rb_tree_increment(const rb_tree_node_base* x) noexcept {
        const rb_tree_node_base* x_copy = x;
        if (x_copy->right) { // 有右子节点，则找出右子树的最小节点
            x_copy = x_copy->right;
            while (x_copy->left)
                x_copy = x_copy->left;
        } else {    // 无右子节点
            const rb_tree_node_base* y = x_copy->parent;
            while (x_copy == y->right) {    // 沿父节点向上找，直到现行节点x_copy是y的左子节点
                x_copy = y;
                y = y->parent;
            }
            if (x_copy->right != y)
                x_copy = y;
        }
        return x_copy;
    }

    // 返回rb_tree的前驱节点
    rb_tree_node_base* rb_tree_decrement(rb_tree_node_base* x) noexcept {
        rb_tree_node_base* x_copy = x;
        if (x_copy->color == red && x_copy->parent->parent == x_copy)   // x为header时
           x_copy = x_copy->right; 
        else if (x_copy->left) {  // x有左子节点
            rb_tree_node_base* y = x_copy->left;
            while (y->right)
                y = y->right;
            x_copy = y;
        } else {    // 非header，也无左子节点
            rb_tree_node_base* y = x_copy->parent;
            while (x_copy == y->left) { // 沿父节点向上找，直到现行节点x_copy是y的右子节点
                x_copy = y;
                y = y->parent;
            }
            x_copy = y;
        }
        return x_copy;
    }

    const rb_tree_node_base* rb_tree_decrement(const rb_tree_node_base* x) {
        const rb_tree_node_base* x_copy = x;
        if (x_copy->color == red && x_copy->parent->parent == x_copy)   // x为header时
           x_copy = x_copy->right; 
        else if (x_copy->left) {  // x有左子节点
            const rb_tree_node_base* y = x_copy->left;
            while (y->right)
                y = y->right;
            x_copy = y;
        } else {    // 非header，也无左子节点
            const rb_tree_node_base* y = x_copy->parent;
            while (x_copy == y->left) { // 沿父节点向上找，直到现行节点x_copy是y的右子节点
                x_copy = y;
                y = y->parent;
            }
            x_copy = y;
        }
        return x_copy;
    }

    template <class T>
    struct rb_tree_iterator {
        using value_type    = T;
        using pointer       = T*;
        using reference     = T&;

        using iterator_category = STL::bidirectional_iterator_tag;
        using differnce_type    = ptrdiff_t;

        using Self      = rb_tree_iterator<T>;
        using Base_ptr  = rb_tree_node_base::base_ptr;  // 指向node_base的指针
        using Link_type = rb_tree_node<T>*;             // 指向node的指针

        Base_ptr node;  // 与容器rb_tree产生连结关系的节点指针

        rb_tree_iterator() : node() { }
        explicit rb_tree_iterator(Base_ptr x) : node(x) { }

        reference operator*() const { return *static_cast<Link_type>(node)->valptr(); }
        pointer operator->() const { return static_cast<Link_type>(node)->valptr(); }

        Self& operator++() {
            node = rb_tree_increment(node);
            return *this;
        }

        Self& operator++(int) {
            Self tmp = *this;
            node = rb_tree_increment(node);
            return tmp;
        }

        Self& operator--() {
            node = rb_tree_decrement(node);
            return *this;
        }

        Self& operator--(int) {
            Self tmp = *this;
            node = rb_tree_decrement(node);
            return tmp;
        }

        bool operator==(const Self& x) const { return node == x.node; }
        bool operator!=(const Self& x) const { return node != x.node; }
    };

    template <class T>
    struct rb_tree_const_iterator {
        using value_type    = T;
        using pointer       = const T*;
        using reference     = const T&;
        using iterator      = rb_tree_iterator<T>;

        using iterator_category = STL::bidirectional_iterator_tag;
        using differnce_type    = ptrdiff_t;

        using Self      = rb_tree_const_iterator<T>;
        using Base_ptr  = rb_tree_node_base::const_base_ptr;    // 指向node_base的指针
        using Link_type = const rb_tree_node<T>*;                     // 指向node的指针

        Base_ptr node;  // 与容器rb_tree产生连结关系的节点指针

        rb_tree_const_iterator() : node() { }
        explicit rb_tree_const_iterator(Base_ptr x) : node(x) { }
        rb_tree_const_iterator(const iterator& it) : node(it.node) { }
            
        iterator M_const_cast() const { return iterator(const_cast<typename iterator::Base_ptr>(node)); }

        reference operator*() const { return *static_cast<Link_type>(node)->valptr(); }
        pointer operator->() const { return static_cast<Link_type>(node)->valptr(); }

        Self& operator++() {
            node = rb_tree_increment(node);
            return *this;
        }

        Self& operator++(int) {
            Self tmp = *this;
            node = rb_tree_increment(node);
            return tmp;
        }

        Self& operator--() {
            node = rb_tree_decrement(node);
            return *this;
        }

        Self& operator--(int) {
            Self tmp = *this;
            node = rb_tree_decrement(node);
            return tmp;
        }

        bool operator==(const Self& x) const { return node == x.node; }
        bool operator!=(const Self& x) const { return node != x.node; }
    };

    void rb_tree_insert_and_rebalance(const bool insert_left,
                                      rb_tree_node_base* x,
                                      rb_tree_node_base* p,
                                      rb_tree_node_base& header) noexcept {
        
    }

    rb_tree_node_base* rb_tree_rebalance_for_erase(rb_tree_node_base* const z,
                                                   rb_tree_node_base& header) noexcept {
        
    }

    template <class Key, class Value, class KeyOfValue, class Compare,
              class Alloc = STL::pool_alloc>
    class rb_tree {
    protected:
    };

} /* namespace STL */

#endif 
