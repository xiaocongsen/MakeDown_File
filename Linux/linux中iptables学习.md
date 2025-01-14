# 学习Iptables

    用于管理和操作 Linux 内核的防火墙规则，可以用来保护网络安全，实现网络流量控制，进行网络地址转换（NAT），以及执行其他网络管理任务

    -n 以数字格式显示规则
## iptables 规则链管理

    iptables -L：列出当前防火墙规则。
    iptables -F：清空所有规则。
    iptables -A：添加一条规则到指定链末尾。
    iptables -I：在指定链的指定位置插入一条规则。
    iptables -D：从指定链中删除一条规则。

## 目标和链

    -j：指定目标，例如 -j ACCEPT 表示接受数据包，-j DROP 表示丢弃数据包，-j LOG 表示记录日志。
    -t：指定规则表，例如 -t filter 表示过滤表，-t nat 表示网络地址转换表。

## 协议和端口

    -p：指定协议，如 -p tcp 或 -p udp。
    --sport 和 --dport：指定源端口和目标端口。

## IP 地址和子网

    -s：指定源 IP 地址或源子网。
    -d：指定目标 IP 地址或目标子网。

## 模块和条件

    -m：指定使用的模块，如 -m state 用于状态跟踪。

## 状态跟踪

-m state：使用状态跟踪模块，用于跟踪数据包的连接状态。
--state：用于指定数据包的状态，如 --state NEW 表示新连接。

## 网络地址转换（NAT）

iptables -t nat -A POSTROUTING：添加 POSTROUTING 规则用于网络地址转换。

## 限制连接数和速率

-m limit：用于限制数据包的处理速率。
-m connlimit：用于限制连接数。

## 用户自定义链

iptables -N：创建用户自定义链。
iptables -A CUSTOM_CHAIN：将规则添加到用户自定义链。

## 保存和加载规则

iptables-save：将当前规则保存到文件。
iptables-restore：从文件加载规则。


## 匹配表的顺序

1. raw 表：用于连接跟踪以前的连接状态。
2. mangle 表：用于修改数据包的特定字段。
3. nat 表：用于网络地址转换规则，如端口转发。
4. filter 表：用于过滤数据包的主要规则表。

数据包首先进入 raw 表，用于处理与连接跟踪相关的规则。大多数情况下，这里不会有匹配，因此数据包将继续向下。
数据包接着进入 mangle 表，允许你修改数据包的特定字段。这里可能会有一些匹配，但如果没有匹配，数据包将继续向下。
数据包随后进入 nat 表，用于进行网络地址转换规则。这里可能有匹配，特别是如果你在防火墙上进行了端口转发或其他类型的地址转换。如果没有匹配，数据包将继续向下。
最后，数据包进入 filter 表，它是最主要的规则表，用于过滤数据包。数据包将首先匹配 INPUT 链中的规则，以确定是否接受或拒绝此连接。然后，根据数据包是否通过了 INPUT 链的规则，数据包可能会继续向下匹配 FORWARD 链或 OUTPUT 链。

## 例子

### 添加规则到过滤链

添加一条规则到 INPUT 链，允许来自 TCP 端口 22（SSH）的数据包进入系统。

``` shell
 iptables -A INPUT -p tcp --dport 22 -j ACCEPT
```

### 删除规则

这个命令将从 FORWARD 链中删除一条源 IP 地址为 192.168.1.0/24 的数据包丢弃规则。

```shell
iptables -D FORWARD -s 192.168.1.0/24 -j DROP
```

### 限制连接速率

这个命令将在 INPUT 链中添加一个限制连接速率的规则，允许每分钟最多接受 5 个目标端口为 80 的 TCP 连接。

```shell
iptables -A INPUT -p tcp --dport 80 -m limit --limit 5/min -j ACCEPT
```

### 网络地址转换（NAT）

这个命令将在 nat 表的 POSTROUTING 链中添加一个规则，用于对通过 eth0 出口的数据包执行地址转换（NAT）。

```shell
iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
```

### 使用自定义链

这个命令将创建一个名为 CUSTOM_CHAIN 的用户自定义链，并将一条规则添加到 FORWARD 链中，将数据包跳转到 CUSTOM_CHAIN 进行进一步处理。

```shell
iptables -N CUSTOM_CHAIN
iptables -A FORWARD -j CUSTOM_CHAIN
```

### 使用状态跟踪

这个命令将在 INPUT 链中添加一个规则，允许已建立和相关的连接进入系统，用于支持已建立的会话

```shell
iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
```

## 列出防火墙规则

-L  显示已定义的规则.
-n  以数字格式显示规则.
-t  指定要显示的防火墙表.

```shell
sudo iptables -L -n -t filter
sudo iptables -L -n -t nat
#命令输出解释：
# Chain INPUT (policy ACCEPT)   指示这是针对进入系统的数据包的规则。 策略是 ACCEPT，表示默认情况下允许所有进入的数据包
# target     prot opt source               destination       
# ACCEPT     udp  --  0.0.0.0/0            10.19.13.43          udp dpt:6432             如果数据包是从任何源地址通过udp发送到本地地址10.19.13.43的端口6432，那么应该被接受。
# ACCEPT     tcp  --  0.0.0.0/0            10.19.13.43          tcp dpt:6432             如果数据包是从任何源地址通过tcp发送到本地地址10.19.13.43的端口6432，那么应该被接受。
# Chain FORWARD (policy ACCEPT) 指示用于处理数据包的转发规则. 策略是 ACCEPT，即默认情况下允许所有数据包转发。
# target     prot opt source               destination      

# Chain OUTPUT (policy ACCEPT)     指示这是处理从系统发出的数据包的链。默认策略是 ACCEPT。
# target     prot opt source               destination         
# SDP_SPA    all  --  0.0.0.0/0            0.0.0.0/0                先匹配这个自定义规则  
# SDP_NETMODE  all  --  0.0.0.0/0            0.0.0.0/0              没找到再找这个自定义规则，如果上一条规则已经匹配成功则不会到这条进行匹配

# Chain SDP_SPA (1 references)
# target     prot opt source               destination         
# NFQUEUE    tcp  --  0.0.0.0/0            10.19.13.120         tcp dpt:443 NFQUEUE balance 80:85       //符合这个条件的输出数据包会被重定向到Netfilter Queue的队列编号范围为80到85。
                                                                                                        
# Chain SDP_NETMODE (1 references)
# target     prot opt source               destination         
# ACCEPT     udp  --  0.0.0.0/0            10.19.13.129         udp dpt:8888
# ACCEPT     tcp  --  127.0.0.1            0.0.0.0/0            tcp spt:6432
# ACCEPT     all  --  0.0.0.0/0            8.8.8.8             
# ACCEPT     all  --  0.0.0.0/0            114.114.114.114     
# ACCEPT     tcp  --  0.0.0.0/0            10.18.17.3           multiport dports 1:65535        //multiport 意味着可以匹配多个端口。 允许目标端口位于 1 到 65535 的范围内
# ACCEPT     tcp  --  0.0.0.0/0            10.19.13.120         tcp dpt:443
# ACCEPT     udp  --  8.8.8.8              0.0.0.0/0            udp spt:53
# ACCEPT     udp  --  114.114.114.114      0.0.0.0/0            udp spt:53
# ACCEPT     all  --  0.0.0.0/0            127.0.0.1           
# NFQUEUE    tcp  --  0.0.0.0/0            0.0.0.0/0            NFQUEUE balance 101:105     //NFQUEUE
# NFQUEUE    udp  --  0.0.0.0/0            0.0.0.0/0            NFQUEUE balance 101:105
# DROP       all  --  0.0.0.0/0           !127.0.0.1                                        //DROP表示拒绝匹配到这个规则的数据包，不让数据包通过防火墙 除非来自127.0.0.1
```




sudo iptables -L -n -v --line-numbers   //查看iptables规则
sudo iptables -t nat -L -n             //查看nat规则