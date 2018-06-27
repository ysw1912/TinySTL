#ifndef TINYSTL_QUEUE_H_
#define TINYSTL_QUEUE_H_ 

#include "deque.h"
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

    };

}

#endif
