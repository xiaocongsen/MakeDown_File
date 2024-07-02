# win下mingw编译的程序生成的dmp文件查看

## 条件

1. 需要debug版本
2. 需要安装vistual stutio
3. 需要winDbg
4. 需要cv2pdb

## 流程

- 命令行输入 cv2pdb.exe debugExe.exe 生成对应的.pdb
- 打开 winDbg 设置symbol file path 即pdb文件目录
- 设置source file path    源码目录
- 设置Image file Path     可执行程序目录
- 打开 crach Dump         即dmp文件
- 进入符号模糊匹配模式：!sym noisy on
- 加载pdb：.reload /i
- 自动化分析：!analyze -v
- 等待打印完成