-Wl    选项告诉编译器将后面的参数传递给链接器。

-m64            设置64位编译
-m32            设置32位编译

-z,relro
relro:Relocation Read Only。Create an ELF "PT_GNU_RELRO" segment header in the object。也就是会创建一个PT_GNU_RELRO段，这个重定向区域时只读的。这个是用来保护目标文件防止被篡改，编译器一般都会默认带上此参数

-z,noexecstack
标记当前链接的目标文件是不需要executable stack的。

-Wall   选项意思是编译后显示所有警告。

-funsigned-char 、-fno-signed-char、-fsigned-char 、-fno-unsigned-char
    这四个参数是对 char 类型进行设置, 决定将 char 类型设置成 unsigned char(前两个参数)或者 signed char(后两个参数)。


--gc-sections
    表示依赖库中不使用的sections将不被链接，可以减小链接目标的大小，一般跟-ffunction-sections一起使用。用法：-Wl,--gc-sections。

–disable-new-dtags告诉新链接器使用旧行为,即RPATH.
–enable-new-dtags告诉旧链接器使用新行为,即RUNPATH.
在通过 rpath 指定路径后，会在二进制文件中生成这两个参数，可以通过 readelf -d main | grep -E (RPATH|RUNPATH) 命令查看，老版本中使用的是 RPATH 新版本使用 RUNPATH 替换掉。
查找动态库的过程中，大致的顺序是 RPATH LD_LIBRARY_PATH RUNPATH ，所以，如果使用的是 RPATH 用户将无法进行调整，所以建议使用 RUNPATH ，这也是 gcc 的默认值。
可以通过 -Wl,--disable-new-dtags 表明使用 RPATH ；通过 -Wl,--enable-new-dtags 标示使用 RUNPATH 。

GCC传递给链接器ld可以同时使用Xlinker和Wl两种命令，这两个命令都可以正确传递给ld作为使用。
Xlinker后面跟的参数第一个是空格，而Wl后面跟的第一个字符是","。
对于传递“-assert definitions”命令给ld来说，Xlinker要一下子传递两个参数需要写两次“Xlinker”，比如-Xlinker -assert -Xlinker defintions而不能一下子写成-Xlinker "-assert definitions"因为链接器会认为这是一个参数，而不是两个参数。如果此时你用的是GNU的linker，通常更简便的做法就是用option=value的方式，比如-Xlinker -Map -Xlinker output.mp可以简写成-Xlinker -Map=output.map。而对于Wl来说，因为他的参数分割是用“，”这样可以不用像Xlinker一样一下子写多个，只需要写多个逗号即可，就拿这个例子来说,可以写成-Wl,-Map,output.map当然了，如果你用的是GNU的linker你也可以-Wl,-Map=output.map。
所以对于rpath来说使用Xlinker可以写成-Xlinker -rpath -Xlinker <dir>（-Xlinker -rpath=<dir>），对于Wl来说可以写成-Wl,rpath,<dir>(-Wl,rpath=<dir>)。
-Xlinker，写成如下格式可以强制gcc重复查找依赖库：
g++ -o tt tt.o -Xlinker "-(" -lws2_32 -lclsocketd -Xlinker "-)" 
这样在括号中的库的依赖关系就不需要你操心啦，虽然会延长链接时间。


-soname 译时用到soname参数，直接改库文件名是不可以的