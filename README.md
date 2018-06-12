# TinySTL

## 项目简介

&ensp;&ensp;基于`C++11`的简易STL标准库，主要参考侯捷老师《STL源码剖析》的`SGI`版本以及我的开发环境`gcc 5.4.0`的`libstdc++`版本，实现了大部分STL中的容器与函数。
&ensp;&ensp;旨在学习常用数据结构与算法以及C++ template编程，使用中文文档与中文注释，忽略掉一些针对多线程优化的代码和异常处理，仍存在许多不足与bug。

## 实现进度

### 功能模块

1. allocator：空间分配器，包括malloc_alloc和pool_alloc

2. construct：标准构造/析构函数

3. type_traits：POD与非POD类型萃取器，基于g++5.4.0中的integral_constant等编译器内置类型计算

4. iterator：迭代器萃取器以及迭代器全局函数

5. uninitialized：内存初始化函数

6. vector

7. list

8. 基于rb_tree的set和map
  - 红黑树删除操作详细分析写在[博客](https://ysw1912.github.io/post/cc++/stl01/)

9. 基于hashtable

10. algorithm：泛型函数

### 测试模块

1. test_vector

2. test_list

3. test_tree

4. test_hashtable

### 待开发模块

1. deque

2. heap

