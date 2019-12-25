### 可用命令
ldd 可执行程序  查询执行程序的链接库
nohup 可执行 &
export MYSQL_CPP_SERVER_VERSION_ID=5700     设置临时环境变量
export PATH=$PATH:/usr/local/go/bin 将/usr/local/go/bin目录添加至PATH环境变量：
cmake ../ -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -DRELEASE_MODE"
scp root@ip:/etc/a.tt ./        把远程拷贝到当前目录
sudo supervisorctl status | awk '{print $1}' | xargs -I {} sudo supervisorctl start {}
sudo supervisorctl status | awk '{print $1}' | xargs -I {} sudo supervisorctl stop {}
ldconfig
/home/deploy/zhuchaodi/code/kafkatest/build/mixcp.bin -C -b 2.0.1.220:9092 -t asset_center_contract_clearing -p 0 -o beginning > a.txt



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
