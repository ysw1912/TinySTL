# TinySTL

## 项目简介

&emsp;&emsp;基于`C++11`的简易STL标准库，主要参考侯捷老师《STL源码剖析》的`SGI`版本以及我的开发环境`gcc 5.4.0`的`libstdc++`版本，实现了大部分STL中的容器与函数。实现过程中的一些收获，主要是《STL源码剖析》中没有提及的部分，会写在下方。

&emsp;&emsp;本项目旨在学习常用数据结构与算法以及 C++ template 编程，使用中文文档与中文注释，忽略掉一些针对多线程优化的代码和异常处理，仍存在许多不足与bug，会一直进行维护，修复发现的bug。

## 实现进度

### 功能模块

1. `allocator.h`：空间分配器，包括 malloc_alloc 和 pool_alloc

2. `construct.h`：标准构造/析构函数

3. `type_traits.h`：POD与非POD类型萃取器，基于`g++ 5.4.0`中的 integral_constant 等编译器内置类型计算

4. `iterator.h`：迭代器萃取器以及迭代器全局函数

5. `uninitialized.h`：内存初始化函数

6. `vector.h`

7. `list.h`
    - [链表排序](https://ysw1912.github.io/post/cc++/stl02/)

8. 基于`tree.h`的`set.h`和`map.h`
    - [红黑树删除节点](https://ysw1912.github.io/post/cc++/stl01/)

9. 基于`hashtable.h`的`unordered_set.h`和`unordered_map.h`

10. `algorithm.h`：泛型函数

### 测试模块

1. `test_vector.cpp`

2. `test_list.cpp`

3. `test_tree.h`

4. `test_hashtable.cpp`

### 待开发模块

1. `deque.h`

2. `heap.h`

