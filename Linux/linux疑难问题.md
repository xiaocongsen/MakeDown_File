# 问题一
 Sub-process /usr/bin/dpkg returned an error code 
- 解决方案
    cd /var/lib/dpkg/info/
    ls|xargs sudo rm
    sudo apt-get clean
    sudo apt-get autoremove
    sudo apt-get autoclean
    sudo apt-get install -f
    sudo apt-get update 

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
        sudo apt-get update