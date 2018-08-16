# 杀死所有进程
ps -efww|grep a.out |grep -v grep|cut -c 9-15|xargs kill -9
echo "" > ddd

```shell
#!/bin/bash
sudo apt-get install  net-tools git libboost-dev openssl  libssl-dev cmake vim autoconf automake libtool libffi-dev doxygen graphviz libpopt-dev xmlto libboost-all-dev libmysqlcppconn-dev
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