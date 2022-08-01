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
