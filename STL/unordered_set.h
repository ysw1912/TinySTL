#ifndef TINYSTL_UNORDERED_SET_H_
#define TINYSTL_UNORDERED_SET_H_ 

#include "hashtable.h"

namespace STL 
{
    template <class Value,
              class HashFcn = std::hash<Value>,
              class EqualKey = std::equal_to<Value>,
              class Alloc = STL::pool_alloc>
    class unordered_set 
    {
    private:
        using Hashtable = STL::hashtable<Value, Value, HashFcn, std::_Identity<Value>, EqualKey, Alloc>;
        Hashtable rep;

    public:
        using key_type          = typename Hashtable::key_type;
        using value_type        = typename Hashtable::value_type;
        using hasher            = typename Hashtable::hasher;
        using key_equal         = typename Hashtable::key_equal;

        using size_type         = typename Hashtable::size_type;
        using difference_type   = typename Hashtable::difference_type;
        using pointer           = typename Hashtable::pointer;
        using const_pointer     = typename Hashtable::const_pointer;
        using reference         = typename Hashtable::reference;
        using const_reference   = typename Hashtable::const_reference;

        using iterator          = typename Hashtable::const_iterator;
        using const_iterator    = typename Hashtable::const_iterator;

    public:
        // The big five
        
        /** 
         *  @brief  constructor
         */ 
        unordered_set() : rep(100) { }
        explicit unordered_set(size_type n) : rep(n) { }
        unordered_set(size_type n, const hasher& hf) : rep(n, hf, key_equal()) { }
        unordered_set(size_type n, const hasher& hf, const key_equal& eql) : rep(n, hf, eql) { }

        /**
         *  @brief  copy constructor
         */
        unordered_set(const unordered_set& x) : rep(x.rep) { }

        /**
         *  @brief  copy assignment
         */
        unordered_set& operator=(const unordered_set& x)
        {
            if (this != &x) {
                rep = x.rep;
            }
            return *this;
        }

        /**
         *  @brief  destructor
         */
        ~unordered_set() { rep.clear(); }

    public:
        // 迭代器
        iterator begin() noexcept { return rep.begin(); }
        const_iterator begin() const noexcept { return rep.begin(); }
        const_iterator cbegin()const noexcept { return rep.cbegin(); }
        iterator end() noexcept { return rep.end(); }
        const_iterator end() const noexcept { return rep.end(); }
        const_iterator cend()const noexcept { return rep.cend(); }

    public:
        // 容量
        size_type size() const noexcept { return rep.size(); }
        size_type max_size() const noexcept { return rep.max_size(); }
        bool empty() const noexcept { return rep.empty(); }

    public:
        // 修改器
        
        /**
         *  @brief  清除unordered_set所有元素
         */
        void clear() noexcept { rep.clear(); }

        /**
         *  @brief  插入元素x
         */
        pair<iterator, bool> insert(const value_type& x)
        {
            pair<typename Hashtable::iterator, bool> p = rep.insert_unique(x);
            return pair<iterator, bool>(p.first, p.second);
        }

        /**
         *  @brief  插入来自范围[first, last)的元素
         */ 
        template <class InputIterator>
        void insert(InputIterator first, InputIterator last)
        { rep.insert_unique(first, last); }

        /**
         *  @brief  插入来自initializer_list的元素
         */
        void insert(std::initializer_list<value_type> l)
        { rep.insert_unique(l.begin(), l.end(), STL::random_access_iterator_tag()); }

        /**
         *  @brief  移除位于pos的元素
         *  @return  被移除元素的下一个节点的迭代器
         */
        iterator erase(const_iterator pos)
        { return iterator(rep.erase(pos)); }

        /**
         *  @brief  移除范围[first, last)中的元素
         *  @return  last迭代器
         *
         *  必须是*this中的合法范围
         */
        iterator erase(const_iterator first, const_iterator last)
        { return iterator(rep.erase(first, last)); }

        /**
         *  @brief  移除键值等于k的元素
         *  @return  移除的节点个数
         */
        size_type erase(const key_type& k)
        { return rep.erase(k); }

        /**
         *  @brief  与另一个unordered_set交换数据
         */
        void swap(unordered_set& x)
        { rep.swap(x.rep); }

    public:
        // 查找
        
        /**
         *  @brief  返回键值为k的元素个数
         */
        size_type count(const key_type& k) const 
        { return rep.count(k); }

        /**
         *  @brief  返回指向键值为k的元素的迭代器
         */
        iterator find(const key_type& k) const 
        { return rep.find(k); }

        /**
         *  @brief  查找hashtable中键值为k的节点范围
         *  @return  pair<iterator, iterator>
         *           第一个指向范围的首节点
         *           第二个指向范围的尾后一位节点
         */
        pair<iterator, iterator> equal_range(const key_type& k) const 
        { return rep.equal_range(k); }

    public:
        // 桶接口
        size_type bucket_count() const { return rep.bucket_count(); }
        size_type max_bucket_count() const { return rep.max_bucket_count(); }

        /**
         *  @brief  第n个桶中的元素个数
         */
        size_type bucket_size(size_type n) const { return rep.bucket_size(n); }

    public:
        // 哈希策略
        
        /**
         *  @brief  负载系数
         */
        float load_factor() const noexcept { return rep.load_factor(); }

    public:
        // 观察器
        
        /**
         *  @brief  返回哈希函数
         */
        hasher hash_function() const { return rep.hash_function(); }

        /**
         *  @brief  返回键值相等性函数
         */
        key_equal key_eq() const { return rep.key_eq(); }

    public:
        // 比较符
        template <class _Value, class _HashFcn, class _EqualKey, class _Alloc>
        friend bool operator==(const unordered_set<_Value, _HashFcn, _EqualKey, _Alloc>& x,
                               const unordered_set<_Value, _HashFcn, _EqualKey, _Alloc>& y);
        template <class _Value, class _HashFcn, class _EqualKey, class _Alloc>
        friend bool operator!=(const unordered_set<_Value, _HashFcn, _EqualKey, _Alloc>& x,
                               const unordered_set<_Value, _HashFcn, _EqualKey, _Alloc>& y);
    };

    template <class Value, class HashFcn, class EqualKey, class Alloc>
    inline bool operator==(const unordered_set<Value, HashFcn, EqualKey, Alloc>& x,
                           const unordered_set<Value, HashFcn, EqualKey, Alloc>& y)
    { return x.rep == y.rep; }

    template <class Value, class HashFcn, class EqualKey, class Alloc>
    inline bool operator!=(const unordered_set<Value, HashFcn, EqualKey, Alloc>& x,
                           const unordered_set<Value, HashFcn, EqualKey, Alloc>& y)
    { return x.rep != y.rep; }

} /* namespace end */

#endif
