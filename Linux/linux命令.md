### 可用命令
ldd 可执行程序  查询执行程序的链接库
nm  查看动态库的链接符合

nohup 可执行 &
export MYSQL_CPP_SERVER_VERSION_ID=5700     设置临时环境变量
export PATH=$PATH:/usr/local/go/bin 将/usr/local/go/bin目录添加至PATH环境变量：
cmake ../ -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -DRELEASE_MODE"
scp root@ip:/etc/a.tt ./        把远程拷贝到当前目录
sudo supervisorctl status | awk '{print $1}' | xargs -I {} sudo supervisorctl start {}
sudo supervisorctl status | awk '{print $1}' | xargs -I {} sudo supervisorctl stop {}
ldconfig
/home/deploy/zhuchaodi/code/kafkatest/build/mixcp.bin -C -b 2.0.1.220:9092 -t asset_center_contract_clearing -p 0 -o beginning > a.txt

hostname    //查看主机名
hostnamectl set-hostname pve2   //修改主机name

dd if=/dev/zero of=/test count=2 bs=512M        生成一个512*2大小的文件

sudo dpkg -i *.deb      按照deb程序

netstat -nr     查看路由表
route -n        查看路由表
sudo route add -net 10.21.0.0/16 gw 192.168.127.2   增加路由
sudo route del -net 10.20.0.0/16 gw 192.168.127.2   删除路由
sudo route add -net 192.168.79.0/24 metric 1000 dev ens33
sudo route del -net 192.168.79.0/24 metric 1000 dev ens33

du -h --max-depth=1 --one-file-system .         //查看文件大小

awk -F'/' '{print $1}' input.txt > output.txt       //将文件名中的/替换为空

uname -m    输出当前系统的硬件架构信息
uname -r    输出内核版本号
uname -a    输出包括内核版本、硬件架构、操作系统版本等。
如：Linux xiaocs-Platform 5.10.0-5-generic #15~v10pro-KYLINOS SMP Tue Aug 3 03:55:56 UTC 2021 x86_64 x86_64 x86_64 GNU/Linux
    Linux: 基于Linux内核的操作系统
    xiaocs-Platform:主机名称
    5.10.0-5-generic: 内核版本号 5.10.0是内核主版本号，5是补丁级别 generic表示内核版本的通用构建
    #15~v10pro-KYLINOS SMP Tue Aug 3 03:55:56 UTC 2021  :是内核的详细版本信息，包括构建编号、操作系统（这里是Ubuntu）和编译时间。
    x86_64 x86_64 x86_64: 这是指在这个特定的系统中，有三个硬件架构都被支持，都是x86_64。通常情况下，一个系统只有一个主要的硬件架构，但有些情况下，一个系统可能会支持多个硬件架构，这是为了兼容不同类型的硬件或处理器。
    GNU/Linux: 指示操作系统是一个基于GNU工具的Linux系统。


cat /etc/os-release | grep -w ID | awk -F '=' '{print $2}' | sed 's/\"//g' | awk '{print tolower($0)}'
                    找到包含ID的行    以等号作为分隔符输出2个   双引号替换为空          将输出转换为小写


sudo insmod lskg.ko     加载模块
lsmod  列出当前加载的内核模块 内核模块是一种动态加载到操作系统内核中的代码，它们可以添加功能、驱动硬件设备或扩展内核的功能。
    Module(列表中的模块名称列)    Size(模块在内存中的占用大小)  Used by(使用该模块的其他模块或进程的数量)

which ls     查看ls命令的位置


### 快捷键
Ctrl+a      跳到本行的行首
Ctrl+e      跳到页尾
Ctrl+u      删除当前光标前面的文字 （还有剪切功能）
Ctrl+k      删除当前光标后面的文字(还有剪切功能)
Ctrl+L      进行清屏操作
Ctrl+y      粘贴Ctrl+u或ctrl+k剪切的内容
Ctrl+w      删除光标前面的单词的字符
Ctrl+b      往回(左)移动一个字符
Ctrl+f      往后(右)移动一个字符
ctrl+t      交换光标位置前的两个字符 

esc+f       往右跳一个词 
esc+b       往左跳一个词
esc+t       交换光标位置前的两个单词。



apt-get install bridge-utils
brctl addbr br0                     创建虚拟网卡
ifconfig br0 10.10.20.1/24 up       开启虚拟网卡

                    组名   用户名
usermod -a -G audio username
id <username>


objdump -T legacy.so | grep GLIBC_  查看某个so库是依赖哪个libc的版本是 且因为哪个函数
strings legacy.so | grep GLIBC_



sudo apt-get install -f     修复系统安装包的依赖问题


sudo dpkg -i --force-all /path/to/package.deb   任何错误都可以安装

sudo dpkg -i --force-depends /path/to/package.deb   忽略依赖性问题

sudo dpkg -i --force-overwrite /path/to/package.deb     忽略覆盖文件冲突

sudo dpkg -i --force-confnew /path/to/package.deb       忽略新旧配置文件冲突    直接使用新文件



# 获取直接依赖和推荐的包名，使用更宽松的正则表达式以尽可能准确地匹配包名
depends=$(sudo apt-cache depends startdde | grep -oP '^  依赖: \K([\w\+\.:_-]+)')
recommends=$(sudo apt-cache depends startdde | grep -oP '^  推荐: \K([\w\+\.:_-]+)')

# 合并依赖和推荐列表，去除重复项
packages=($(echo "$depends $recommends" | tr ' ' '\n' | sort -u))



dpkg -S 全路径/进程   可以获取这个进程的包名



cp -rL 源文件夹 目标文件夹      拷贝文件 如果是软链接 会替换成实际文件