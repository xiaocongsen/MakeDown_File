# PE学习

## 什么是可执行文件

    可执行文件指的是可以由操作系统进行加载的文件。
    win的可执行文件的格式：PE文件结构
    linux的可执行文件的格式：ELF文件结构

## PE指纹查找

    0x00 0x01 前2个字节必须是4D5A(MZ)
    0x3C      在找0x3C的值(1个字节)
    根据找到的这个字节查找对应的位置是否为5045(PE)

## PE文件结构图

![PE结构图](./pe%E6%96%87%E4%BB%B6%E6%A0%BC%E5%BC%8F%E5%9B%BE.png)

```c++

//DOS头结构
//它在早期的DOS可执行文件中扮演了重要的角色，它的存在使得这些文件可以在DOS操作系统上执行。在现代的Windows PE文件中，DOS头部一般被用作兼容性的考虑，用于兼容旧版的DOS和Windows系统。实际上，PE文件的执行是通过PE头部来控制的。
typedef struct _IMAGE_DOS_HEADER {
    WORD   e_magic;                     //DOS签名字段，用于识别文件是否为DOS可执行文件。通常为"MZ"，表示MS-DOS格式的可执行文件。

    //16位程序使用的,已经没啥用了
    WORD   e_cblp;                      // 文件最后一页的字节数，通常为0。
    WORD   e_cp;                        // 文件中的页数，每页大小为512字节。
    WORD   e_crlc;                      // 重定位项的数量，对于PE文件而言，通常为0。
    WORD   e_cparhdr;                   // 头部的大小（以段为单位），即IMAGE_DOS_HEADER结构体的大小除以16。
    WORD   e_minalloc;                  // 程序加载时所需的最小内存段数，通常为0。
    WORD   e_maxalloc;                  // 程序加载时所需的最大内存段数，通常为0xFFFF。
    WORD   e_ss;                        // 初始堆栈段的相对地址，通常为0。
    WORD   e_sp;                        // 初始堆栈指针的值，通常为0xB8。
    WORD   e_csum;                      // 文件校验和，通常为0。
    WORD   e_ip;                        // 程序初始执行的偏移地址，通常为0。
    WORD   e_cs;                        // 初始代码段的相对地址，通常为0。
    WORD   e_lfarlc;                    // 重定位表在文件中的偏移地址。
    WORD   e_ovno;                      // 覆盖号，通常为0。
    WORD   e_res[4];                    // 保留字段，共4个2字节的保留字，通常为0。
    WORD   e_oemid;                     // OEM标识符，通常为0。
    WORD   e_oeminfo;                   // OEM信息，与e_oemid一起使用，通常为0。
    WORD   e_res2[10];                  // 保留字段，共10个2字节的保留字，通常为0。
    //16位程序END

    LONG   e_lfanew;                    //新的可执行文件头（PE头）的文件地址偏移量。该字段指定了PE头在文件中开始的位置。
} IMAGE_DOS_HEADER, * PIMAGE_DOS_HEADER;    //64个字节

//DOS stub是e_lfanew到pe头直接的内容  大小不固定  它在早期的DOS可执行文件中具有实际的功能，因为这些文件可以在DOS操作系统上直接运行。但在现代的Windows PE文件中，这段代码并不具备实际的功能，仅用于兼容性和保留的目的。

//pe头结构 ,64位结构IMAGE_NT_HEADERS64
typedef struct _IMAGE_NT_HEADERS {
    DWORD Signature;                            //pe标识
    IMAGE_FILE_HEADER FileHeader;               //标准pe头
    IMAGE_OPTIONAL_HEADER32 OptionalHeader;     //扩展pe头  64位IMAGE_OPTIONAL_HEADER64
} IMAGE_NT_HEADERS32, * PIMAGE_NT_HEADERS32;

//标准pe头
typedef struct _IMAGE_FILE_HEADER {
    WORD    Machine;                                //表示可执行文件的目标硬件平台或操作系统。它使用特定的标识符来表示不同的架构，例如 0代表任意 x86(0x014C)、x64(0x8664)、ARM(0x01C0)等
    WORD    NumberOfSections;                       //表示节和 节表的数量
    DWORD   TimeDateStamp;                          //编译器填写的时间戳 与文件属性里面(创建时间、修改时间)无关 编译时间  
    DWORD   PointerToSymbolTable;                   //符号表的文件偏移地址，指示符号表在PE文件中的位置。符号表存储了与代码、函数和变量等相关的符号信息。
    DWORD   NumberOfSymbols;                        //符号表中符号的数量，表示PE文件中包含的符号的数量。
    WORD    SizeOfOptionalHeader;                   //扩展pe头的大小 正常情况下x86大小是0xE0,x64是0xF0
    WORD    Characteristics;                        //文件属性
} IMAGE_FILE_HEADER, * PIMAGE_FILE_HEADER;          //20个字节

//Characteristics文件属性
// 0    IMAGE_FILE_RELOCS_STRIPPED                          //文件中不包含重定位信息。意味着PE文件不能被加载到不同的内存地址，适用于固定基址的可执行文件。
// 1    IMAGE_FILE_EXECUTABLE_IMAGE                         //文件是一个可执行的映像文件
// 2    IMAGE_FILE_LINE_NUMS_STRIPPED                       //文件中不包含行号信息。
// 3    IMAGE_FILE_LOCAL_SYMS_STRIPPED                      //文件中不包含本地符号信息。
// 4    IMAGE_FILE_AGGRESIVE_WS_TRIM                        //系统可以采取积极的方式来缩减文件的工作集大小。即：调整工作集
// 5    IMAGE_FILE_LARGE_ADDRESS_AWARE                      //应用程序可处理大于2GB的地址。适用于64位应用程序。
// 6                                                        //此标志保留
// 7    IMAGE_FILE_BYTES_REVERSED_LO                        //小尾方式
// 8    IMAGE_FILE_32BIT_MACHINE                            //只在32位平台上运行
// 9    IMAGE_FILE_DEBUG_STRIPPED                           //不包含调试信息
// 10   IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP                  //不能从可移动盘运行
// 11   IMAGE_FILE_NET_RUN_FROM_SWAP                        //不能从网络运行,可以将其从交换设备（如软盘）运行。
// 12   IMAGE_FILE_SYSTEM                                   //系统文件(如驱动程序)，用于操作系统组件,不能直接运行
// 13   IMAGE_FILE_DLL                                      //文件是一个动态链接库（DLL）。
// 14   IMAGE_FILE_UP_SYSTEM_ONLY                           //文件应该仅在上层操作系统上运行 文件不能在多处理器计算机上运行
// 15   IMAGE_FILE_BYTES_REVERSED_HI                        //大尾方式  高位字节在前，低位字节在后。

//32位扩展pe头
typedef struct _IMAGE_OPTIONAL_HEADER {
    //
    // Standard fields.
    //

    WORD    Magic;                                                               ///PE32为0x10B  PE32 + 为0x20B
    BYTE    MajorLinkerVersion;                                                  //链接器版本号
    BYTE    MinorLinkerVersion;                                                  //链接器版本号
    DWORD   SizeOfCode;                                                          //所有代码节的总和 文件对齐后的大小 编译器填的 没用
    DWORD   SizeOfInitializedData;                                               //包含所有已经初始化数据的节的总大小 文件对齐后的大小 编译器填的 没用
    DWORD   SizeOfUninitializedData;                                             //包含未初始化数据的节的总大小 文件对齐后的大小 编译器填的 没用
    DWORD   AddressOfEntryPoint;                                                 ///程序人口地址 加上ImageBase为内存中实际程序入口地址
    DWORD   BaseOfCode;                                                          //代码开始的基址，编译器填的 没用
    DWORD   BaseOfData;                                                          //数据开始的基址，编译器填的 没用

    //
    // NT additional fields.
    //

    DWORD   ImageBase;                                                           ///内存中镜像基址   开始展开pe文件的开始位置
    DWORD   SectionAlignment;                                                    ///节对齐大小，表示内存中各节的对齐要求
    DWORD   FileAlignment;                                                       ///文件对齐大小，表示文件中各节的对齐要求。
    WORD    MajorOperatingSystemVersion;                                         //标识操作系统版本号 主版本号
    WORD    MinorOperatingSystemVersion;                                         //标识操作系统版本号 次版本号
    WORD    MajorImageVersion;                                                   //PE文件自身的版本号
    WORD    MinorImageVersion;                                                   //PE文件自身的版本号
    WORD    MajorSubsystemVersion;                                               //运行所需子系统版本号
    WORD    MinorSubsystemVersion;                                               //运行所需子系统版本号
    DWORD   Win32VersionValue;                                                   //子系统版本的值，必须为0
    DWORD   SizeOfImage;                                                         ///内存中整个PE文件的映射的尺寸，可比实际的值大，必须是SectionAlignment的整数信
    DWORD   SizeOfHeaders;                                                       ///PE头部分和所有节表按照文件对齐后的大小,是FileAlignment的整数倍
    DWORD   CheckSum;                                                            ///校验和，一些系统文件有要求.用来判断文件是否被修改
    WORD    Subsystem;                                                           //子系统      驱动程序(1) 图形界面(2) 控制台、DLL(3)
    WORD    DllCharacteristics;                                                  //文件特性 不是针对DLL文件的
    DWORD   SizeOfStackReserve;                                                  //初始化时保留的栈大小
    DWORD   SizeOfStackCommit;                                                   //初始化时实际提交的大小
    DWORD   SizeOfHeapReserve;                                                   //初始化时保留的堆大小
    DWORD   SizeOfHeapCommit;                                                    //初始化时实践提交的大小
    DWORD   LoaderFlags;                                                         //调试相关
    DWORD   NumberOfRvaAndSizes;                                                 //目录项数目 (如导入表 导出表 重定位表...)
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];        
} IMAGE_OPTIONAL_HEADER32, * PIMAGE_OPTIONAL_HEADER32;                      //224个字节
//DllCharacteristics文件特性
// 0                                                                            保留，必须为0
// 1                                                                            保留，必须为0
// 2                                                                            保留，必须为0
// 3                                                                            保留，必须为0
// 4                                                                            
// 5
// 6                IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE                       DLL可以在加载时被重定位
// 7                IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY                    强制代码实施完整性验证
// 8                IMAGE_DLLCHARACTERISTICS_NX_COMPAT                          该映像兼容DEP 
// 9                IMAGE_DLLCHARACTERISTICS_NO_ISOLATION                       可以隔离，但并不隔离此映像
// 10               IMAGE_DLLCHARACTERISTICS_NO_SEH                             映像不使用SEH
// 11               IMAGE_DLLCHARACTERISTICS_NO_BIND                            不绑定映像
// 12                                                                           保留，必须为0
// 13               IMAGE_DLLCHARACTERISTICS_WDM_DRIVER                         该映像为一个WDMdriver
// 14                                                                           保留，必须为0
// 15               IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE              可用于终端服务器
// IMAGE_DATA_DIRECTORY的每一个下表对应的表
#define IMAGE_DIRECTORY_ENTRY_EXPORT          0   //导出目录。包含有关导出函数和符号的信息，允许其他模块访问这些导出。
#define IMAGE_DIRECTORY_ENTRY_IMPORT          1   //导入目录。包含了PE文件所依赖的其他模块的信息，包括导入函数的名称和地址等。
#define IMAGE_DIRECTORY_ENTRY_RESOURCE        2   //资源目录。包含了PE文件中嵌入的资源，如图标、位图、字符串等。
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION       3   //异常目录。包含了异常处理信息，用于处理在程序执行期间发生的异常情况。
#define IMAGE_DIRECTORY_ENTRY_SECURITY        4   //安全目录。包含了数字签名和证书等安全相关的信息。
#define IMAGE_DIRECTORY_ENTRY_BASERELOC       5   //基址重定位表。包含了重定位信息，用于在加载时调整程序的内存地址。
#define IMAGE_DIRECTORY_ENTRY_DEBUG           6   //调试目录。包含了调试符号和调试相关的信息，用于调试和故障排除。
//      IMAGE_DIRECTORY_ENTRY_COPYRIGHT       7   //(X86 usage)
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    7   //体系结构特定数据目录。存储与特定体系结构相关的数据。
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR       8   //全局指针目录。存储全局指针的相对虚拟地址（RVA）。
#define IMAGE_DIRECTORY_ENTRY_TLS             9   //TLS（线程本地存储）目录。包含了线程本地存储相关的信息。
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10   //加载配置目录。包含了与加载器相关的配置信息。
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   11   //绑定导入目录。存储绑定导入的相关信息。
#define IMAGE_DIRECTORY_ENTRY_IAT            12   //导入地址表目录。存储导入函数的地址。
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13   //延迟加载导入目录。存储延迟加载导入的相关信息。
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   //COM（组件对象模型）运行时描述符。存储与COM相关的信息。

//64位扩展pe头
typedef struct _IMAGE_OPTIONAL_HEADER64 {
    WORD        Magic;                              
    BYTE        MajorLinkerVersion;
    BYTE        MinorLinkerVersion;
    DWORD       SizeOfCode;
    DWORD       SizeOfInitializedData;
    DWORD       SizeOfUninitializedData;
    DWORD       AddressOfEntryPoint;
    DWORD       BaseOfCode;
    ULONGLONG   ImageBase;
    DWORD       SectionAlignment;
    DWORD       FileAlignment;
    WORD        MajorOperatingSystemVersion;
    WORD        MinorOperatingSystemVersion;
    WORD        MajorImageVersion;
    WORD        MinorImageVersion;
    WORD        MajorSubsystemVersion;
    WORD        MinorSubsystemVersion;
    DWORD       Win32VersionValue;
    DWORD       SizeOfImage;
    DWORD       SizeOfHeaders;
    DWORD       CheckSum;
    WORD        Subsystem;
    WORD        DllCharacteristics;
    ULONGLONG   SizeOfStackReserve;
    ULONGLONG   SizeOfStackCommit;
    ULONGLONG   SizeOfHeapReserve;
    ULONGLONG   SizeOfHeapCommit;
    DWORD       LoaderFlags;
    DWORD       NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64, * PIMAGE_OPTIONAL_HEADER64;

//节表  可以有多个 .text节 .data节 .rsrc节 .reloc节 ... 
typedef struct _IMAGE_SECTION_HEADER {
    BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];          //ASCII字符串 可自定义 只截取8个 节的名字
    union {                                         //Misc 双字 是该节在没有对齐前的真实大小,该值可以不准确 实际值 
        DWORD   PhysicalAddress;                    //
        DWORD   VirtualSize;                        //
    } Misc;                                         //初始化好的变量在文件的节中是占空间的，未初始化的变量在文件中是没空间的，但到内存中需要分配空间
    DWORD   VirtualAddress;                         //节区在内存中的偏移地址,加上ImageBase才是在内存中的真正地址
    DWORD   SizeOfRawData;                          //节在文件中对齐后的尺寸           未初始化的变量不给分配大小  和真实大小谁大用谁在内存中展开
    DWORD   PointerToRawData;                       //节区在文件中的偏移  可以找到节开始的位置
    DWORD   PointerToRelocations;                   //调试相关
    DWORD   PointerToLinenumbers;                   //
    WORD    NumberOfRelocations;                    //
    WORD    NumberOfLinenumbers;                    //
    DWORD   Characteristics;                        //节的属性
} IMAGE_SECTION_HEADER, * PIMAGE_SECTION_HEADER;                ///40个字节 

//Characteristics 节属性
//5   IMAGE_SCN_CNT_CODE 或 00000020h                                  节中包含代码
//6   IMAGE_SCN_CNT_INITIALIZED_DATA 或 00000040h                      节中包含已初始化数据
//7   IMAGE_SCN_CNT_UNINITIALIZED_DATA 或00000080h                     节中包含未初始化数据
//8   IMAGE_SCN_LNK_OTHER 成 00000100h                                 保留供将来使用
//25  IMAGE_SCN_MEM_DISCARDABLE 或02000000h                            节中的数据在进程开始以后将被丢弃，如.reloc
//26  IMAGE_SCN_MEM_NOT_CACHED 或 04000000h                            节中的数据不会经过缓存
//27  IMAGE_SCN_MEM_NOT_PAGED 或08000000h                              节中的数据不会被交换到磁盘
//28  IMAGE_SCN_MEM_SHARED 或10000000h                                 表示节中的数据将被不同的进程所共享
//29  IMAGE_SCN_MEM_EXECUTE 或 20000000h                               映射到内存后的页面包含可执行属性
//30  IMAGE_SCN_MEM_READ 或 40000000h                                  映射到内存后的页面包含可读属性
//31  IMAGE_SCN_MEM_WRITE 或 80000000h                                 映射到内存后的页面包含可写属性


//SizeOfHeaders - n*40 - 224 - 20  - 4为多余的 可以随意修改，

//然后就是第一个的块数据 也必须是FileAlignment的整数倍,不够0填充 

//导出表
typedef struct _IMAGE_EXPORT_DIRECTORY {
    DWORD   Characteristics;        //未使用
    DWORD   TimeDateStamp;          //时间戳 编译生成时间
    WORD    MajorVersion;           //未使用
    WORD    MinorVersion;           //未使用
    DWORD   Name;                   //指针地址,指向该导出表文件名字的字符串   
    DWORD   Base;                   //导出函数起始序号
    DWORD   NumberOfFunctions;      //所有导出函数的个数        //不一定准确，是通过序号最大-序号最小 但中间可能有断层
    DWORD   NumberOfNames;          //以函数名字导出的函数个数      
    DWORD   AddressOfFunctions;     //指针地址 导出函数地址表RVA    存储数量是NumberOfFunctions
    DWORD   AddressOfNames;         //指针地址 导出函数名称表RVA    数量是NumberOfNames
    DWORD   AddressOfNameOrdinals;  //指针地址 导出函数序号表RVA    数量是NumberOfNames
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;         

typedef struct _IMAGE_DATA_DIRECTORY {
    DWORD   VirtualAddress;                         //开始的地址
    DWORD   Size;                                   //大小 编译器填写的，没有实际意义
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;


// 导入表
typedef struct _IMAGE_IMPORT_DESCRIPTOR {
    union {
        DWORD   Characteristics;            // 若为0，表示导入表描述符的终止符。若非0，表示指向未绑定的导入地址表的相对虚拟地址（RVA）。
        DWORD   OriginalFirstThunk;         // 未绑定的导入地址表存储了导入函数的信息，包括函数名称和导入函数的序号。
    } DUMMYUNIONNAME;
    DWORD   TimeDateStamp;                  // 若该字段为0，表示该导入模块未被绑定。
                                            //若该字段为-1，表示该导入模块已被绑定，并且真实的日期/时间戳可以在IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT中找到（新版BIND）。
                                            //若该字段为其他非零值，表示该导入模块已被绑定，并且日期/时间戳表示绑定到的DLL的日期/时间戳（旧版BIND）。

    DWORD   ForwarderChain;                 //若该字段为-1，表示没有转发导入（forwarders）。若该字段为其他非零值，表示该字段是一个索引，指向下一个转发导入。
    DWORD   Name;                           //一个指向导入模块名称的相对虚拟地址（RVA）。
    DWORD   FirstThunk;                     //指向导入地址表（Import Address Table）的相对虚拟地址（RVA）。导入地址表存储了导入函数的实际地址，或者在未绑定状态下存储了指向未绑定导入函数的引用。
} IMAGE_IMPORT_DESCRIPTOR;
//导入表的结束 所有字段都为0的导入表对象
```

## 注意项

    PE文件运行前后不一样

## RVA

    RVA（Relative Virtual Address，相对虚拟地址）是相对于模块加载基址（Image Base）的地址。在内存中执行时，PE文件被加载到进程的虚拟内存中，每个模块都会有一个基址。RVA表示相对于该基址的偏移量，用于定位模块内的数据或代码。

## FOA

    FOA（File Offset Address，文件偏移地址）是指相对于PE文件开头的偏移量。它表示数据在PE文件中的实际位置的偏移量，以字节为单位。FOA可以用于定位文件中的各个部分，如节表、数据段等。

### RVA到FOA的转换

    1. RVA - ImageBase
    2. 判断RVA的地址是否位于PE头中,如果是：FOA == RVA
    3. 判断RVA的地址位于那个节
        RVA >= 节.VirtualAddress                            //判断大于等于节开始的位置
        RVA <= 节.VirtualAddress + 当前内存对齐后的大小       //小于等于节结束的位置
        差值 = RVA - 节.VirtualAddress                       //减去节开始的位置,就能得到差值
    4.FOA = 节.PointerToRawData + 差值

## 学习实践

### 对已经编译好的文件在本电脑上进行新功能的添加

> 向一个程序中添加一个弹窗提示，然后再指向原来的程序

1. 初步构造要写入的二进制代码
    00000F90h: 6A 00 6A 00 6A 00 6A 00 E8 00 00 00 00 E9 00 00 00 00
    开始地址    4个传入参数              call           jmp
1. 找编译好程序可以插入二进制代码的位置
    节表后有些为了对齐添加的00，节后也有为了对齐添加的00，都可以随意修改
1. 找该编译好的程序中MessageBoxA的入口地址
    将编译好的程序丢入OD->按工具栏的E找到链接USER32.dll->点击进入后按ctrl+n跳转到符号表，查找MessageBoxA的地址76E1Fd1E
1. 计算E8 E9后面的值
    MessageBoxA入口 - (ImageBase + E8指令当前的地址) - 5
    76E1Fd1E - 400F98 - 5 =  76A1ED81
    程序入口地址 - E9指令当前的地址 - 5
    183D7 - F9D - 5 = 17435
1. 填入地址
    00000F90h: 6A 00 6A 00 6A 00 6A 00 E8 81 ED A1 76 E9 35 74 01 00
1. 修改入口地址
    扩展pe头中AddressOfEntryPoint修改为 90 0F 00 00

### 扩大节

1. 查找最后一个节,并查看节的Characteristics是否有可执行属性(最好新增一个节)
1. 在文件最后面分配一块新的空间，大小为S 最好是内存对齐
1. 将最后一个节的SizeOfRawData和VirtualSize改为N
    N = (SizeOfRawData和VirtualSize最大的 内存对齐后的值) + S
1. 修改SizeOfimage大小

### 新增节

1. 判断是否有足够的空间，可以添加一个节表.
1. 在节表中新增一个成员
1. 修改PE头中节的数量.
1. 修改sizeOflmage的大小
1. 再原有数据的最后.新增一个节的数据(内存对齐的整数倍).
1. 修正新增节表的属性

### 合并节

1. 按照内存对齐展开
2. 将第一个节的内存大小、文件大小改成一样
    Max = SizeOfRawData>VirtualSize?SizeOfRawData:VirtualSize
    SizeOfRawData = VirtualSize = 最后一个节的VirtualAddress + Max - SizeOfHeaders内存对齐后的大小
3. 将第一个节的属性改为包含所有节的属性
4. 修改节的数量为1