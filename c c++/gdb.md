# GDB

    gdb 程序名
    run 参数1 参数2

    停止后
    bt  查看堆栈


# 开启生成core文件

    ulimit -c   查看可以生成core文件的大小
    ulimit -c unlimited    取消该大小限制
    设置好后程序奔溃就会产生core文件

    gdb 程序名 ./core   

# 命令

    (gdb) start                          //开始调试
    (gdb) n                             //一条一条执行
    (gdb) step/s                        //执行下一条，如果函数进入函数
    (gdb) backtrace/bt                  //查看函数调用栈帧
    (gdb) info/i locals                 //查看当前栈帧局部变量
    (gdb) frame/f                       //选择栈帧，再查看局部变量
    (gdb) print/p                       //打印变量的值
    (gdb) finish                        //运行到当前函数返回
    (gdb) set var sum=0                 //修改变量值
    (gdb) list/l 行号或函数名             //列出源码
    (gdb) display/undisplay sum         //每次停下显示变量的值/取消跟踪
    (gdb) break/b  行号或函数名           //设置断点
    (gdb) continue/c                    //连续运行
    (gdb) info/i breakpoints            //查看已经设置的断点
    (gdb) delete breakpoints 2          //删除某个断点
    (gdb) disable/enable breakpoints 3  //禁用/启用某个断点
    (gdb) break 9 if sum != 0           //满足条件才激活断点
    (gdb) run/r                         //重新从程序开头连续执行
    (gdb) watch input[4]                //设置观察点
    (gdb) info/i watchpoints            //查看设置的观察点
    (gdb) x/7b input                    //打印存储器内容，b--每个字节一组，7--7组
    (gdb) disassemble                   //反汇编当前函数或指定函数
    (gdb) si                            // 一条指令一条指令调试 而 s 是一行一行代码
    (gdb) info registers                // 显示所有寄存器的当前值
    (gdb) x/20 $esp                    //查看内存中开始的20个数


# 调试已经在运行的程序
    gdb attach pid
    从ps获得进程号，通过attach命令连接到该进程。attach一个进程后，gdb首先stop该进程，这样就可以设置断点，执行step、continue等命令；如果执行r命令，会杀掉原来的进程。
    detach：释放该进程，原进程会继续执行。



# 多线程调试

info threads： 显示线程信息。
thread thread-id： 切换到对应线程。
thread name [name]: 修改当前thread的名称。默认为程序名称

# 调试fork

    默认情况下，当一个程序fork，gdb会跟踪父进程而子进程畅通无止的执行。如果在child中设置了一个断点，执行到断点处，child会收到一个SIGTRAP信号，默认为终止程序。
    最开始调试child进程的方法是在child中设置一个sleep，然后通过attach连到child。
    set follow-fork-mode mode
    show follow-fork-mode:
    在Linux2.5.46以后，gdb支持fork函数的调试；通过set-follow-mode命令，设置跟随parent还是child，如果不设置默认为跟随parent。
    set detach-on-fork mode
    show detach-on-fork:
    在Linux，可以通过set detach-on fork off命令，在fork后，对parent和child都进行调试。父、子进程为多个inferiors，参见第三部分。


# 设置源码位置
    (gdb) l
    (gdb) directory /usr/src/python  设置源码位置
    (gdb) show directories 显示当前源码位置
    (gdb) l

# Checkpoint

    在Linux，gdb支持保留程序在某个时候的快照，除了进程号和已经输出的内容，所有状态恢复到快照状态。
    通过以下命令实现快照。
    checkpoing: 生成当前状态的快照
    nfo checkpoint：显示快照信息
    restart checkpoint-id：恢复到某个checkpoint
    delete checkpoint checkpoint-id：删除某个checkpoint