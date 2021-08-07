警告： deleting object of polymorphic class type which has non_virtual destructor might cause undefine behavior
如果不定义虚析构，就会基类指针直接析构基类。这样派生类对象销毁不完整。所以编译器会告诉你，警告你的

警告： comparison between signed and unsigned integer expressions
有符号和无符号整数表达式之间的比较
