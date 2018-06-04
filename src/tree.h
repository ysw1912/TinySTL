#ifndef TINYSTL_TREE_H_ 
#define TINYSTL_TREE_H_ 

#include <memory>
#include <utility>

#include "allocator.h"
#include "iterator.h"

using std::pair;

namespace STL {

    enum rb_tree_color { red = false, black = true };

    struct rb_tree_node_base {
        using Base_ptr          = rb_tree_node_base*;
        using Const_Base_ptr    = const rb_tree_node_base*;

        rb_tree_color color;    // 节点颜色
        Base_ptr parent;        // 父节点
        Base_ptr left;          // 左节点
        Base_ptr right;         // 右节点
        
        static Base_ptr minimum(Base_ptr x) {   // RB-tree最小值节点
            while (x->left) x = x->left;
            return x;
        }

        static Const_Base_ptr minimum(Const_Base_ptr x) {  
            while (x->left) x = x->left;
            return x;
        }

        static Base_ptr maximum(Base_ptr x) {   // RB-tree最大值节点
            while (x->right) x = x->right;
            return x;
        }

        static Const_Base_ptr maximum(Const_Base_ptr x) {  
            while (x->right) x = x->right;
            return x;
        }
    };

    template <class Val>
    struct rb_tree_node : public rb_tree_node_base {
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
        using Base_ptr  = rb_tree_node_base::Base_ptr;  // 指向node_base的指针
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
        using Base_ptr  = rb_tree_node_base::Const_Base_ptr;    // 指向node_base的指针
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
/*
    void rb_tree_insert_and_rebalance(const bool insert_left,
                                      rb_tree_node_base* x,
                                      rb_tree_node_base* p,
                                      rb_tree_node_base& header) noexcept {
        
    }

    rb_tree_node_base* rb_tree_rebalance_for_erase(rb_tree_node_base* const z,
                                                   rb_tree_node_base& header) noexcept {
        
    }
*/ 

    // 在旋转点x处进行左旋转
    void rb_tree_rotate_left(rb_tree_node_base* x, rb_tree_node_base*& root) {
        rb_tree_node_base* y = x->right;    // y指向旋转点的右孩子
        x->right = y->left;
        if (y->left)    y->left->parent = x;    // 设完A->child = B，记得设置B->parent = A
        
        // 令y完全接替x的位置
        y->parent = x->parent;
        if (x == root)                  // x为根节点  
            root = y;
        else if (x == x->parent->left)  // x为其父的左孩子
            x->parent->left = y;
        else                            // x为其父的右孩子
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    // 在旋转点x处进行右旋转，代码与左旋转完全对称
    void rb_tree_rotate_right(rb_tree_node_base* x, rb_tree_node_base*& root) {
        rb_tree_node_base* y = x->left;
        x->left = y->right;
        if (y->right)    y->right->parent = x;
        y->parent = x->parent;
        if (x == root)  
            root = y;
        else if (x == x->parent->right)
            x->parent->right = y;
        else 
            x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    /**
     *  @brief  插入节点后，重新调整RB-tree至平衡
     *  @param  x  指向新增节点
     *  @param  root  指向根节点
     */ 
    void rb_tree_rebalance(rb_tree_node_base* x, rb_tree_node_base*& root) {
        x->color = red;     // 新节点必为红
        while (x != root && x->parent->color == red) {
            // x的父节点为祖父节点的左孩子
            if (x->parent == x->parent->parent->left) {
                rb_tree_node_base* y = x->parent->parent->right;    // y为x的伯父节点
                if (y && y->color == red) {    // 伯父节点存在且为红
                    x->parent->color = black;
                    y->color = black;           // 父节点、伯父节点改为黑
                    y->parent->color = red;     // 祖父节点改为红
                    x = y->parent;  // x指向祖父节点，继续向上判断
                } else {    // 无伯父节点，或伯父节点为黑
                    if (x == x->parent->right) {    // x为父节点的右孩子，即内侧插入
                        x = x->parent;
                        rb_tree_rotate_left(x, root);   // 左旋转
                    }
                    x->parent->color = black;
                    x->parent->parent->color = red;
                    rb_tree_rotate_right(x->parent->parent, root);  // 右旋转
                }
            } else {    // 与if部分完全对称
                rb_tree_node_base* y = x->parent->parent->left;
                if (y && y->color == red) {
                    x->parent->color = black;
                    y->color = black;      
                    y->parent->color = red; 
                    x = y->parent; 
                } else {   
                    if (x == x->parent->left) {
                        x = x->parent;
                        rb_tree_rotate_right(x, root);
                    }
                    x->parent->color = black;
                    x->parent->parent->color = red;
                    rb_tree_rotate_left(x->parent->parent, root);
                }

            }
        } // while 
        root->color = black;    // 根节点永远为黑
    }

    template <class Key, class Val, class KeyOfValue, class Compare,
              class Alloc = STL::pool_alloc>
    class rb_tree {
    protected:
        using rb_tree_node_allocator    = STL::allocator<rb_tree_node<Val>, Alloc>;
        using Base_ptr                  = rb_tree_node_base*;
        using Const_Base_ptr            = const rb_tree_node_base*;
        using Link_type                 = rb_tree_node<Val>*;
        using Const_Link_type           = const rb_tree_node<Val>*;

    public:
        using key_type          = Key;
        using value_type        = Val;
        using pointer           = value_type*;
        using const_pointer     = const value_type*;
        using reference         = value_type&;
        using const_reference   = const value_type&;
        using size_type         = size_t;
        using differnce_type    = ptrdiff_t;

    protected:
        Link_type get_node() { return rb_tree_node_allocator::allocate(); }
        void put_node(Link_type p) { rb_tree_node_allocator::deallocate(p); }
        
        // 构造节点
        template <class... Args>
        void construct_node(Link_type node, Args&&... args) {
            try {
                ::new(node) rb_tree_node<value_type>;
                STL::construct(node->valptr(), std::forward<Args>(args)...);
            } catch(...) {
                node->~rb_tree_node<value_type>();
                put_node(node);
                throw;
            }
        }

        // 创建节点 = 分配内存 + 构造节点值
        template <class... Args>
        Link_type create_node(Args&&... args) {
            Link_type tmp = get_node();
            construct_node(tmp, std::forward<Args>(args)...);
            return tmp;
        }

        // 析构节点
        void destroy_node(Link_type p) noexcept {
            STL::destroy(p->valptr());
            p->~rb_tree_node<value_type>();
        }

        // 删除节点 = 析构节点 + 释放内存
        void drop_node(Link_type p) {
            destroy_node(p);
            put_node(p);
        }

        // 克隆节点（值和颜色）
        Link_type clone_node(Const_Link_type x) {
            Link_type tmp = create_node(*x->valptr());
            tmp->color = x->color;
            tmp->left = tmp->right = 0;
            return tmp;
        }

    protected:
        Compare             key_compare;    // 节点间的键值大小的比较函数对象
        rb_tree_node_base   header;
        size_type           node_count;     // 追踪记录树的大小

        void reset() {
            header.parent = 0;
            header.left = &header;
            header.right = &header;
            node_count = 0;
        }

    private:
        void initialize() {
            header.color = red;     // 令header为红色，以区分header和root
            header.parent = 0;
            header.left = &header;  // header的左/右子节点指向自己
            header.right = &header;
        }
    
    protected:
        // 取得header的成员
        Base_ptr& root() { return header.parent; }
        Const_Base_ptr& root() const { return header.parent; }
        Base_ptr& leftmost() { return header.left; }
        Const_Base_ptr& leftmost() const { return header.left; }
        Base_ptr& rightmost() { return header.right; }
        Const_Base_ptr& rightmost() const { return header.right; }
        Link_type M_begin() { return static_cast<Link_type>(header.parent); }
        Const_Link_type M_begin() const { return static_cast<Const_Link_type>(header.parent); }
        Link_type M_end() { return static_cast<Link_type>(&header); }
        Const_Link_type M_end() const { return static_cast<Const_Link_type>(&header); }

        // 取得x的成员
        static const_reference value(Const_Link_type x) { return *x->valptr(); }
        static const_reference value(Const_Base_ptr x) { return *static_cast<Const_Link_type>(x)->valptr(); }
        static const key_type& key(Const_Link_type x) { return KeyOfValue()(value(x)); }
        static const key_type& key(Const_Base_ptr x) { return KeyOfValue()(value(x)); }
        static Link_type left(Base_ptr x) { return static_cast<Link_type>(x->left); }
        static Const_Link_type left(Const_Base_ptr x) { return static_cast<Const_Link_type>(x->left); }
        static Link_type right(Base_ptr x) { return static_cast<Link_type>(x->right); }
        static Const_Link_type right(Const_Base_ptr x) { return static_cast<Const_Link_type>(x->right); }
    
        // 求最小值、最大值
        static Base_ptr minimum(Base_ptr x) { return rb_tree_node_base::minimum(x); }
        static Const_Base_ptr minimum(Const_Base_ptr x) { return rb_tree_node_base::minimum(x); }
        static Base_ptr maximum(Base_ptr x) { return rb_tree_node_base::maximum(x); }
        static Const_Base_ptr maximum(Const_Base_ptr x) { return rb_tree_node_base::maximum(x); }

    public:
        using iterator          = rb_tree_iterator<value_type>;
        using const_iterator    = rb_tree_const_iterator<value_type>;
        
    public:
        // The big five
        rb_tree(const Compare& cmp = Compare())
        : header(), node_count(0), key_compare(cmp) { initialize(); }

    public:
        // 元素访问
        Compare key_cmp() const { return key_compare; }
        iterator begin() { return iterator(header.left); }
        const_iterator begin() const { return const_iterator(header.left); }
        iterator end() { return iterator(&header); }
        const_iterator end() const { return const_iterator(&header); }

    public:
        // 容量
        bool empty() const { return node_count == 0; }
        size_type size() const { return node_count; }
        size_type max_size() const { return rb_tree_node_allocator::max_size(); }

    protected:
        // x - 新值插入点，初始为叶节点的孩子(nullptr)
        // y - x的父节点
        // v - 新值
        iterator M_insert(Base_ptr _x, Base_ptr _y, const value_type& v) {
            Link_type x = static_cast<Link_type>(_x);
            Link_type y = static_cast<Link_type>(_y);
            Link_type z;

            if (y == header || x || key_compare(KeyOfValue()(v), key(y))) {
                z = create_node(v);     // 创建值为v的节点z
                left(y) = z;
            }
        }

    public:
        pair<iterator, bool> insert_unique(const value_type& v) {
            Link_type y = header;
            Link_type x = root();
            bool cmp = true;
            while (x) {     // x从根节点开始，往下寻找适当的插入点
                y = x;
                cmp = key_compare(KeyOfValue()(v), key(x));     // v的键值与当前节点键值比较
                x = cmp ? left(x) : right(x);
            }
            // 此时x为插入点（叶节点），y是其父节点
            iterator j = iterator(y);   // 迭代器j指向y
            return M_insert(x, y, v);
        }
        
        /** 
         *  @brief  插入新值v，节点键值允许重复
         */ 
        iterator insert_equal(const value_type& v) {
            Link_type y = header;
            Link_type x = root();
            while (x) {     // x从根节点开始，往下寻找适当的插入点
                y = x;
                // v 小于 x 往左，v 大于等于 x 往右
                x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
            }
            return M_insert(x, y, v);
        }
    };

} /* namespace STL */

#endif 
