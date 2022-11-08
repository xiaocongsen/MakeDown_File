# shell语法
```shell
if  [ ! -n "$1" ] ;then         #判断该变量是否为空
        echo "没有输入参数1"
        exit 1
fi

for i in $(seq $end -1 $begin)              #begin=15 end=18   则18 17 16 15    $(seq $var -1 1)即从var到1
do
    echo "输出 倒数数 54321"
done

var=$(ls -l)        #获取命令执行后输出的字符串(没有换行)

ls -al | sed 's/-//g'       #删除所有的-


let begin-=1    #变量计算
```

# AWK
```shell
lsblk | awk 'NR==1'             #获取第一行
lsblk | awk 'END{print}'        #获取最后一行
ls -l | awk -F " Apr " '{print $2}'   #每一行以 Apr 分割成多组 输出第2组
ls -l | awk -F " Apr " '{print $2}' | wc -l     #输出后通过wc -l计算出有多少行数据
```

# sed
```shell
lsblk | sed -n '/ceph/='          #获取出现关键字ceph的行数

```

# 杀死所有进程
ps -efww|grep a.out |grep -v grep|cut -c 9-15|xargs kill -9
echo "" > ddd

```shell
#!/bin/bash
sudo apt install  net-tools git libboost-dev openssl  libssl-dev cmake vim autoconf automake libtool libffi-dev doxygen graphviz libpopt-dev xmlto libboost-all-dev libmysqlcppconn-dev
cd ../
# 安装 rabbitmq-c
if [ ! -d "rabbitmq-c" ]; then
    git clone https://github.com/alanxz/rabbitmq-c
    cd rabbitmq-c
    if [ ! -d "build" ]; then
        mkdir build 
        cd build
        cmake ..
        cmake --build .
        make
        sudo make install
        cp librabbitmq/librabbitmq.so* ../../Run-Project/so/
        # 切换回主目录
        cd ../../
    fi
fi

# 下载libcurl
if [ ! -f "curl-7.60.0.tar.gz" ]; then
    wget https://curl.haxx.se/download/curl-7.60.0.tar.gz
fi

```

```shell
#!/bin/bash
a=$1"-"$2
entructName=$a"/EntrustService"
clearName=$a"/ClearService"

if [ ! -n "$2" ] ;then
    echo "你需要输入币对名称,如:BTC-USDT!"
else
    Id=`ps -ef | grep "$entructName" | grep -v "grep" | awk '{print $2}'`
    echo $Id
    kill -9 $Id

    Id=`ps -ef | grep "$clearName" | grep -v "grep" | awk '{print $2}'`
    echo $Id
    kill -9 $Id
    cd ../
    if [ ! -d "$a" ]; then
        echo "文件不存在"
        # mkdir $a
        cp ./Run-Project $a -R
        cd ./$a
        sed -i "3i\CoinId=$1" common.cfg
        sed -i "3i\MarketId=$2" common.cfg
        sed -i "3i\DBPort=3306" common.cfg
    else
        echo "存在"
        cp ./Run-Project/* ./$a/ -R
    fi
fi

```