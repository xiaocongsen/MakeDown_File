# 从C 转向C++

1. 尽量用const 和inline 而不用#define
  - const double ASPECT_RATIO = 1.653;
  - const char * const authorName = "Scott Meyers";
  - 类常量 static const int NUM_TURNS = 5;
  - #define max(a,b) ((a) > (b) ? (a) : (b)) 用 
   template<class T>
   inline const T& max(const T& a, const T& b)
    { return a > b ? a : b; }来替换 

2. 尽量用<iostream>而不用<stdio.h>, 性能要求高时用<stdio.h>。

3. 尽量用new 和delete 而不用malloc 和free
  -  两种方法给一个包含 10 个string 对象的数组分配空间，一个用malloc，另一个用new
  -  用malloc的没有调用构造 没有初始化
  -  在释放空间是用free也不会调用析构, 如果string自己分配了内存,调用free释放就会造成内存泄露

4. 尽量使用C++风格的注释
  -  C++的注释  //
  -  C的注释 /* */

# 内存管理
5. 对应的new 和delete 要采用相同的形式
   - new 单个 delete
   - new 数组 delete[]

6. 析构函数里对指针成员调用delete
   - delete NULL 是安全的
   - 一种避免必须删除指针成员的方法，即把这些成员用智能指针对象来代替，比如C++标准库里的auto_ptr。
  
7. 预先准备好内存不够的情况
   - 可以用set_new_handler(void (*new_handler)() ) 来设置new时内存不够的处理函数
   - operator new 不能满足内存分配请求时，new-handler 函数不只调用一次，而是不断重复，直至找到足够的内存。

