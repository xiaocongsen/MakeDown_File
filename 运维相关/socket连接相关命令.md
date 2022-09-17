``` sh
# 查看各种状态的数量
    netstat -n | awk '/^tcp/ {++S[$NF]} END {for(a in S) print a, S[a]}'

#查看当前所有tcp端口
netstat -ntlp   

#查看所有80端口使用情况
netstat -ntulp |grep 80   

#查看所有3306端口使用情况
netstat -an | grep 3306   

#查看一台服务器上面哪些服务及端口
netstat  -lanp     

#查看一个服务有几个端口。比如要查看mysqld
ps -ef |grep mysqld     

#查看某一端口的连接数量,比如3306端口
netstat -pnt |grep :3306 |wc       

#这是用于检查主机的可用性的最常用的工具，但是也可以用于识别带宽延时产品计算的 RTT。
ping  

# 打印某个连接到网络主机所经过的包括一系列路由器和网关的路径(路由)，从而确定每个 hop 之间的延时。
traceroute 

#确定有关网络子系统、协议和连接的各种统计信息。
netstat  

#显示一个或多个连接的协议级的报文跟踪信息；其中还包括时间信息，您可以使用这些信息来研究不同协议服务的报文时间。
tcpdump  

#为应用程序提供一些有关网络性能方面的信息。
netlog

#为瓶颈链接带宽生成一个度量标准；可以用于协议的自动优化。
nettimer

#以一个易于使用的图形化界面提供了 tcpump(报文跟踪)的特性。
Ethereal

#测量 TCP 和 UDP 的网络性能；测量最大带宽，并汇报延时和数据报的丢失情况。
iperf
```


