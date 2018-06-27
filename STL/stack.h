#ifndef TINYSTL_STACK_H_
#define TINYSTL_STACK_H_ 

#include "deque.h"

namespace STL 
{
    template <class T, class Sequence = STL::deque<T>>
    class stack 
    {
        template <class T1, class Seq1>
        friend bool operator==(const stack<T1, Seq1>& x, const stack<T1, Seq1>& y);

        template <class T1, class Seq1>
        friend bool operator!=(const stack<T1, Seq1>& x, const stack<T1, Seq1>& y);

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
        explicit stack(const Sequence& _c)
        : c(_c) { }

        explicit stack(Sequence&& _c = Sequence())
        : c(std::move(_c)) { }

    public:
        // 元素访问
        reference top() { return c.back(); }
        const_reference top() const { return c.back(); }

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
        { c.pop_back(); }

        void swap(stack& s)
        { c.swap(s.c); }

    };

    template <class T, class Seq>
    inline bool operator==(const stack<T, Seq>& x, const stack<T, Seq>& y)
    { return x.c == y.c; }

    template <class T, class Seq>
    inline bool operator!=(const stack<T, Seq>& x, const stack<T, Seq>& y)
    { return !(x == y); }

}

#endif
