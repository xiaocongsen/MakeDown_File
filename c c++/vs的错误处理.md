# error C2589: “(”: “::”右边的非法标记

```c++
size.Width = std::max(size.Width, elementSize.Width);  
改为：
size.Width = (std::max)(size.Width, elementSize.Width);  
```

# error C2061: 语法错误: 标识符“noexcept”
解决方案是：
将创建的项目后缀.c改为.cpp