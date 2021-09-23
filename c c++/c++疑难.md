警告： deleting object of polymorphic class type which has non_virtual destructor might cause undefine behavior
如果不定义虚析构，就会基类指针直接析构基类。这样派生类对象销毁不完整。所以编译器会告诉你，警告你的

警告： comparison between signed and unsigned integer expressions
有符号和无符号整数表达式之间的比较


静态变量定义在头文件， 会导致每个文件保护这个头问题都会生成一个新变量，作用域在该文件内

C函数返回指针位数错误，截断问题
gcc编译器对本函数调用未声明的函数，都为强制将其返回值类型转为int类型，int在64bit系统中占4个字节，这样指针类型的返回值就会出现截断现象！