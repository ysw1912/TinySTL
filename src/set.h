#ifndef TINYSTL_SET_H_ 
#define TINYSTL_SET_H_ 

#include <functional>
#include <initializer_list>

#include "tree.h"

namespace STL {

    template <class Key, class Compare = std::less<Key>,
              class Alloc = STL::pool_alloc>
    class set {
    public:
        using key_type      = Key;
        using value_type    = Key;
        using key_compare   = Compare;
        using value_compare = Compare;

    private:
        using Rep_type  = STL::rb_tree<key_type, value_type, std::_Identity<value_type>, key_compare>;
        Rep_type t;     // 使用红黑树represent集合

    public:
        using pointer           = typename Rep_type::pointer;
        using const_pointer     = typename Rep_type::const_pointer;
        using reference         = typename Rep_type::reference;
        using const_reference   = typename Rep_type::const_reference;
        // set的迭代器无法执行写入操作
        using iterator          = typename Rep_type::const_iterator;
        using const_iterator    = typename Rep_type::const_iterator;
        using size_type         = typename Rep_type::size_type;
        using difference_type   = typename Rep_type::difference_type;

    public:
        // The big five
        
        /**
         *  @brief  constructor
         */ 
        set() : t() { }

        explicit set(const Compare& cmp) : t(cmp) { }

        template <class InputIterator>
        set(InputIterator first, InputIterator last, const Compare& cmp = Compare())
        : t(cmp) { t.insert_unique(first, last); }

        set(std::initializer_list<value_type> l, const Compare& cmp = Compare())
        : t(cmp) { t.insert_unique(l.begin(), l.end()); }

        /**
         *  @brief  copy constructor
         */ 
        set(const set& x) : t(x.t) { }

        /**
         *  @brief move constructor
         */ 
        set(set&& x) : t(std::move(x.t)) { }

        /**
         *  @brief copy assignment
         */ 
        set& operator=(const set& x) {
            t = x.t;
            return *this;
        }

        /**
         *  @brief  move assignment
         */ 
        set& operator=(set&&) = default;

    public:
        // 观察器
        key_compare key_cmp() const { return t.key_cmp(); }
        value_compare value_cmp() const { return t.key_cmp(); }

    public:
        // 迭代器
        iterator begin() const { return t.begin(); }
        iterator cbegin() const { return t.begin(); }
        iterator end() const { return t.end(); }
        iterator cend() const { return t.end(); }

    public:
        // 容量
        bool empty() const { return t.empty(); }
        size_type size() const { return t.size(); }
        size_type max_size() const { return t.max_size(); }

    public:
        // 修改器
        
        /**
         *  @brief  移除set中所有元素
         */
        void clear() noexcept { t.clear(); }

        /**
         *  @brief  插入值为x的元素
         */ 
        pair<iterator, bool> insert(const value_type& x) {
            pair<typename Rep_type::iterator, bool> p = t.insert_unique(x);
            return pair<iterator, bool>(p.first, p.second);
        }

        /**
         *  @brief  插入来自范围[first, last)的元素
         */ 
        template <class InputIterator>
        void insert(InputIterator first, InputIterator last)
        { t.insert_unique(first, last); }

        /**
         *  @brief  插入来自initialize list的元素
         */ 
        void insert(std::initializer_list<value_type> l)
        { this->insert(l.begin(), l.end()); }

        /**
         *  @brief  移除位于pos的元素
         */ 
        iterator erase(const_iterator pos)
        { return t.erase(pos); }

        /**
         *  @brief  移除set中迭代器范围[first, last)的元素
         */ 
        iterator erase(const_iterator first, const_iterator last)
        { return t.erase(first, last); }

        /**
         *  @brief  移除set中键值为x的元素
         *  @return  移除元素的数量
         */
        size_type erase(const key_type& x)
        { return t.erase(x); }

        /**
         *  @brief  与set x交换数据
         */ 
        void swap(set& x) noexcept { t.swap(x.t); }
        
    public:
        // 查找
        
        /**
         *  @brief  返回set中键值等于k的元素个数
         */ 
        size_type count(const key_type& k) const 
        { return t.count(k); }

        /**
         *  @brief  查寻set中是否有键值等于k的元素
         *
         *  没找到则返回end()
         */ 
        iterator find(const key_type& k)
        { return t.find(k); }

        const_iterator find(const key_type& k) const 
        { return t.find(k); }

        /**
         *  @brief  查寻set中所有键值为k的元素范围，范围以返回的两个迭代器定义
         *  @return  第一个迭代器指向首个键值不小于k的元素，第二个迭代器指向首个键值大于k的元素
         *
         *  若无元素的键值不小于k 或 大于k，则相应的迭代器将返回end()
         */ 
        pair<iterator, iterator> equal_range(const key_type& k) const 
        { return t.equal_range(k); }
        
        /**
         *  @brief  返回首个不小于k的元素的迭代器
         */
        iterator lower_bound(const key_type& k) const 
        { return t.lower_bound(k); } 
        
        /**
         *  @brief  返回首个大于k的元素的迭代器
         */ 
        iterator upper_bound(const key_type& k) const 
        { return t.upper_bound(k); }

    };

} /* namespace STL */ 

#endif
