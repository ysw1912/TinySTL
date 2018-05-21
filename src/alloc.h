#ifndef TINYSTL_ALLOC_H_
#define TINYSTL_ALLOC_H 

#include <cstdlib>
#include <new> 

#ifndef THROW_BAD_ALLOC 
#define THROW_BAD_ALLOC throw std::bad_alloc 
#endif 

namespace STL {

    // 一级配置器
    class malloc_alloc {

    private:
        static void *oom_malloc(size_t);
        static void (*malloc_alloc_oom_handler)();
    public:
        // 直接使用malloc()
        static void * allocate(size_t n) {
            void *result = malloc(n); 
            if (0 == result)    result = oom_malloc(n); // 无法malloc，改用oom_malloc
            return result;
        }

        // 直接使用free()
        static void deallocate(void *p) {
            free(p);
        }

        // 仿真C++的set_new_handler()
        static void (*set_malloc_handler(void (*f)()))() {
            void (*old)() = malloc_alloc_oom_handler;
            malloc_alloc_oom_handler = f;
            return old;
        }
    };

    void (* malloc_alloc::malloc_alloc_oom_handler)() = 0;
    
    void * malloc_alloc::oom_malloc(size_t n) {
        void (*my_malloc_handler)();
        void *result;

        for (;;) {
            my_malloc_handler = malloc_alloc_oom_handler;
            if (0 == my_malloc_handler) { THROW_BAD_ALLOC; }
            (*my_malloc_handler)(); // 调用处理函数
            result = malloc(n);     // 再次尝试分配内存
            if (result) return result;
        }
    }

    enum { ALIGN = 8 }; // 小型区块的上调边界
    enum { MAX_BYTES = 128 };   // 小型区块的上限
    enum { LEN_FREE_LIST = MAX_BYTES / ALIGN };  // free_list节点个数

    // free_lists节点
    union FreeNode {
        union FreeNode *next;   // 指向下一个区块
        char data[1];           // 本快内存首地址
    };

    // 二级配置器
    class default_alloc {

    private:
        // 将bytes上调至ALIGN的倍数
        static size_t ROUND_UP(size_t bytes) {
            return (bytes + ALIGN - 1) & ~(ALIGN - 1);
        }

        // 16个节点的free_list
        static FreeNode * volatile free_list[LEN_FREE_LIST];

        // 根据bytes大小，决定使用第n号free_list
        static size_t FREE_LIST_INDEX(size_t bytes) {
            return (bytes + ALIGN - 1) / ALIGN - 1;
        }

        // 返回大小为n的对象，并可能加入大小为n的其它区块到free_list 
        static void *refill(size_t n);

        // 配置一大块空间，可容纳n_nodes个大小为size的区块
        // 弱空间不足，n_nodes可能会降低
        static char *chunk_alloc(size_t size, int &n_nodes);

        // 内存池状态
        static char *start;     // 内存池起始位置
        static char *end;       // 内存池结束位置
        static size_t heap_size;

    public:
        static void * allocate(size_t n);
        static void deallocate(void *p, size_t n);
    };
    
    // 初值
    char *default_alloc::start = nullptr;
    char *default_alloc::end = nullptr;
    size_t default_alloc::heap_size = 0;
    FreeNode* volatile default_alloc::free_list[LEN_FREE_LIST] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    };

    void * default_alloc::allocate(size_t n) {
        // 大于MAX_BYTES就调用一级配置器
        if (n > static_cast<size_t>(MAX_BYTES)) {
            return malloc_alloc::allocate(n);
        }
        // 寻找16个free_lists中适当的free_list
        FreeNode *my_free_list = free_list[FREE_LIST_INDEX(n)];
        FreeNode *result = my_free_list;
        if (nullptr == result) {
            // 没找到可用的free_list，将n上调，重新填充并返回
            return refill(ROUND_UP(n));
        }
        my_free_list = result->next;
        return result;
    };

    void default_alloc::deallocate(void *p, size_t n) {
        // 大于MAX_BYTES就调用一级配置器
        if (n > static_cast<size_t>(MAX_BYTES)) {
            malloc_alloc::deallocate(p);
            return;
        }
        FreeNode *q = reinterpret_cast<FreeNode *>(p);
        FreeNode *my_free_list = free_list[FREE_LIST_INDEX(n)];
        q->next = my_free_list;
        my_free_list = q;
    }
   
    // free_list无可用时调用，为free_list填充空间
    // 新的空间取自内存池，取得20个新节点
    // 若内存池空间不足，则获得节点数会＜20
    void * default_alloc::refill(size_t n) {    // n已经上调至ALIGN的倍数
        int n_nodes = 20;
        // 取得n_nodes个区块作为free_list的新节点
        char *chunk = chunk_alloc(n, n_nodes);
        FreeNode *my_free_list;    
    }

    // 从内存池取空间给free_list使用
    char * default_alloc::chunk_alloc(size_t size, int &n_nodes) {
        char *result;
        size_t total_bytes = size * n_nodes;
        size_t left_bytes = end - start;    // 内存池剩余空间
        
        if (left_bytes >= total_bytes) {
            // 剩余空间满足需求量
            result = start;
            start += total_bytes;
            return result;
        } else if (left_bytes >= size) {
            // 剩余空间不满足需求量，但能提供大于等于1个区块
            n_nodes = left_bytes / size;    // 修改n_nodes
            total_bytes = size * n_nodes;
            result = start;
            start += total_bytes;
            return result;
        } else {
            // 剩余空间不够提供1个区块
            // 准备从堆中获取2倍需求量+附加量的空间
            size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4); 
            // 将内存池的残余零头配给free_list中适当的节点
            if (left_bytes > 0) {
                FreeNode *my_free_node = free_list[FREE_LIST_INDEX(left_bytes)];
                reinterpret_cast<FreeNode *>(start)->next = my_free_node;
                my_free_node = reinterpret_cast<FreeNode *>(start);
            }
            // 配置heap空间，补充内存池
            start = (char *)malloc(bytes_to_get);
            // heap空间不足，malloc失败
            if (nullptr == start) {
                

            }
        }

    }
}
#endif 
