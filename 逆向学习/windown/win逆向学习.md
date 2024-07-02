## 最快的学习方法
1. CreckMe练习
2. 多参加CTF大赛

什么是关键点 程序注册判断注册码是否正确的位置

什么是爆破 通过对程序流程的修改，让程序判断失误


# OllyDbg使用
L       日志
E       模块信息
M       内存
T       线程
W       窗口信息
H       句柄信息
C       CPU 代码 寄存器 内存等信息
/       补丁
K       调用堆栈
B       断点窗口
R       参考
...     run跟踪
S       Source

## 快捷键
    插件            查找函数表达式 CTRL+G 输入GetWindowTextA 点击ok 跳转到GetWindowTextA的代码上
    断点            F2或者双击字节内容
    条件断点        shift+F2 (esp+4) != 002906242
    F8              运行一条指令
    F7              进入call
    -               回退到上次运行位置，单步回退单步，多步回退多部，不是真的回退
    Ctrl+z          撤销修改

# 查找关键点
1. api断点法
    GetWindowTextA
    SetWindowTextA
    MessageBoxA
    GetVersion  壳吐出代码后大概率调用这个 
    CreateWindowExA  壳吐出代码后调用窗体创建函数
    LoadLibraryA    壳吐出代码后调用加载库函数
2. 字符串查找法
    加壳的需要跳转到00401000再点击智能搜索
3. 交叉引用查找法
    通过ide找到字符串再代码中的引用 (快捷键x是查看引用) 可以找到引用的代码地址，通过od去断点
4. ALT+K暂停法
    运行弹出失败对话框后，点击暂停，然后点击K按钮查看调用堆栈
5. 访问断点法
    点击M打开内存面板，内存查找字符串，右键断点 内存访问断点 或者硬件访问断点
6. 自动跟踪法-控制台程序
    将程序拖入od->点击自动步过->停下来的位置断点->点击(<<重来按钮)->点击运行->到达断点位置->取消断点->F7进入call->点击自动步过->重复

# 关键位修改方法
    nop掉指令
    改标志位

# 找程序入口点


# 经验
ESP == EBP  代表函数头部
je跳转到jmp指令后 为一个if(1==1){} else{}判断 
JZ/JE 如果EFL第6位零标志位的值为1，则跳转到名为LABEL的标签地址所在的位置


程序中大量jmp是增量链接技术(快速编译，只修改修改的链接)
关闭方法->Visual Studio->右键工程名->属性->链接器->所有选项->启用增量链接->改为否

当F8卡住了的时候 可以点击T按钮显示线程窗口->右击->Resume All Threads(恢复所有线程)


```
当看见这样类似的汇编时，为易语言典型的创建线程，并在线程中调用函数

push esi
push edi
push ebx
call xxx.004019b3
pop ebx
pop edi
pop esi
retn
通过堆栈找到创建线程的位置,就能找到线程的开始代码位置 既004019b3


怎么跳转到主程序 右击push esi->查找参考->选定命令->选择不是红色的点击进入 就可以看到哪里引用了当前这个线程 (Ctrl+R)
```

```
在代码的开始位置00401000搜索(test edx,3)为易语言字符串比较的特征码。

004010FA    8B5424 04       mov edx,dword ptr ss:[esp+0x4]
004010FE    8B4C24 08       mov ecx,dword ptr ss:[esp+0x8]
00401102    85D2            test edx,edx
00401104    75 0D           jnz short 第五课课.00401113
00401106    33C0            xor eax,eax
00401108    85C9            test ecx,ecx
0040110A    74 06           je short 第五课课.00401112
0040110C    8039 00         cmp byte ptr ds:[ecx],0x0
0040110F    74 01           je short 第五课课.00401112
00401111    48              dec eax
00401112    C3              retn
00401113    85C9            test ecx,ecx
00401115    75 09           jnz short 第五课课.00401120
00401117    33C0            xor eax,eax
00401119    803A 00         cmp byte ptr ds:[edx],0x0
0040111C    74 01           je short 第五课课.0040111F
0040111E    40              inc eax
0040111F    C3              retn
00401120    F7C2 03000000   test edx,0x3

```

如果多个模块有同一个函数，Ctrl+g后 需要加模块名.函数名

## 需要显示地址内存内容的 
    选择push xxx.地址 右击->数据窗口中跟随->立即数 内存就会跳转过去显示该地址内容

# 保存修改后的文件
OD 右击 复制到可执行文件->所有修改->全部复制 ->右击->保存文件



# 动态基地址
## 修改成固定地址
    CFF工具->打开exe->可选头部->DllCharacteristics->点击click here->去掉DLL can move->另存为  基地址就固定为可选头中->ImageBase的Value一般是00400000 代码起始位为00401000

## 编译的时候修改
    Visual Studio->右键工程名->属性->链接器->所有选项->随机基地址->改为否 

## Im.0046efb6插件的原理
    Edit label 就是给某个call到的地址设置标签，就是给某个函数取别名
    如果没插件就这么操作：先Ctrl+G输入地址跳转到call的地址,然后右键点击标签,输入别名
    需要在内存中看到别名: 先Ctrl+G输入地址跳转到内存位置，修改显示格式右击长型->ASCII数据地址



## od插件查找字符串的原理：
    查找指令 push mov lea后面的操作数是否是以\0结尾。 易语言中字符串不是单独的字符串是结构体，od的插件一般找不出来




## 怎么防止od和ide的字符串搜索
    将中文转换为十六进制,将十六进制赋值给数组。 
    不要写在全局变量中(全局变量编译到.data中 就被识别出字符串了)

## 局部变量
    分析代码后出现的local.1,eax local.2,0x0
    分析代码前是 dword ptr ss:[ebp - 0x4],eax  dowrd ptr ss:[ebp -0x8],0x0

## 解码修饰符合名称
    选项->调试设置->解码修饰符合名称


## od插件的bug
    硬件断点重新运行后才生效是因为IDA插件的问题导致的




# ida
    改完后无法撤销

## ida窗口解释
    上面导航条表示不同区域的块(代码区，数据区，常量区...)
    IDA View-A      反汇编窗口(全局对象选中类型alt+q可以修改ida识别不正确的类型为结构体，局部变量p)
    Hex View-A      当前文件的十六进制显示窗口
    Imports         当前程序使用了哪些模块和那些模块的哪些函数(根据导入表生成的)
    Exports         当前程序导出了什么函数(根据导出表生成的)
    Structures      ida识别出来的结构体 和 添加自己分析出来的结构体 (edit->add struct type) d添加成员 选中类型按d切换类型大小(数组的话右键选择Array,在改数量或者按*) 选择类型alt+q会显示所有识别出的结构体 选择名字按n修改成员名称
    Enums           识别出的枚举 和 添加自己分析出来的枚举
    
## 命名
    sub是ida识别出来的函数的命名
    loc

## 快捷键
    g       跳转到地址
    空格    切换显示图形还是文本结构
    a       把当前的数据按照字符串的形式显示
    c       把当前的数据按照代码的形式显示
    d       把当前的数据按照数据的形式显示  按一下1个字节，按两下2个字节 按三下4个字节 为单位
    u       把当前的数据按照原始字节来显示
    alt+t   搜索
    n       重命名
    ;       注释，引用的地方也会同步显示  选中函数既可写函数的注释
    shift+; 注释，引用的地方不会显示
    y       修改函数的声明
    exc     退回到上一步骤界面
    insert  结构体节目按这个是新建添加结构体节目
    p       将数据转换成函数
    F5      转换成c语言伪代码

## 修改字符串变量编码
选中变量->edit->String->Unicode

## 交叉引用查看方法
    选中函数或变量 view->Open subviews->Cross references

## 怎么修改和保存修改后的文件 (keypatch插件)
    g->输入修改的地址并跳转->点击edit(编辑)->Patch program(修补程序)->Assemble...(汇编)->输入NOP 
    点击edit(编辑)->Patch program(修补程序)->Apply patches to input file...(修补程序应用到输入文件)->起始地址修改为00401000->确定

## ida查找字符串的原理:
    通过pe文件静态搜索，有字符串的就显示出来

## 字符串显示异常处理
    点击异常字符串会跳转到存储字符串的数据区->选择字符串Text->Options->String literals...->Currently:->UTF-16LE

## 显示每条指令最多硬编码字节数(硬编码一行最多只有16)
    常规->操作码字节数(非图表) 0 ->改为16
    Opions->General->Number of opcode bytes->改为16

## ide修改字节
    edit->patch program-> change byte...


# CFF Explorer

## 程序加壳
    拖入->UPX Utility -> pack ->另存为
    加壳后修改无法保存，需要使用补丁工具修改内存 或者使用dll劫持
### 壳的介绍
    壳就是将代码压缩了，运行的时候再解压还原出来

### OEP 
    pe文件中的addressOfEntryPoint+ImageBase 为第一个执行的代码 这就是OEP

### 调试方法
    如果需要调试的函数在程序启动的时候调用 则可以在调用的函数位置下硬件写入断点 壳吐出代码的时候就会被断点

## 分析壳
    在系统dll上断点如：(kernel32 代码段) 然后去代码段断点，就可以知道壳代码调用了那个系统函数 并能找到壳代码进行分析 

# c语言实现注册机
## 函数名：OpenProcess
函数功能：打开一个已存在的进程对象，返回进程对象的句柄
参数：
dwDesiredAccess：指定进程对象的访问权限。可以是 PROCESS_ALL_ACCESS（最高权限）或者其他标志位，如 PROCESS_CREATE_PROCESS、PROCESS_QUERY_INFORMATION、PROCESS_VM_READ 等。更多访问权限请参考 Microsoft 官方文档。
bInheritHandle：指定是否继承进程句柄。如果为 TRUE，则新进程会继承该句柄，否则不会。如需要在新进程中访问当前进程中打开的某个资源时，才需要将其设置为 TRUE
dwProcessId：指定要打开的进程的 ID。
返回值：如果函数执行成功，则返回指定进程的句柄；如果执行失败，则返回 NULL。
函数说明：在 Windows 操作系统中，进程是系统资源的一个重要组成部分。OpenProcess 函数可以打开一个已存在的进程对象，并返回该进程的句柄。通过该句柄，我们可以访问该进程的资源，如内存、线程等。
注意事项：
要使用 OpenProcess 函数打开一个进程，必须具有足够的权限。
打开进程时，可以指定访问权限，以控制对进程的访问。
如果要访问一个 64 位进程，需要在 64 位应用程序中使用 OpenProcess 函数。
打开进程对象后，一定要记得使用 CloseHandle 函数关闭进程句柄，否则会导致资源泄漏。

## 函数名：WriteProcessMemory
函数功能：向另一个进程的虚拟地址空间中写入数据
参数：
hProcess：目标进程的句柄
lpBaseAddress：要写入数据的目标进程中的起始地址
lpBuffer：指向要写入的数据的指针
nSize：要写入的数据的字节数
lpNumberOfBytesWritten：实际写入的字节数，如果函数执行成功，则该参数返回写入的字节数。
返回值：如果函数执行成功，则返回 TRUE；如果执行失败，则返回 FALSE。
函数说明：WriteProcessMemory 函数用于向另一个进程的虚拟地址空间中写入数据。它需要调用进程的句柄以及要写入的数据和目标地址。
需要注意的是，写入数据的大小不能超过目标进程的虚拟地址空间大小，否则会导致访问非法内存并触发异常。此外，在写入数据之前，必须确保目标进程有足够的权限允许写入操作，否则也会导致失败。
参数 lpNumberOfBytesWritten 是一个输出参数，用于返回实际写入的字节数。如果函数执行成功，该参数将包含写入的字节数，如果执行失败，则该参数的值没有意义。
需要注意的是，WriteProcessMemory 函数只能写入数据，而不能改变目标进程中的程序代码。如果想要修改程序代码，可以通过修改目标进程的可执行映像文件或者注入 DLL 等方式实现。

``` c
#include <iostream>
#include <windows.h>
int main()
{
	int pId;
	printf("请输入要破解程序的进程ID:");
	scanf_s("%d", &pId);
	byte buff[] = { 0x90,0x90,0x90,0x90,0x90,0x90 };
	HANDLE hHandle=OpenProcess(PROCESS_ALL_ACCESS, TRUE, pId);		//打开一个进程 PROCESS_ALL_ACCESS流程对象的所有可能访问权限

	WriteProcessMemory(hHandle, (LPVOID)0x004034F6, buff, 6, NULL); //向另一个进程的虚拟地址空间中写入数据
	CloseHandle(hHandle);   //关闭进程句柄
}
```




# vs调试dll
    属性->调试->命令->添加一个使用这个dll的exe


``` c
CONTEXT ctx;
HANDLE H =  GetCurrentThread()  //检索调用线程的伪句柄
GetThreadContext(H, &ctx);      //检索指定线程的上下文
//用于存储寄存器 ctx
```



# 异常之VEH
## 函数名：AddVectoredExceptionHandler
函数功能：向当前进程添加一个异常处理程序
参数：
First：指定添加的异常处理程序是位于链表的开始位置还是结束位置。如果该参数为 1，则将新的异常处理程序添加到链表的开头，否则将其添加到链表的末尾。
Handler：指向要添加的异常处理程序的函数指针，该函数会在发生异常时被调用。
返回值：如果函数执行成功，则返回一个指针，指向先前设置的异常处理程序；如果执行失败，则返回 NULL。
函数说明：AddVectoredExceptionHandler 函数用于向当前进程中添加一个异常处理程序。异常处理程序是一段代码，它会在进程发生异常时被调用。通常情况下，异常处理程序用于捕获和处理未处理的异常，例如访问非法内存或者执行非法指令等。
该函数会将新的异常处理程序添加到一个链表中，该链表用于保存所有的异常处理程序。链表中的每个异常处理程序都会在进程发生异常时被依次调用，直到有一个异常处理程序成功处理了异常或者链表中的所有异常处理程序都被调用完毕为止。
参数 First 用于指定添加的异常处理程序是位于链表的开始位置还是结束位置。如果该参数为 1，则将新的异常处理程序添加到链表的开头，否则将其添加到链表的末尾。
参数 Handler 是一个函数指针，指向要添加的异常处理程序。该函数应该符合 PVECTORED_EXCEPTION_HANDLER 类型的定义，即它应该接受两个参数：指向 EXCEPTION_POINTERS 结构的指针和一个指向 CONTEXT 结构的指针。EXCEPTION_POINTERS 结构包含关于异常的信息，而 CONTEXT 结构包含当前线程的上下文信息。函数可以使用这些信息来处理异常。
需要注意的是，AddVectoredExceptionHandler 函数不会修改系统的异常处理程序，它只会向当前进程中添加一个自定义的异常处理程序。如果希望修改系统的异常处理程序，可以使用 SetUnhandledExceptionFilter 函数。

RemoveVectoredExceptionHandler 取消注册矢量异常处理程序。


## 函数名：KiUserExceptionDispatcher

函数功能：用于处理用户模式下的异常
参数：
ExceptionRecord：指向 EXCEPTION_RECORD 结构的指针，该结构包含关于异常的信息，例如异常代码、异常地址等。
ContextRecord：指向 CONTEXT 结构的指针，该结构包含当前线程的上下文信息，例如寄存器值、指令指针等。
返回值：该函数没有返回值。
函数说明：KiUserExceptionDispatcher 函数用于处理用户模式下的异常。当发生异常时，操作系统会将控制权转移到该函数，该函数会根据异常类型和处理程序的设置，决定如何处理该异常。
在 Windows 操作系统中，用户模式下的异常分为两类：C++ 异常和 SEH 异常。C++ 异常是由 C++ 程序抛出的异常，需要使用 try-catch 语句进行处理。SEH 异常是由系统或应用程序自己抛出的异常，可以使用 SetUnhandledExceptionFilter 函数或 AddVectoredExceptionHandler 函数来设置处理程序。
KiUserExceptionDispatcher 函数会先判断该异常是否为 C++ 异常。如果是，则会将控制权转移到 C++ 运行时库中的异常处理程序。如果不是，则会根据 SEH 异常的处理方式，将控制权转移到系统或者应用程序自己设置的异常处理程序。
需要注意的是，KiUserExceptionDispatcher 函数是一个内部函数，不应该被应用程序直接调用。应用程序应该使用 SetUnhandledExceptionFilter 函数或 AddVectoredExceptionHandler 函数来设置异常处理程序。




## CRC检测
CRC（循环冗余校验码）是一种错误检测技术，主要用于检测数据传输过程中的错误。CRC 检测是通过将一些特定的数据位与一个预设的多项式进行异或运算，然后将结果附加到原始数据后面，最终形成一个新的数据块。接收方可以重复此过程并将结果与接收到的数据进行比较，以检测是否存在传输错误。

CRC 检测广泛应用于计算机网络、串口通讯、存储设备等领域，以确保数据的完整性。在网络中，通常使用 CRC 校验来检测数据包是否损坏。在存储设备中，CRC 校验可用于检测数据在读写过程中的错误。另外，CRC 校验也常用于数据加密和压缩等方面。

CRC 校验的一个重要特点是具有很高的误检率，也就是说，它可能会将正确的数据块错误地标记为错误的。因此，在实际应用中，通常需要使用其他的校验算法来检测数据的正确性，以提高数据的可靠性

## CC断点的原理
        添加断点等于就是将指令改成0xCC, 0xCC 是 INT3 指令，表示断点。然后把原来的指令字节备份保存下来，当调试器需要继续执行时，会恢复备份的原指令并执行

### 反调试

1. 检测代码段是否被修改







# 花指令
-   主要是防静态分析的，防不住动态调试
-   还可以放字符串搜索
-   设计思路是构造恒指令的EIP跳转,中间插入无效数据

``` c
//简单的花指令
_asm
{
    xor eax,eax         //将eax置为0   自己和自己异或 就是置零
    test eax,eax        //判断eax是否为0
    je Label1           //成立跳转到Label1
        jnz Label0      //不成立跳转到Label0
        Label0:
            _emit 0e8h      //是用于生成机器码的指令，它会将一个字节的值 0xE8 直接写入到当前代码段的指令流中。汇编指令 call 的编码，它用于调用一个相对地址
    Label1:
}

clc         //将进位标志（carry flag）清零，既清理cf位
jnb         //跳转与否和cf位有关 cf为0 则跳转
```

``` c
//稍微复杂的花指令
00402006 | E8 00000000              | call 课后作业.40200B                      //跳转到0040200B 调用call会将call的下一地址压栈 既将0x0040200B压入栈中并将ESP指针指向该地址
0040200B | 810424 17000000          | add dword ptr ss:[esp],17                //当前ESP指向的值加17 既将站顶数据改为0x00402022。
00402012 | C3                       | ret                                     //弹出栈地址 即跳转到00402022执行
00402013 | 57                       | push edi                                //花指令
00402014 | 61                       | popad                                   //花指令
00402015 | 74 63                    | je 课后作业.40207A                       //花指令
00402017 | 68 20757220              | push 20727520                           //花指令
0040201C | 73 74                    | jae 课后作业.402092                     //花指令
0040201E | 65:70 21                 | jo 课后作业.402042                       //花指令
00402021 | 0083 7D0C010F            | add byte ptr ds:[ebx+F010C7D],al       //花指令 83开始才是继续执行的代码

```

## 怎么处理花指令
    nop掉恒指令跳转 (纯手动处理 只要有耐心 铁棒磨成针)
    od->选择需要处理的代码->插件->花指令插件->mainDlg->size 1000->Execution (原理匹配易语言花指令的所有模板)

## 生成干净的代码需要修改Visual Studio的配置
1.  平台工具集   xp         既可以支持xp
2.  字符集改成多字节        无所谓
3.  SDL检查                 
3.  安全检查
4.  基本运行时检查 ->未初始化的变量
5.  优化 已禁用
6.  预处理到文件
7.  运行库->mt
8.  启用增量链接->否
9.  生成调试信息->生成经过优化的共享和发布的调试信息(debug:full) 可以显示函数名
10. 执行数据保护(DEP) ->否
11. 随机基地->否        dll不要关
10. 支持仅我的代码调试  ->否

符合模式->否




# main函数是最早执行的函数吗?
    正常最先执行的是OEP，有些技术如TLS线程局部存储技术就优于OEP执行
    c++初始化全局变量的函数_initterm,  main函数的调用invoke_main



# TLS线程局部存储
    TLS（Thread Local Storage）线程局部存储技术是一种操作系统级别的机制，它允许在多线程程序中每个线程有自己独立的数据存储空间。简单来说，TLS可以让不同的线程共享相同的代码段和数据段，但是在数据段中，每个线程都有自己独立的空间，不会相互干扰。

    TLS的实现通常是通过一个特殊的段来实现的，这个段的大小是固定的，并且所有的线程都可以访问到这个段，但是每个线程访问到的实际上是自己独立的一份。这个特殊的段被称为TLS段，其实现方式通常是使用寄存器或者系统调用。

 ##   在Windows操作系统中，TLS可以使用以下方式实现：

1.    使用TLS API
    Windows提供了TLS API，包括以下函数：

    TlsAlloc: 分配一个未使用的TLS索引。
    TlsGetValue: 获取指定索引处的TLS变量值。
    TlsSetValue: 设置指定索引处的TLS变量值。
    TlsFree: 释放指定的TLS索引。
    使用TLS API可以方便地实现TLS，但是需要显式地调用相关的API函数。

2.    使用__declspec(thread)关键字
    在Visual C++中，可以使用__declspec(thread)关键字来声明一个TLS变量，如下所示：

    arduino
    Copy code
    __declspec(thread) int tls_var;
    这样就可以在程序中使用tls_var变量，每个线程都有自己独立的tls_var变量。

3.    使用TLS回调函数
    TLS回调函数是一组在DLL加载和卸载时调用的函数，可以用于初始化和清理TLS变量。Windows提供了以下TLS回调函数：

-    DLL_THREAD_ATTACH: 在新线程创建时调用。
-    DLL_THREAD_DETACH: 在线程退出时调用。
-    DLL_PROCESS_ATTACH: 在DLL加载时调用。
-    DLL_PROCESS_DETACH: 在DLL卸载时调用。
 
    使用TLS回调函数可以实现一些复杂的初始化和清理操作，但是需要在DLL中实现相应的回调函数。

    总的来说，TLS线程局部存储技术可以有效地解决多线程程序中数据共享的问题，提高程序的运行效率和安全性。在实际开发中，需要根据具体情况选择适合的实现方式。

## 逆向调试的使用
    先于OEP执行
-   32位下使用StrongOD插件 Options->Break On Tls
-   PE下的TLS目录中AddressOfCallBacks的地址 

# 易语言特征码
     需要从代码段开始搜索
1. 字符串比较函数特征码：test edx,3
``` c
易语言的字符串比较特征代码
mov edx,dword ptr ss:[esp+0x??]
mov ecx,dword ptr ss:[esp+0x??]
test edx,ecx
...
test edx,0x3

将mov edx,dword ptr ss:[esp+0x??]改为:  即恒成功
mov eax,0x0
retn
```
- 注意观察ECX和EDX,存储的就是比较的2个字符 ECX大概率就是注册码 EDX就是用户输入的字符

2. 按钮事件特征码：FF55FC5F5E
    易语言控件消息派发函数


3. 易语言体特征码：FF25
