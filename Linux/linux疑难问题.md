# 问题一
 Sub-process /usr/bin/dpkg returned an error code 
- 解决方案
    cd /var/lib/dpkg/info/
    ls|xargs sudo rm
    sudo apt clean
    sudo apt autoremove
    sudo apt autoclean
    sudo apt install -f
    sudo apt update 

# 问题二
E: Could not open lock file /var/lib/dpkg/lock-frontend - open (13: Permission denied)
E: Unable to acquire the dpkg frontend lock (/var/lib/dpkg/lock-frontend), are you root?
- 解决方法
    rm /var/lib/dpkg/lock
    rm /var/lib/dpkg/lock-frontend


# 问题三 脚本执行报错
    末尾：syntax error: unexpected end of file
    window环境下编写的，然后传到linux服务器上的 每行的结尾多一个^M 所以报错
- 解决方法
    vim register.sh
    :set fileformat=unix
    :wq

# 问题四 执行脚本报错 command not found
    ./register.sh: line 2: [!: command not found
- 解决方法
    条件判断的[] 两边需要空格 
    如：
    if  [! -n "$1"] ;then           --这就会报错
    正确：
    if  [ ! -n "$1" ] ;then


# Ubuntu使用apt时提示 You must put some ‘source’ URIs in your sources.list
    解决方法：
        sudo sed -i -- 's/#deb-src/deb-src/g' /etc/apt/sources.list && sudo sed -i --'s/# deb-src/deb-src/g' /etc/apt/sources.list
        sudo apt update

# ubuntu虚拟机网卡图标不见了
    sudo dhclient ens32

# 无法远程登录需按照openssh-server
    apt install openssh-server

# 无法远程root登陆
    sudo vi /etc/ssh/sshd_config
    添加 PermitRootLogin yes
    service sshd restart
    
#  允许无密码登录
    sudo vi /etc/ssh/sshd_config
    PermitEmptyPasswords yes
    service sshd restart

# ubuntu安装vmware tools
    sudo apt update
    sudo apt install open-vm-tools
    sudo apt install open-vm-tools-desktop

# ubuntu libxcb-xinerama.so.0 => not found
    sudo apt install libxcb-xinerama0
    

# ubuntu pushd: not found
    pushd命令在bash环境
    执行sudo dpkg-reconfigure dash 命令，将dash设置为No



# libc.so.6: version 'GLIBC_2.25' not found

    使用的高版本gcc提供的新函数，系统版本过低不支持该函数
    可以使用
    objdump -T legacy.so | grep GLIBC_
    strings legacy.so | grep GLIBC_
    nm -D legacy.so | grep GLIBC_
    来查看该so库是因为哪个函数导致的问题