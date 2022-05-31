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

```

