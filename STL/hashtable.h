#ifndef TINYSTL_HASHTABLE_H_
#define TINYSTL_HASHTABLE_H_ 

#include "allocator.h"
#include "iterator.h"
#include "vector.h"

using std::pair;

namespace STL
{

    template <class Value>
    struct hashtable_node
    {
        hashtable_node* next;
        Value val;
    };

    template <class Value, class Key, class HashFcn, class ExtractKey, class Equal, class Alloc = STL::pool_alloc>
    class hashtable;

    template <class Value, class Key, class HashFcn, class ExtractKey, class Equal, class Alloc>
    struct hashtable_iterator
    {
        using Node              = hashtable_node<Value>;
        using Hashtable         = hashtable<Value, Key, HashFcn, ExtractKey, Equal, Alloc>;
        using iterator          = hashtable_iterator<Value, Key, HashFcn, ExtractKey, Equal, Alloc>;

        using iterator_category = STL::forward_iterator_tag;
        using value_type        = Value;
        using size_type         = size_t;
        using difference_type   = ptrdiff_t;
        using pointer           = value_type*;
        using reference         = value_type&;
        
        Node* cur;      // 迭代器目前指向的节点
        Hashtable* ht;  // 保持对容器的连接

        hashtable_iterator() : cur(nullptr), ht(nullptr) { }
        hashtable_iterator(Node* n, Hashtable* tab) : cur(n), ht(tab) { }

        reference operator*() const { return cur->val; }
        pointer operator->() const { return &(operator*()); }
        
        iterator& operator++() 
        {
            const Node* old = cur;
            cur = cur->next;
            if (cur == nullptr) {
                // 根据元素值，定位出下一个bucket
                size_type bucket = ht->bkt_num(old->val);
                while (cur == nullptr && ++bucket < ht->buckets.size())
                    cur = ht->buckets[bucket];
            }
            return *this;
        }  

        iterator operator++(int) 
        {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

        bool operator==(const iterator& it) const { return cur == it.cur; }
        bool operator!=(const iterator& it) const { return cur != it.cur; }
    };

    template <class Value, class Key, class HashFcn, class ExtractKey, class Equal, class Alloc>
    struct hashtable_const_iterator 
    {
        using Node              = hashtable_node<Value>;
        using Hashtable         = hashtable<Value, Key, HashFcn, ExtractKey, Equal, Alloc>;
        using iterator          = hashtable_iterator<Value, Key, HashFcn, ExtractKey, Equal, Alloc>;
        using const_iterator    = hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, Equal, Alloc>;

        using iterator_category = STL::forward_iterator_tag;
        using value_type        = Value;
        using size_type         = size_t;
        using difference_type   = ptrdiff_t;
        using pointer           = const value_type*;
        using reference         = const value_type&;
        
        const Node* cur;      // 迭代器目前指向的节点
        const Hashtable* ht;  // 保持对容器的连接

        hashtable_const_iterator() : cur(nullptr), ht(nullptr) { }
        hashtable_const_iterator(const Node* n, const Hashtable* tab) : cur(n), ht(tab) { }
        hashtable_const_iterator(const iterator& it) : cur(it.cur), ht(it.ht) { }

        reference operator*() const { return cur->val; }
        pointer operator->() const { return &(operator*()); }
        
        const_iterator& operator++()
        {
            const Node* old = cur;
            cur = cur->next;
            if (cur == nullptr) {
                // 根据元素值，定位出下一个bucket
                size_type bucket = ht->bkt_num(old->val);
                while (cur == nullptr && ++bucket < ht->buckets.size())
                    cur = ht->buckets[bucket];
            }
            return *this;
        }  

        const_iterator operator++(int)
        {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

        bool operator==(const const_iterator& it) const { return cur == it.cur; }
        bool operator!=(const const_iterator& it) const { return cur != it.cur; }
    };

    enum { num_primes = 28 };
    static const unsigned long prime_list[num_primes] = {
        53,        97,        193,       389,       769,        1543,         3079,
        6151,      12289,     24593,     49157,     98317,      196613,       393241,
        786433,    1572869,   3145739,   6291469,   12582917,   25165843,     50331653,
        100663319, 201326611, 402653189, 805306457, 1610612741, 3221225437ul, 4294967291ul 
    };

    // 找出最接近并大于等于n的质数
    inline unsigned long next_prime(unsigned long n)
    {
        const unsigned long* first = prime_list;
        const unsigned long* last = prime_list + num_primes;
        const unsigned long* pos = STL::lower_bound(first, last, n);
        return pos == last ? *(last - 1) : *pos;
    }

    
    /**
     *  Hashtable模板类
     *
     *  @tparam  Value      节点的实值类型
     *  @tparam  Key        节点的键值类型
     *  @tparam  HashFcn    hahs函数类型
     *  @tparam  ExtractKey 从节点中取出Key的函数对象
     *  @tparam  Equal      判断键值是否相同的函数对象
     *  @tparam  Alloc      空间分配器
     *
     */ 
    template <class Value, class Key, class HashFcn,
              class ExtractKey, class Equal, class Alloc>
    class hashtable
    {
        using Node              = hashtable_node<Value>;
        using Bucket_type       = STL::vector<Node*, Alloc>;

    public:
        using value_type        = Value;
        using key_type          = Key;
        using key_equal         = Equal;
        using hasher            = HashFcn;
        
        using size_type         = size_t;
        using difference_type   = ptrdiff_t;
        using pointer           = value_type*;
        using const_pointer     = const value_type*;
        using reference         = value_type&;
        using const_reference   = const value_type&;

    private:
        using node_allocator    = STL::allocator<Node, Alloc>;

    protected:
        Node* get_node() { return node_allocator::allocate(); }
        void put_node(Node* p) { node_allocator::deallocate(p); }

    private:
        hasher      hash;
        key_equal   equal;
        ExtractKey  get_key;
        Bucket_type buckets;
        size_type   num_elements;

    public:
        using iterator          = hashtable_iterator<value_type, key_type, hasher, ExtractKey, key_equal, Alloc>;
        using const_iterator    = hashtable_const_iterator<value_type, key_type, hasher, ExtractKey, key_equal, Alloc>;
        // 使迭代器能访问hashtable的私有数据成员
        friend struct hashtable_iterator<value_type, key_type, hasher, ExtractKey, key_equal, Alloc>;
        friend struct hashtable_const_iterator<value_type, key_type, hasher, ExtractKey, key_equal, Alloc>;

    protected:
        // 创建节点 = 分配内存 + 构造节点
        template <class... Args>
        Node* create_node(Args&&... args)
        {
            Node* n = get_node();
            n->next = nullptr;
            try {
                STL::construct(&n->val, std::forward<Args>(args)...);
                return n;
            } catch(...) {
                put_node(n);
            }
        }

        // 删除节点 = 析构节点 + 释放内存
        void drop_node(Node* n)
        {
            STL::destroy(&n->val);
            put_node(n);
        }

        // 初始化next_prime(n)个桶，将其全部填0 
        void initialize_buckets(size_type n)
        {
            const size_type n_buckets = next_prime(n);
            buckets.reserve(n_buckets);
            buckets.insert(buckets.end(), n_buckets, static_cast<Node*>(nullptr));
            num_elements = 0;
        }

        // 复制
        void copy_from(const hashtable& ht)
        {
            buckets.clear();    // 请空buckets vector 
            buckets.reserve(ht.buckets.size());
            buckets.insert(buckets.end(), ht.buckets.size(), static_cast<Node*>(nullptr));
            try {
                for (size_type i = 0; i < ht.buckets.size(); ++i) {
                    if (const Node* cur = ht.buckets[i]) {  // 当前桶的链表不为空（有节点）
                        Node* copy = create_node(cur->val);
                        buckets[i] = copy;
                        // 复制整个链表
                        for (Node* next = cur->next; next; cur = next, next = cur->next) {
                            copy->next = create_node(next->val);
                            copy = copy->next;
                        }
                    }
                }
                num_elements = ht.num_elements;
            } catch(...) {
                clear();
                throw;
            }
        }

        // 判断元素的落脚处
        // 版本一 接受实值x和桶数n
        size_type bkt_num(const value_type& x, size_type n) const 
        { return bkt_num_key(get_key(x), n); }
        // 版本二 接受实值x
        size_type bkt_num(const value_type& x) const 
        { return bkt_num_key(get_key(x)); }
        // 版本三 接受键值k
        size_type bkt_num_key(const key_type& k) const 
        { return bkt_num_key(k, buckets.size()); }
        // 版本四 接受实值和桶数
        size_type bkt_num_key(const key_type& k, size_type n) const 
        { return hash(k) % n; }
        

    public:
        // The big five
        
        /**
         *  @brief  constructor
         */
        hashtable(size_type n)
        : hash(HashFcn()), equal(Equal()), get_key(ExtractKey()), num_elements(0) 
        { initialize_buckets(n); }

        hashtable(size_type n, const HashFcn& hf, const Equal& eql)
        : hash(hf), equal(eql), get_key(ExtractKey()), num_elements(0) 
        { initialize_buckets(n); }

        hashtable(size_type n, const HashFcn& hf, const Equal& eql, const ExtractKey& ext)
        : hash(hf), equal(eql), get_key(ext), num_elements(0) 
        { initialize_buckets(n); }

        /**
         *  @brief  copy constructor
         */ 
        hashtable(const hashtable& ht)
        : hash(ht.hash), equal(ht.equal), get_key(ht.get_key), num_elements(0)
        { copy_from(ht); }

        /**
         *  @brief  copy assignment
         */
        hashtable& operator=(const hashtable& ht)
        {
            if (this != &ht) {
                clear();
                hash = ht.hash;
                equal = ht.equal;
                get_key = ht.get_key;
                copy_from(ht);
            }
            return *this;
        }

        /**
         *  @brief  destructor
         */
        ~hashtable() { clear(); }

    protected:
        // 判断是否需要重建table
        // 将节点个数（计入新增节点）与bucket vector的大小对比，若前者大于后者，则重建table
        // 因此每个bucket（链表）的最大容量与bucket vector的大小相同
        void resize(size_type num_elements_hint)
        {
            const size_type old_n = buckets.size();
            if (num_elements_hint > old_n) {    // 需要重新配置table
                const size_type n = next_prime(num_elements_hint);
                if (n > old_n) {
                    Bucket_type tmp(n, static_cast<Node*>(nullptr));
                    try {
                        for (size_type bid = 0; bid < old_n; ++bid) {
                            Node* first = buckets[bid];
                            // 处理每个旧bucket所含的每个节点
                            while (first) {
                                size_type new_bucket = bkt_num(first->val, n);  // 找出节点落在哪个新bucket内
                                // (1) 令旧bucket指向下一个节点
                                buckets[bid] = first->next;
                                // (2)(3) 将当前节点first前插进新bucket中
                                first->next = tmp[new_bucket];
                                tmp[new_bucket] = first;
                                // (4) 回到旧bucket的下一个待处理节点处
                                first = buckets[bid];
                            }
                        }
                        buckets.swap(tmp);
                    } catch(...) {
                        // 若操作失败，则删除新hashtable内的所有节点
                        for (size_type bid = 0; bid < tmp.size(); ++bid) {
                            while (tmp[bid]) {
                                Node* next = tmp[bid]->next;
                                drop_node(tmp[bid]);
                                tmp[bid] = next;
                            }
                        }
                        throw;
                    }
                }
            }
        }
    
    public:
        // 迭代器
        iterator begin()
        {
            // 找到第一个非空的桶，获取第一个节点
            for (size_type i = 0; i < buckets.size(); ++i)
                if (buckets[i])
                    return iterator(buckets[i], this);
            return end();
        } 
        
        const_iterator begin() const
        {
            for (size_type i = 0; i < buckets.size(); ++i)
                if (buckets[i])
                    return const_iterator(buckets[i], this);
            return cend();
        }

        const_iterator cbegin() const { return const_iterator(begin()); }

        iterator end() { return iterator(nullptr, this); }

        const_iterator end() const { return const_iterator(nullptr, this); }

        const_iterator cend() const { return const_iterator(nullptr, this); }

    public:
        // 容量
        size_type size() const { return num_elements; }
        size_type max_size() const { return node_allocator::max_size(); }
        bool empty() const { return num_elements == 0; }
       
    protected:
        // 不需要重建table的情况下插入新节点，键值不允许重复
        pair<iterator, bool> insert_unique_noresize(const value_type& x)
        {
            const size_type n = bkt_num(x);     // x应位于#n bucket 
            Node* first = buckets[n];
            // buckets[n]被占用
            for (Node* cur = first; cur; cur = cur->next) {
                if (equal(get_key(cur->val), get_key(x)))   // 若与链表中某键值相同，则立即返回
                    return pair<iterator, bool>(iterator(cur, this), false);
            }
            // 未进入循环（buckets[n]未被占用）离开循环（无重复键值）
            Node* tmp = create_node(x);
            tmp->next = first;
            buckets[n] = tmp;   // 创建新节点前插进buckets[n]的链表头
            ++num_elements;
            return pair<iterator, bool>(iterator(tmp, this), true);
        }

        // 不需要重建table的情况下插入新节点，键值允许重复
        iterator insert_equal_noresize(const value_type& x)
        {
            const size_type n = bkt_num(x);     // x应位于#n bucket 
            Node* first = buckets[n];
            // buckets[n]被占用
            for (Node* cur = first; cur; cur = cur->next) {
                if (equal(get_key(cur->val), get_key(x))) {     // 若与链表中某键值相同，则立即插入
                    Node* tmp = create_node(x);
                    tmp->next = cur->next;
                    cur->next = tmp;    // 将新节点插入重复节点之后
                    ++num_elements;
                    return iterator(tmp, this);
                }
            }
            // 未发现重复的键值
            Node* tmp = create_node(x);
            tmp->next = first;
            buckets[n] = tmp;   // 创建新节点前插进buckets[n]的链表头
            ++num_elements;
            return iterator(tmp, this);
        }

        // 插入来自范围[first, last)的元素，键值不允许重复
        // input_iterator版本
        template <class InputIterator>
        void insert_unique(InputIterator first, InputIterator last, STL::input_iterator_tag) 
        {
            for ( ; first != last; ++first)
                insert_unique(*first);
        } 
        // forward_iterator版本
        template <class ForwardIterator>
        void insert_unique(ForwardIterator first, ForwardIterator last, STL::forward_iterator_tag) 
        {
            size_type n = STL::distance(first, last);
            resize(num_elements + n);
            for ( ; n; --n, ++first)
                insert_unique_noresize(*first);
        }

        // 插入来自范围[first, last)的元素，键值允许重复
        // input_iterator版本
        template <class InputIterator>
        void insert_equal(InputIterator first, InputIterator last, STL::input_iterator_tag) 
        {
            for ( ; first != last; ++first)
                insert_equal(*first);
        } 
        // forward_iterator版本
        template <class ForwardIterator>
        void insert_equal(ForwardIterator first, ForwardIterator last, STL::forward_iterator_tag) 
        {
            size_type n = STL::distance(first, last);
            resize(num_elements + n);
            for ( ; n; --n, ++first)
                insert_equal_noresize(*first);
        }

    public:
        // 修改器
        
        /**
         *  @brief  清除hashtable所有元素
         */ 
        void clear() 
        {
            for (size_type i = 0; i < buckets.size(); ++i) {
                Node* cur = buckets[i];
                while (cur) {
                    Node* next = cur->next;
                    drop_node(cur);
                    cur = next;
                }
                buckets[i] = nullptr;
            }
            num_elements = 0;
            // buckets vector并未释放空间
        }
        
        /**
         *  @brief  插入元素x，不允许重复
         *  @return  pair<iterator, bool>
         *           iterator  若成功，指向新增节点；若失败，指向重复节点
         *           bool      插入是否成功
         */  
        pair<iterator, bool> insert_unique(const value_type& x) 
        {
            resize(num_elements + 1);
            return insert_unique_noresize(x);
        }

        /**
         *  @brief   插入来自范围[first, last)的元素，键值不允许重复
         */ 
        template <class InputIterator>
        void insert_unique(InputIterator first, InputIterator last)
        { insert_unique(first, last, STL::iterator_category(first)); }

        /**
         *  @brief  插入元素x，允许重复
         *  @return  iterator  指向新增节点
         */
        iterator insert_equal(const value_type& x) 
        {
            resize(num_elements + 1);
            return insert_equal_noresize(x);
        }

        /**
         *  @brief   插入来自范围[first, last)的元素，键值允许重复
         */ 
        template <class InputIterator>
        void insert_equal(InputIterator first, InputIterator last)
        { insert_equal(first, last, STL::iterator_category(first)); }

        /**
         *  @brief  移除位于pos的节点
         */ 
        iterator erase(const_iterator pos) 
        {
            return iterator(0, this);
        }

        /**
         *  @brief  移除范围[first, last)中的节点
         *
         *  必须是*this中的合法范围
         */
        iterator erase(const_iterator first, const_iterator last) 
        {
        
            return iterator(0, this);
        }

        /**
         *  @brief  移除键值等于k的节点
         *  @return  移除的节点个数
         */
        size_type erase(const key_type& k) 
        {
            const size_type n = bkt_num_key(k);
            Node** first = &buckets[n]; // 使用二级指针避免对头结点的另行处理
            size_type erased = 0;
            while (*first) {
                Node* cur = *first;
                if (equal(get_key(cur->val), k)) {
                    *first = cur->next;
                    drop_node(cur);
                    ++erased;
                    --num_elements;
                } else 
                    first = &(cur->next);
            }
            return erased;
        }

        /**
         *  @brief  与另一个hashtable交换数据
         */ 
        void swap(hashtable& ht) 
        {
            STL::swap(hash, ht.hash);
            STL::swap(equal, ht.equal);
            STL::swap(get_key, ht.get_key);
            buckets.swap(ht.buckets);
            STL::swap(num_elements, ht.num_elements);
        }

    public:
        // 查找
        
        /**
         *  @brief  返回键值为k的节点个数
         */
        size_type count(const key_type& k) const 
        {
            const size_type n = bkt_num_key(k);
            size_type result = 0;
            for (const Node* cur = buckets[n]; cur; cur = cur->next)
                if (equal(get_key(cur->val), k))
                    ++result;
            return result;
        }

        /**
         *  @brief  返回指向键值为k的节点的迭代器
         */ 
        iterator find(const key_type& k) 
        {
            const size_type n = bkt_num_key(k);
            Node* first;
            for (first = buckets[n];
                 first && !equal(get_key(first->val), k);
                 first = first->next) { }
            return iterator(first, this);
        }

    public:
        // 桶接口
        size_type bucket_count() const { return buckets.size(); }
        size_type max_bucket_count() const { return prime_list[num_primes - 1]; }

        /**
         *  @brief  第n个桶中的节点个数
         */ 
        size_type bucket_size(size_type n) const 
        {
            size_type result = 0;
            for (Node* cur = buckets[n]; cur; cur = cur->next)
                ++result;
            return result;
        }
    };

} /* namespace STL */

#endif
