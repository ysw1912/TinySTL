#ifndef TINYSTL_QUEUE_H_
#define TINYSTL_QUEUE_H_ 

#include "deque.h"
#include "heap.h"
#include "vector.h"

namespace STL 
{
    /**
     *  @brief  FIFO队列
     */ 
    template <class T, class Sequence = STL::deque<T>>
    class queue 
    {
        template <class T1, class Seq1>
        friend bool operator==(const queue<T1, Seq1>& x, const queue<T1, Seq1>& y);

        template <class T1, class Seq1>
        friend bool operator!=(const queue<T1, Seq1>& x, const queue<T1, Seq1>& y);

    public:
        using value_type        = typename Sequence::value_type;
        using reference         = typename Sequence::reference;
        using const_reference   = typename Sequence::const_reference;
        using size_type         = typename Sequence::size_type;
        
    protected:
        Sequence c;     // 底层容器

    public:
        // The big five
        
        /**
         *  @brief  constructor
         */ 
        explicit queue(const Sequence& _c)
        : c(_c) { }

        explicit queue(Sequence&& _c = Sequence())
        : c(std::move(_c)) { }

    public:
        // 元素访问
        reference front() { return c.front(); }
        const_reference front() const { return c.front(); }
        reference back() { return c.back(); }
        const_reference back() const { return c.back(); }

    public:
        // 容量
        bool empty() const { return c.empty(); }
        size_type size() const { return c.size(); }

    public:
        // 修改器
        void push(const value_type& x)
        { c.push_back(x); }

        void push(value_type&& x)
        { c.push_back(std::move(x)); }

        template <class... Args>
        void emplace(Args&&... args)
        { c.emplace_back(std::forward<Args>(args)...); }
        
        void pop()
        { c.pop_front(); }

        void swap(queue& s)
        { c.swap(s.c); }

    };

    template <class T, class Seq>
    inline bool operator==(const queue<T, Seq>& x, const queue<T, Seq>& y)
    { return x.c == y.c; }

    template <class T, class Seq>
    inline bool operator!=(const queue<T, Seq>& x, const queue<T, Seq>& y)
    { return !(x == y); }

    /**
     *  @brief  优先权队列
     */ 
    template <class T, class Sequence = STL::vector<T>,
              class Compare = std::less<T> >
    class priority_queue
    {
    public:
        using value_type        = typename Sequence::value_type;
        using reference         = typename Sequence::reference;
        using const_reference   = typename Sequence::const_reference;
        using size_type         = typename Sequence::size_type;
        
    protected:
        Sequence c;     // 底层容器
        Compare cmp;

    public:
        // The big five
        
        /**
         *  @brief  constructor
         */
        explicit priority_queue(const Compare& _cmp, const Sequence& _c)
        : c(_c), cmp(_cmp)
        { STL::make_heap(c.begin(), c.end(), cmp); }

        explicit priority_queue(const Compare& _cmp = Compare(), Sequence&& _c = Sequence())
        : c(std::move(_c)), cmp(_cmp)
        { STL::make_heap(c.begin(), c.end(), cmp); }

        template <class InputIterator>
        priority_queue(InputIterator first, InputIterator last, 
                       const Compare& _cmp, const Sequence& _c)
        : c(_c), cmp(_cmp)
        {
            c.insert(c.end(), first, last);
            STL::make_heap(c.begin(), c.end(), cmp);
        }

        template <class InputIterator>
        priority_queue(InputIterator first, InputIterator last, 
                       const Compare& _cmp = Compare(), Sequence&& _c = Sequence())
        : c(std::move(_c)), cmp(_cmp)
        {
            c.insert(c.end(), first, last);
            STL::make_heap(c.begin(), c.end(), cmp);
        }
        
    public:
        // 元素访问
        const_reference top() const { return c.front(); }

    public:
        // 容量
        bool empty() const { return c.empty(); }
        size_type size() const { return c.size(); }

    public:
        // 修改器
        void push(const value_type& x)
        {
            try {
                c.push_back(x);
                STL::push_heap(c.begin(), c.end(), cmp);
            } catch(...) {
                c.clear();
                throw;
            }
        }

        void push(value_type&& x)
        {
            try {
                c.push_back(std::move(x));
                STL::push_heap(c.begin(), c.end(), cmp);
            } catch(...) {
                c.clear();
                throw;
            }
        }

        template <class... Args>
        void emplace(Args&&... args)
        {
            try {
                c.emplace_back(std::forward<Args>(args)...);
                STL::push_heap(c.begin(), c.end(), cmp);
            } catch(...) {
                c.clear();
                throw;
            }
        }

        void pop()
        {
            try {
                STL::pop_heap(c.begin(), c.end(), cmp);
                c.pop_back();
            } catch(...) {
                c.clear();
                throw;
            }
        }

        void swap(priority_queue& pq)
        {
            c.swap(pq.c);
            STL::swap(cmp, pq.cmp);
        }

    };

}

#endif
