#ifndef TINYSTL_MAP_H_
#define TINYSTL_MAP_H_ 

#include <initializer_list>

#include "tree.h"

namespace STL {

    template <class Key, class T, class Compare = std::less<Key>,
              class Alloc = STL::pool_alloc>
    class map {
    
    public:
        using key_type      = Key;                  // 键值类型
        using mapped_type   = T;                    // 实值/数据类型
        using value_type    = pair<const Key, T>;   // 元素类型（键值/实值）
        using key_compare   = Compare;    
        
        // functor，用于调用元素比较函数
        class value_compare
        : public std::binary_function<value_type, value_type, bool> {
            friend class map<Key, T, Compare, Alloc>;
        protected:
            Compare cmp;
            value_compare(Compare c) : cmp(c) { }
        
        public:
            bool operator()(const value_type& x, const value_type& y) const 
            { return cmp(x.first, y.first); }
        };

        using Rep_type  = STL::rb_tree<key_type, value_type, std::_Select1st<value_type>, key_compare>;
        Rep_type t;

    public:
        using pointer           = typename Rep_type::pointer;
        using const_pointer     = typename Rep_type::const_pointer;
        using reference         = typename Rep_type::reference;
        using const_reference   = typename Rep_type::const_reference;
        // 允许用户修改元素的实值
        using iterator          = typename Rep_type::iterator;
        using const_iterator    = typename Rep_type::const_iterator;
        using size_type         = typename Rep_type::size_type;
        using difference_type   = typename Rep_type::difference_type;

    public:
        // The big five
        
        /**
         *  @brief  constructor
         */ 
        map() : t(Compare()) { }
        
        explicit map(const Compare& cmp) : t(cmp) { }

        map(std::initializer_list<value_type> l, const Compare& cmp = Compare())
        : t(cmp) { t.insert_unique(l.begin(), l.end()); }

        template <class InputIterator>
        map(InputIterator first, InputIterator last, const Compare& cmp = Compare())
        : t(cmp) { t.insert_unique(first, last); }

        /**
         *  @brief  copy constructor
         */
        map(const map& x) : t(x.t) { }

        /**
         *  @brief  move constructor
         */ 
        map(map&& x) : t(std::move(x.t)) { }

        /**
         *  @brief  copy assignment
         */
        map& operator=(const map& x) {
            t = x.t;
            return *this;
        }

        /**
         *  @brief  move assignment
         */
        map& operator=(map&&) = default;

    public:
        // 元素访问
        mapped_type& operator[](const key_type& k) {
            return (*(insert(value_type(k, mapped_type()))).first).second;
        }
        
    public:
        // 迭代器
        iterator begin() noexcept { return t.begin(); }
        const_iterator begin() const noexcept { return t.begin(); }
        const_iterator cbegin() const noexcept { return t.begin(); }
        iterator end() noexcept { return t.end(); }
        const_iterator end() const noexcept { return t.end(); }
        const_iterator cend() const noexcept { return t.end(); }

    public:
        // 容量
        bool empty() const noexcept { return t.empty(); }
        size_type size() const noexcept { return t.size(); }
        size_type max_size() const noexcept { return t.max_size(); }
    
    public: 
        // 修改器
         
        /**
         *  @brief  移除map中所有元素
         */ 
        void clear() noexcept { t.clear(); }

        /**
         *  @brief  插入值为x的元素
         *  @return  bool  是否插入成功
         *           iterator  若成功，指向新插入元素；若失败，指向键值重复的旧元素
         */ 
        pair<iterator, bool> insert(const value_type& x)
        { return t.insert_unique(x); }
        
        /**
         *  @brief  插入来自[first, last)的元素
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
         *  @brief  移除map中迭代器范围[first, last)的元素
         */ 
        iterator erase(const_iterator first, const_iterator last)
        { return t.erase(first, last); }

        /**
         *  @brief  移除map中键值为x的元素
         *  @return  移除元素的数量
         */
        size_type erase(const key_type& x)
        { return t.erase(x); }

        /**
         *  @brief  与map x交换数据
         */ 
        void swap(map& x) noexcept { t.swap(x.t); }
        
    public:
        // 查找
        
        /**
         *  @brief  返回map中键值等于k的元素个数
         */ 
        size_type count(const key_type& k) const 
        { return t.count(k); }

        /**
         *  @brief  查寻map中是否有键值等于k的元素
         *
         *  没找到则返回end()
         */ 
        iterator find(const key_type& k)
        { return t.find(k); }

        const_iterator find(const key_type& k) const 
        { return t.find(k); }

        /**
         *  @brief  查寻map中所有键值为k的元素范围，范围以返回的两个迭代器定义
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
