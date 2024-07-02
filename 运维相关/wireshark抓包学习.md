
    SYN表示建立连接
    FIN表示关闭连接
    ACK表示响应
    PSH表示有DATA数据传输
    RST表示连接重置

## ack和seq的关系
SEQ 号表示当前数据包的编号,是表示当前包在整个数据流中的位置。
ACK 号表示收到的最后一个数据包的 SEQ 号,是当前状态下已经收到的数据包编号的最大值。

当一个主机发送数据包时,它把数据包的 SEQ 号设置为当前数据流的编号,同时该数据包 ACK 号表示收到的最后一个数据包的 SEQ 号。当收到该数据包并返回 ACK 数据包时,它的 ACK 号就是该数据包的 SEQ 号 + 数据长度。

ACK 和 SEQ 的值之间相差很大的原因有很多。可能是因为连接中有大量数据交换,也可能是因为存在大量重传。还有可能是因为网络中出现了拥堵,导致连接超时。具体原因取决于特定情况。

SEQ/ACK analysis 是通过比较 SEQ 和 ACK 的值来判断 TCP 连接的状态和数据流量是否正常的。

## Wireshark整条tcp数据分析
>SourceIP=10.19.11.65 DestinationIP=10.19.12.123
- 第一个数据  seq=0        ack=0      SYN          datalen=0               nextseq=1                  //首次建立tcp链接,属于三次握手的第一次握手 本端nextseq=seq+1(datalen等于0的时候且非ACK和RST就+1) nextseq表示本端下一个数据包的的seq值=1
- 第三个数据  seq=1        ack=1      ACK          datalen=0               nextseq=1                  //首次建立tcp链接,属于三次握手的第三次握手 ack=对端的nextseq 本端nextseq=datalen+seq 表示本端下一个数据包的seq还是1               
- 第四个数据  seq=1        ack=1      PSH,ACK      datalen=253             nextseq=254                //本端发送254字节数据,ack=对端的nextseq 本端nextseq=datalen+seq nextseq表示本端下一个数据包的sqe的值=254 表示对端的下一个数据包ack的值等于254,这里发送ACK用来再次确认本端收到了序号为ACK所示的数据包
- 第八个数据  seq=254      ack=1520   ACK          datalen=0               nextseq=254                //本端确定收到数据,收到的数据长度为本端当前ack-前一个ack的值(1520-1=1519),ack=对端的nextseq 本端nextseq=datalen+seq 表示本端下一个数据包的seq还是254
- 第九个数据  seq=254      ack=1520   PSH,ACK      datalen=346             nextseq=600                //本端发送346字节数据,ack=对端的nextseq 本端nextseq=datalen+seq nextseq表示本端下一个数据包的sqe的值=600 表示对端的下一个数据包ack的值等于600,这里发送ACK用来再次确认本端收到了序号为ACK所示的数据包
- 第11个数据  seq=600      ack=1599   PSH,ACK      datalen=505             nextseq=1105               //本端发送505字节数据,确认接收数据长度为本端当前ack-前一个ack的值(1599-1520=79) ack=对端的nextseq 本端nextseq=datalen+seq nextseq表示本端下一个数据包的sqe的值=1105 表示对端的下一个数据包ack的值等于1105,这里发送ACK用来确认本端收到了ack=1599的数据包(即对端的nextseq=1599的数据包)
- 第13个数据  seq=1105     ack=1816   PSH,ACK      datalen=57              nextseq=1162               //本端发送57字节数据,确认接收数据长度为本端当前ack-前一个ack的值(1816-1599=217) ack=对端的nextseq 本端nextseq=datalen+seq nextseq表示本端下一个数据包的sqe的值=1162 表示对端的下一个数据包ack的值等于1162,这里发送ACK用来确认本端收到了ack=1816的数据包(即对端的nextseq=1816的数据包)
- 第14个数据  seq=1162     ack=1816   FIN,ACK      datalen=0               nextseq=1163               //本端发送FIN,属于四次挥手的第一次挥手,ack=对端的nextseq 本端nextseq=seq+1 nextseq表示本端下一个数据包的seq还是1163,表示对端的下一个数据包ack的值等于1163,这里发送ACK用来再次确认本端收到了序号为ACK所示的数据包
- 第18个数据  seq=1163     ack=1817   ACK          datalen=0               nextseq=1163               //本端接收到对端的FIN并回复ACK报文,FIN属于四次挥手的第四次挥手,并没有收到RST报文(如果收到则不会回复ACK报文) RST是对端要求终止连接,ack=对端的nextseq 本端nextseq=seq nextseq表示本端下一个数据包的seq还是1163,表示对端的下一个数据包ack的值等于1816,这里发送ACK用来确认本端收到了ack=1163的数据包(即对端的nextseq=1163的数据包)


>SourceIP=10.19.12.123 DestinationIP=10.19.11.65
- 第二个数据  seq=0        ack=1      SYN,ACK      datalen=0               nextseq=1                  //首次建立tcp链接,属于三次握手的第二次握手 ack=对端的nextseq 本端nextseq=1表示本端下一个数据包的seq=1
- 第五个数据  seq=1        ack=254    ACK          datalen=0               nextseq=1                  //本端接收数据并发送独立的确定数据包,收到的数据长度为本端当前ack-前一个ack的值(254-1=253),ack=对端nextseq 本端nextseq=datalen+1 nextseq表示本端下一个数据包的的seq值=1
- 第六个数据  seq=1        ack=254    PSH          datalen=1460            nextseq=1461               //本端独立发送1460字节数据,ack=对端nextseq 本端nextseq=datalen+seq nextseq表示本端下一个数据包的seq=1461 表示对端的下一个数据包ack的值等于1461
- 第七个数据  seq=1461     ack=254    PSH,ACK      datalen=59              nextseq=1520               //本端发送59字节数据,ack=对端nextseq 本端nextseq=datalen+seq   nextseq表示本端下一个数据包的seq=1520 表示对端的下一个数据包ack的值等于1520,这里发送ACK用来再次确认本端收到了序号为ACK所示的数据包
- 第10个数据  seq=1520     ack=600    PSH,ACK      datalen=79              nextseq=1599               //本端发送79字节数据,收到的数据长度为本端当前ack-前一个ack的值(600-254=346),ack=对端nextseq 本端nextseq=datalen+seq   nextseq表示本端下一个数据包的seq=1599 表示对端的下一个数据包ack的值等于1599,这里发送ACK用来确认本端收到了ack=600的数据包(即对端的nextseq=600的数据包)
- 第12个数据  seq=1599     ack=1105   PSH,ACK      datalen=217             nextseq=1816               //本端发送217字节数据,收到的数据长度为本端当前ack-前一个ack的值(1105-600=505),ack=对端nextseq 本端nextseq=datalen+seq   nextseq表示本端下一个数据包的seq=1816 表示对端的下一个数据包ack的值等于1816,这里发送ACK用来确认本端收到了ack=1105的数据包(即对端的nextseq=1105的数据包)
- 第15个数据  seq=1816     ack=1163   ACK          datalen=0               nextseq=1816               //本端收到FIN包后发送ACK数据包,属于四次挥手的第二次挥手,ack=对端nextseq 本端nextseq=datalen+seq  nextseq表示本端下一个数据包的seq=1816 表示对端的下一个数据包ack的值等于1816
- 第16个数据  seq=1816     ack=1163   FIN,ACK      datalen=0               nextseq=1817               //本端发送FIN数据包,属于四次挥手的第三次挥手,ack=对端nextseq 本端nextseq=seq+1  nextseq表示本端下一个数据包的seq=1817 表示对端的下一个数据包ack的值等于1817,这里发送ACK用来再次确认本端收到了序号为ACK所示的数据包
- 第17个数据  seq=1817     ack=1163   RST,ACK      datalen=0               nextseq=1817               //本端发送RST数据包来终止连接(当服务端发送RST时,四次挥手的过程被中断,并且连接直接终止,不再继续执行剩余的步骤),ack=对端nextseq 本端nextseq=seq(RST的时候)  
- 第19个数据  seq=1817     ack=0      RST          datalen=0               nextseq=1817               //本端发送第二次RST数据包来确认连接是否已经断开,ack=0(第二次RST) 本端nextseq=seq(RST的时候) 整条tcp数据结束。

## Win
Win 表示当前窗口的大小。在TCP协议中,当两端进行数据传输时,为防止数据积压,造成网络拥堵,需要使用窗口来限制传输的数据量。Win字段的值表示当前发送端允许接收端发送的数据包的最大大小,也就是窗口大小。接收端收到Win字段值后,如果超过了该大小,就会对数据进行抛弃处理。


# Wireshark学习
## NBNS   
> NBNS是NetBIOS Name Service的缩写,是一种协议,用于在小型局域网络上识别计算机的名称。当Wireshark抓包时,如果它检测到NetBIOS Name Service协议正在使用,它将在protocol字段中显示为NBNS。
``` data
    NetBIOS Name Service                                                            
        Transaction ID: 0xde50                                                                                      //用于标识此NBNS请求。
        Flags: 0x0010, Opcode: Name query, Broadcast                                                                //标志位,用于标识此NBNS请求的类型。
            0... .... .... .... = Response: Message is a query                                                      //0表示请求,1表示响应
            .000 0... .... .... = Opcode: Name query (0)                                                            //请求类型
            .... ..0. .... .... = Truncated: Message is not truncated                                               //0表示未截断,1表示截断
            .... ...0 .... .... = Recursion desired: Don't do query recursively                                     //0表示不递归,1表示递归
            .... .... ...1 .... = Broadcast: Broadcast packet                                                       //0表示单播,1表示广播
        Questions: 1                                                                                                //问题数1个
        Answer RRs: 0                                                                                               //回答数0个
        Authority RRs: 0                                                                                            //授权数0个
        Additional RRs: 0                                                                                           //附加数0个
        Queries                                                                                                     //查询
            *<00><00><00><00><00><00><00><00><00><00><00><00><00><00><00>: type NBSTAT, class IN                    //查询内容为 *<00><00><00><00><00><00><00><00><00><00><00><00><00><00><00> ,类型为NBSTAT,类别为IN
                Name: *<00><00><00><00><00><00><00><00><00><00><00><00><00><00><00> (Workstation/Redirector)        //查询的主机名
                Type: NBSTAT (33)                                                                                   //查询的类型
                Class: IN (1)                                                                                       //查询的类别
    //这是一个工作站/重定向器名称,表示请求的是工作站/重定向器的状态信息。
```

## ICMP
> ICMP是Internet Control Message Protocol的缩写,是一种用于网络诊断、错误报告和状态报告的协议,用于在IP网络中传输控制消息。当Wireshark抓包时,如果它检测到ICMP协议正在使用,它将在protocol字段中显示为ICMP。
``` data
    Internet Control Message Protocol                                                                           
        Type: 3 (Destination unreachable)                                                           //ICMP类型为3,表示目的地不可达
        Code: 3 (Port unreachable)                                                                  //ICMP代码为3,表示端口不可达
        Checksum: 0x45c3 [correct]                                                                  //校验和为0x45c3,正确
        [Checksum Status: Good]                                                                     //校验和状态为Good
        Unused: 00000000                                                                            //未使用
        Internet Protocol Version 4, Src: 10.18.11.65, Dst: 10.19.12.123                            //源IP地址为10.18.11.65 ,目的IP地址为10.19.12.123 
            0100 .... = Version: 4                                                                  //版本为4
            .... 0101 = Header Length: 20 bytes (5)                                                 //头部长度为20字节
            Differentiated Services Field: 0x00 (DSCP: CS0, ECN: Not-ECT)                           //区分服务字段为0x00,DSCP为CS0,ECN为Not-ECT
                0000 00.. = Differentiated Services Codepoint: Default (0)                          //区分服务代码点为Default
                .... ..00 = Explicit Congestion Notification: Not ECN-Capable Transport (0)         //显式拥塞通知为Not ECN-Capable Transport
            Total Length: 78                                                                        //总长度为78
            Identification: 0xafd3 (45011)                                                          //标识为0xafd3
            Flags: 0x0000                                                                           //标志为0x0000
                0... .... .... .... = Reserved bit: Not set                                         //保留位为0
                .0.. .... .... .... = Don't fragment: Not set                                       //不分片位为0
                ..0. .... .... .... = More fragments: Not set                                       //更多分片位为0
            Fragment offset: 0                                                                      //分片偏移为0
            Time to live: 62                                                                        //生存时间为62
            Protocol: UDP (17)                                                                      //协议为UDP
            Header checksum: 0xa0eb [validation disabled]                                           //头部校验和为0xa0eb,校验和状态为validation disabled
            [Header checksum status: Unverified]                                                    //头部校验和状态为Unverified
            Source: 10.18.11.65                                                                     //源IP地址为
            Destination: 10.19.12.123                                                               //目的IP地址为
        User Datagram Protocol, Src Port: 137, Dst Port: 137                                        //源端口为137,目的端口为137
            Source Port: 137                                                                        //源端口为137
            Destination Port: 137                                                                   //目的端口为137
            Length: 58                                                                              //长度为58
            Checksum: 0xb5ed [unverified]                                                           //校验和为0xb5ed,校验和状态为unverified
            [Checksum Status: Unverified]                                                           //校验和状态为Unverified
            [Stream index: 13]                                                                      //流索引为13
```

## TCP
> Frame 4470: 54 bytes on wire (432 bits), 54 bytes captured (432 bits) on interface \Device\NPF_{E8A578A1-14C3-44BF-BEA9-132860C75D38}, id 0

- Frame 4470 是 Wireshark 抓到的一个网络帧的编号。
- "54 bytes on wire (432 bits)"表示该帧在网络上传输了54字节的数据,占用432 bits的网络带宽。
- "54 bytes captured (432 bits) on interface \Device\NPF_{E8A578A1-14C3-44BF-BEA9-132860C75D38}",表示该帧在接口 \Device\NPF_{E8A578A1-14C3-44BF-BEA9-132860C75D38} 上被抓到了54字节的数据,占用了432 bits的带宽。
- "id 0" 是 Wireshark 内部使用的该帧的标识。

``` data
    Interface id: 0 (\Device\NPF_{E8A578A1-14C3-44BF-BEA9-132860C75D38})            //网络接口的标识符(以此表示该数据包是从哪个接口抓取的)。
        Interface name: \Device\NPF_{E8A578A1-14C3-44BF-BEA9-132860C75D38}          //网络接口的名称
        Interface description: 以太网                                               //网络接口的描述
    Encapsulation type: Ethernet (1)                                                //数据帧的封装类型
    Arrival Time: Feb  2, 2023 15:22:37.837878000 中国标准时间                      // 数据包到达的时间
    [Time shift for this packet: 0.000000000 seconds]                               //数据帧的时间偏移量
    Epoch Time: 1675322557.837878000 seconds                                        //从 1970 年 1 月 1 日的秒数的数据帧的时间
    [Time delta from previous captured frame: 0.007533000 seconds]                  //与上一个捕获的数据帧的时间差。
    [Time delta from previous displayed frame: 0.010663000 seconds]                 //与上一个显示的数据帧的时间差
    [Time since reference or first frame: 1.788612000 seconds]                      //从引用或第一帧的时间
    Frame Number: 215                                                               //数据帧的编号
    Frame Length: 307 bytes (2456 bits)                                             //数据帧的长度,以字节和比特表示
    Capture Length: 307 bytes (2456 bits)                                           //捕获的数据帧的长度,以字节和比特表示
    [Frame is marked: False]                                                        //表示这个数据包是否被标记。False 表示没有被标记,True 表示已经被标记。
    [Frame is ignored: False]                                                      //表示这个数据包是否被忽略。False 表示不被忽略,True 表示被忽略。
    [Protocols in frame: eth:ethertype:ip:tcp:tls]                                  //表示在这个数据包中出现的协议层。从上往下依次为以太网、以太网类型、IP、TCP 和 TLS。
    [Coloring Rule Name: TCP]                                                       //表示这个数据包被 Wireshark 的颜色规则分类到了什么
    [Coloring Rule String: tcp]                                                     //表示这个数据包被分类到 TCP 的规则字符串
```

> Ethernet II, Src: 00:cf:e0:5b:74:3c (00:cf:e0:5b:74:3c), Dst: HuaweiTe_ba:dd:e2 (84:46:fe:ba:dd:e2)
- "Ethernet II"是Ethernet帧结构的一种。该行表示捕获的帧类型是Ethernet II。
``` data
    //这是以太网帧头部分,包括了目标地址（Destination）和源地址（Source）,以及以太网帧类型（Type）
    Destination: HuaweiTe_ba:dd:e2 (84:46:fe:ba:dd:e2)                                          //指明报文的目标主机
        Address: HuaweiTe_ba:dd:e2 (84:46:fe:ba:dd:e2)                                          
        .... ..0. .... .... .... .... = LG bit: Globally unique address (factory default)       //用于指示 MAC 地址的类型
        .... ...0 .... .... .... .... = IG bit: Individual address (unicast)                    //用于指示 MAC 地址的类型
    Source: 00:cf:e0:5b:74:3c (00:cf:e0:5b:74:3c)                                               //指明报文的源主机
        Address: 00:cf:e0:5b:74:3c (00:cf:e0:5b:74:3c)
        .... ..0. .... .... .... .... = LG bit: Globally unique address (factory default)       //LG bit 为 0 时表示是全球唯一地址
        .... ...0 .... .... .... .... = IG bit: Individual address (unicast)                    //IG bit 为 0 时表示是单播地址
    Type: IPv4 (0x0800)                                                          //用于识别数据帧所封装的上层协议,此处为 IPv4,即上层协议为 IP 协议。
```



- Src：00:cf:e0:5b:74:3c（00:cf:e0:5b:74:3c）：表示源 MAC 地址,即数据帧从这个地址发送。

- Dst：HuaweiTe_ba:dd:e2（84:46:fe:ba:dd:e2）：表示目的 MAC 地址,即数据帧将被发送到这个地址。

> Internet Protocol Version 4, Src: 10.18.11.65, Dst: 10.19.12.123
-   使用了互联网协议版本4(IPv4),源地址为10.18.11.65,目标地址为10.19.12.123

```data
    //这是Wireshark捕获到的一个IP数据包的详细信息
    0100 .... = Version: 4                                                      //这表示IP数据包的版本是IPv4。
    .... 0101 = Header Length: 20 bytes (5)                                     //这表示IP数据包的首部长度为20字节
    Differentiated Services Field: 0x00 (DSCP: CS0, ECN: Not-ECT)               //这是一个8位字段,用于指示网络服务质量(QoS),其中最高位2位代表ECN,而接下来的6位代表DSCP。在这个例子中,ECN为Not-ECT,DSCP为CS0,表示最低优先级的服务。
    0000 00.. = Differentiated Services Codepoint: Default (0)                  //是一种在IP包头的8位ToS字段中表示网络服务质量的方法。它被用于标识不同的网络服务类型,以便在网络中对不同的数据流进行不同的处理。默认的值为 "0000 00..",表示使用默认的服务质量。在这种情况下,DSCP 的值为 0,意味着该 IP 包将按照网络中的默认设置进行处理,不会得到特殊的处理。
    .... ..00 = Explicit Congestion Notification: Not ECN-Capable Transport (0) //是一种在IP网络中对拥塞管理的方法。在ECN中,路由器可以在不丢弃数据包的情况下标记网络拥塞,以便在终端上采取预防措施,例如降低发送速率。当前标识表示运输层（例如TCP）不支持ECN。
    Total Length: 293                                                           //这是IP数据包的总长度,单位为字节。
    Identification: 0xab08 (43784)                                              //这是一个16位字段,用于标识同一个数据报的分片（如果数据报的长度超过了传输路径的最大长度,则必须分为多个分片）。
    Flags: 0x4000, Don't fragment                                               //这是一个3位字段,指示是否允许分片。Don't fragment字段被设置为1,表示不允许分片。
        0... .... .... .... = Reserved bit: Not set                             //是 IPv4 标头中的一个位,保留供将来使用。
        .1.. .... .... .... = Don't fragment: Set                               //表示该IP数据报不能被分段传输。当接收方的网络不能容纳整个数据报时,如果该位被设置,则该数据报将被丢弃,而不是分段
        ..0. .... .... .... = More fragments: Not set                           //表示该数据报是该数据流的最后一个分段数据报。如果该位被设置,表示该数据报不是该数据流的最后一个分段数据报。
    Fragment offset: 0                                                          //该字段表示当前分片在整个 IP 数据包中的偏移位置,以 8 字节为单位。如果为 0,则表示当前帧是整个数据包的第一个分片。
    Time to live: 64                                                            //TTL是一个 8 位字段,它指定 IP 包最多允许经过的路由器数量。每经过一个路由器,TTL 的值就会减 1。当 TTL 的值为 0 时,该 IP 包就被视为无效并被丢弃。
    Protocol: TCP (6)                                                           //该字段指示当前数据包的上层协议类型,如 TCP（协议号为 6）
    Header checksum: 0x62ea [validation disabled]                               //IP 包头的校验和字段。此字段用于检测 IP 包头中的数据是否被破坏,以确保 IP 包的完整性。
    [Header checksum status: Unverified]                                        //表示校验和字段未被验证。如果 Wireshark 可以验证校验和字段,则应该显示为 [Header checksum status: Verified]。
    Source: 10.18.11.65                                                         //数据包的源 IP 地址
    Destination: 10.19.12.123                                                   //数据包的目标 IP 地址
``` 

> Transmission Control Protocol, Src Port: 9625, Dst Port: 2443, Seq: 95659, Ack: 401212, Len: 0
- 这是一个TCP协议的报文
- Src Port：源端口,这里的值为9625。
- Dst Port：目的端口,这里的值为2443。
- Seq：序列号,这里的值为95659。它标识了这个数据包中的数据在数据流中的相对顺序。
- Ack：确认号,这里的值为401212。它标识了在对端收到的最后一个数据包的结尾。
- Len：数据长度,这里的值为0。它指示这个数据包中包含的实际数据长度。

    > 从这个TCP报文中可以看到,这是一个确认报文,其中Seq和Ack的值表示了数据流中的一些位置,而Len的值为0,表示这个数据包中没有任何实际数据。

```data
    这是一个TCP数据包的详细信息,每一个字段都是指的是特定的数据
    Source Port: 9625                                                           //源端口,即发送该数据包的端口号
    Destination Port: 2443                                                      //目标端口,即接收该数据包的端口号
    [Stream index: 5]                                                           //数据包在流中的索引 是Wireshark为了组织和管理数据流量使用的一个编号,Wireshark将一系列有关联的数据报分配到相同的Stream index中,使其在显示和分析数据流量时更方便。
    [TCP Segment Len: 253]                                                      //TCP数据段长度
    Sequence number: 1    (relative sequence number)                            //序列号,指定该数据包在TCP传输中的相对顺序,表示发送方在当前传输控制协议（TCP）连接中发送的数据的字节数
    Sequence number (raw): 439134108                                            //"raw"（即未经转换）的数字
    [Next sequence number: 254    (relative sequence number)]                   //下一个序列号
    Acknowledgment number: 1    (relative ack number)                           //确认号,指定期望接收到的下一个数据包的序列号
    Acknowledgment number (raw): 1832006081                                     //"raw"（即未经转换）的数字
    0101 .... = Header Length: 20 bytes (5)                                     //数据包头长度,单位为字节
    Flags: 0x018 (PSH, ACK)                                                     //标志位
    000. .... .... = Reserved: Not set                                          //保留位,必须为0
    ...0 .... .... = Nonce: Not set                                             //非0表示该报文不是一个随机数,用于防止重放攻击
    .... 0... .... = Congestion Window Reduced (CWR): Not set                   //非0表示发送方收到了一个ECN-Echo报文,表示接收方的拥塞窗口已经减小
    .... .0.. .... = ECN-Echo: Not set                                          //0表示该报文不是环境拥塞通知回显报文
    .... ..0. .... = Urgent: Not set                                            //非0表示该报文包含紧急数据
    .... ...1 .... = Acknowledgment: Set                                        //非0表示该报文中包含了确认报文的序列号
    .... .... 1... = Push: Set                                                  //表示该报文是“强制刷新”报文,用于强制立即将已缓存的数据发送出去
    .... .... .0.. = Reset: Not set                                             //非复位报文
    .... .... ..0. = Syn: Not set                                               //非同步报文
    .... .... ...0 = Fin: Not set                                               //非终止报文
    [TCP Flags: ·······AP···]                                                   //是对 TCP 包的标志位的描述 A：ACK,表示此 TCP 包中包含了确认编号。P：PSH,表示请求对方尽快把数据交付给应用层。
    
    Flags: 0x002 (SYN)
    .... .... ..1. = Syn: Set                                                               //表示该报文是一个同步报文,用于诊断网络流量的相关信息。它提供了关于此特定 TCP 流的详细说明。
        [Expert Info (Chat/Sequence): Connection establish request (SYN): server port 2443]     //Wireshark的一个高级功能
            [Connection establish request (SYN): server port 2443]                              //表示此TCP是一个连接请求,这意味着客户端正在试图建立到服务器2443的连接
            [Severity level: Chat]                                                              //是 Wireshark 中一种分类报告信息的方法,它将报告信息分为三种级别：Chat、Note 和 Warning。Chat 级别的报告信息是最低级别的,它们只是一些普通的信息,不需要特别注意。         
            [Group: Sequence]                                                                   //Wireshark用于分组TCP数据包的分析信息

    .... .... ...1 = Fin: Set                                                                   //表示此报文是一个终止报文,用于终止TCP连接
        [Expert Info (Chat/Sequence): Connection finish (FIN)]                                  // Wireshark 中的专家信息,表示此报文中的 TCP 连接已经结束（FIN）
            [Connection finish (FIN)]                                                           //表示此TCP是一个连接结束,这意味着客户端正在试图关闭到服务器2443的连接
            [Severity level: Chat]
            [Group: Sequence]

    .... .... .1.. = Reset: Set                                                                 //表示该报文是一个复位报文,用于复位TCP连接
        [Expert Info (Warning/Sequence): Connection reset (RST)]                                //Wireshark 中的专家信息,表示此报文中的 TCP 连接已经被复位（RST）
            [Connection reset (RST)]
            [Severity level: Warning]
            [Group: Sequence]


    Window size value: 64240                                                    //窗口大小值,指定发送方允许的数据的最大字节数
    [Calculated window size: 64240]                                             //表示在接收方允许发送方发送的最大数据量。窗口大小值是由接收方发送给发送方的,用来控制发送方发送数据的速率。发送方会持续监听窗口大小值,当其为0时则不发送数据,等到接收方告诉发送方可以再次发送数据时,发送方才会继续发送数据。
    [Window size scaling factor: -2 (no window scaling used)]                   //用于将接收方的窗口大小缩放为它的真实值。在TCP传输协议中,接收方会告诉发送方他的窗口大小,但由于传输数据的实际大小可能比发送方传输数据的限制大,因此需要使用缩放因子来缩放接收方的窗口大小,以便发送方可以知道实际可以传输的数据大小,数值为正时,表示使用了缩放因子,数值为负时,表示将不使用缩放。
    Checksum: 0xec6b [unverified]                                               //用于验证数据包的完整性。TCP数据包的校验和是用来检测数据包中数据是否有在传输过程中的错误。当接收到一个 TCP 数据包时,接收方会计算数据包中的校验和,并与数据包的校验和字段进行比较。如果校验和不匹配,则认为数据包已损坏,接收方会丢弃该数据包。
    [Checksum Status: Unverified]                                               //指TCP数据包的校验和状态未经过验证
    Urgent pointer: 0                                                           //紧急指针,指定紧急数据的位置。如果接收方收到了一个带有紧急指针的数据报,它就知道在接下来的数据流中,将有一段紧急数据,并优先处理这些数据。当Urgent pointer的值为0时,表示当前数据报中没有紧急数据。
    [SEQ/ACK analysis]                                                          //是一种 TCP 协议分析技术,用于评估两个网络节点之间的数据传输状况。SEQ/ACK 通过检查序列号和确认号的值来评估数据的流动情况。序列号表示当前数据包的顺序位置,而确认号则告诉发送方已经成功接收到的数据的末尾位置。通过分析这两个值,可以确定数据传输的正确性以及是否存在丢失或重复的数据包。
        [iRTT: 0.000494000 seconds]                                             //iRTT是指数据从源地址到目的地址,再从目的地址返回到源地址所需的时间,可以用来评估网络延迟,从而对数据传输进行优化。
        [Bytes in flight: 253]                                                  //是指正在通过网络传输的,但还没有被确认的数据字节数。在TCP中,数据发送方维护一个发送缓存,在数据被接收方确认前,这些数据都是处于“飞行”状态的。这个值对于控制 TCP 的流量很重要,因为它决定了接收方在收到下一个数据包之前,发送方还能发送多少数据。
        [Bytes sent since last PSH flag: 253]                                   //表示在最后一次使用 "PSH" 标志的TCP数据包发送以来,已经发送的字节数。因此,在每次使用 "PSH" 标志时,应该记录当前已发送的字节数。


        [This is an ACK to the segment in frame: 127]                           //指当前数据包是对第127个数据包的确认
        [The RTT to ACK the segment was: 0.000575000 seconds]                   //表示从发送数据报到收到确认数据报所用的时间,即往返时间

        [TCP Analysis Flags]                                                    //是指一组用于分析TCP协议行为的标志位,这些标志位可以用来分析TCP协议的行为,如重传、丢包、延迟等。
            [This is a TCP duplicate ack]                                       //表示当前数据包是一个重复的确认报文 重复的确认报文可能表示数据包丢失,并且发送方重新发送了该数据包


            [Expert Info (Note/Sequence): This frame is a (suspected) retransmission]   //是Wireshark中的专家信息,表示此报文是重传报文
                [This frame is a (suspected) retransmission]                            //表示此报文是重传报文
                [Severity level: Note]                                                  
                [Group: Sequence]
            [The RTO for this segment was: 0.000017000 seconds]                         //指的是该报文的重传超时（RTO）时间。   RTO是一种 TCP 协议中使用的机制,用于确定发送方何时重新发送报文。如果发送方未在预期时间内收到确认报文,则它将使用 RTO 时间来决定何时重新发送报文。
            [RTO based on delta from frame: 132]                                        //指的是 RTO 计算是基于帧132(自己是133,所以是132)中的时间差（delta）进行计算的。


        [Duplicate ACK #: 1]                                                    //重复确认的次数
        [Duplicate to the ACK in frame: 130]                                    //重复确认的数据包的序号
            [Expert Info (Note/Sequence): Duplicate ACK (#1)]                   //是Wireshark中的专家信息,表示此报文是重复确认报文,重复确认的次数为1
                [Duplicate ACK (#1)]                                            //重复确认的次数
                [Severity level: Note]                                          //专家信息的严重级别,分为Note、Warn、Error、Chat、Note、Conversation、Debug、None
                [Group: Sequence]                                               //专家信息的分组,分为Sequence、Protocol、Response、Reassemly、Chat、Conversation、Debug、None

            [Expert Info (Chat/Sequence): TCP window update]                    //是Wireshark中的专家信息,表示此报文是TCP窗口更新报文
                [TCP window update]                                             //TCP窗口更新报文
                [Severity level: Chat]
                [Group: Sequence]


    [Timestamps]                                                                //时间戳,提供有关数据包在TCP流中的时间信息,如时间戳差值和相对时间
        [Time since first frame in this TCP stream: 0.011168000 seconds]        //从第一个TCP数据包到当前数据包的时间差
        [Time since previous frame in this TCP stream: 0.010663000 seconds]     //从上一个TCP数据包到当前数据包的时间差
    TCP payload (253 bytes)                                                     //TCP负载,为数据包中的有效负载数据
    Retransmitted TCP segment data (253 bytes)




    Options: (20 bytes), Maximum segment size, No-Operation (NOP), Window scale, SACK permitted, Timestamps
    TCP Option - Maximum segment size: 1460 bytes                                                               //TCP选项,最大段大小 指定了发送方可以发送的最大数据段的大小为1460（不包括TCP首部）。这个选项字段用于消除在传输数据时可能出现的分片问题,确保数据在网络上的传输高效稳定。
        Kind: Maximum Segment Size (2)                                                                          //2是值 代表Maximum Segment Size
        Length: 4                                                                                               //长度为4。
        MSS Value: 1460                                                                                         //表示最大分段大小的值。
    TCP Option - No-Operation (NOP)                                                                             //TCP选项,是一个占位符,用于占据一个字节的空间,以防止其他选项的位置改变。
        Kind: No-Operation (1)                                                                                  //值为1,代表No-Operation
    TCP Option - Window scale: 8 (multiply by 256)                                                              //TCP选项,缩放比例因子为8,意味着实际的最大窗口大小为Window size value*256
        Kind: Window Scale (3)                                                                                  //值为3,代表Window Scale
        Length: 3                                                                                               //长度为3
        Shift count: 8                                                                                          //指将接收窗口大小的值向左移动8位的操作即就是乘以256
        [Multiplier: 256]                                                                                       //窗口大小被乘以的数
    TCP Option - SACK permitted                                                                                 //TCP选项,表示该TCP连接支持SACK,指定了发送方是否允许接收方发送SACK选项
        Kind: SACK Permitted (4)                                                                                //4是值,代表SACK Permitted。SACK是一种用于提高TCP传输效率的技术,允许接收端选择性地确认已收到的数据包。通常情况下,TCP只能简单地确认已接收到的最后一个数据包,SACK则允许接收端更细粒度地确认已收到的数据。这有助于提高网络传输效率,特别是在遇到丢包情况时。
        Length: 2                                                                                               //长度为2
    TCP Option - Timestamps: TSval 457829718, TSecr 0                                                           //TCP选项,时间戳,在TCP数据报头中包含用于提供时间戳信息。"TSval" 表示发送方的时间戳值,"TSval" 表示发送方的时间戳值,时间戳选项可以用来帮助评估数据包在网络中传输的时延,以及确定数据包是否过早或过晚到达。它也可以用于帮助诊断网络中的丢包情况。
        Kind: Time Stamp Option (8)                                                                             //8是值,代表Time Stamp Option
        Length: 10                                                                                              //长度为10
        Timestamp value: 457829718                                                                              //发送方发送数据包时的时间戳
        Timestamp echo reply: 0                                                                                 //接收方发送确认包时的时间戳


```


> [2 Reassembled TCP Segments (1465 bytes): #4355(1460), #4356(5)]
- "2 Reassembled TCP Segments" 指的是将2个数据包按照顺序重新组装成一个数据流
- "(1465 bytes)" 总大小
- "#4355(1460), #4356(5)" 序号为4355的包大小为1460,序号为4356的包大小为5 (序号是wireshark的第一列No字段)

``` data
    [2 Reassembled TCP Segments (1448 bytes): #135(1398), #136(50)]                        //2个TCP数据包被重新组装成一个数据流,总大小为1448字节,序号为135的包大小为1398,序号为136的包大小为50
        [Frame: 135, payload: 0-1397 (1398 bytes)]                                          //序号为135的包大小为1398
        [Frame: 136, payload: 1398-1447 (50 bytes)]                                         //序号为136的包大小为50
        [Segment count: 2]                                                                  //2个TCP数据包被重新组装成一个数据流
        [Reassembled TCP length: 1448]                                                      //总大小为1448字节
        [Reassembled TCP Data: 16030305a30b00059f00059c0002fc308202f830820261a0…]           //重新组装后的数据

```

> 9624 → 2443 [SYN] Seq=0 Win=64240 Len=0 MSS=1460 WS=256 SACK_PERM=1 TSval=457829718 TSecr=0

- 这是一个Wireshark抓取的TCP数据包,表示一个TCP连接请求的初始化。它的源端口是9624,目的端口是2443,消息类型是SYN(Synchronize),表示同步连接请求。
- SEQ(Sequence)值为0,表示这是一个新的连接请求,
- Win(Window)值为64240,表示接收方的最大可接收的字节数,
- MSS(Maximum Segment Size)值为1460,表示最大的报文段长度,
- WS(Window Scale)值为256,
- SACK_PERM(Selective Acknowledgment Permitted)值为1,表示接收方支持选择性确认,
- TSval(Timestamp value) 值为457829718,是发送方的时间戳值,用于指示该 TCP 包的发送时间
- TEcr(Timestamp Echo Reply)值为0,是接收方的时间戳回复,用于指示接收到该 TCP 包的时间。

> 2443 → 9624 [SYN, ACK] Seq=0 Ack=1 Win=8192 Len=0 MSS=1460
- [SYN, ACK]：这是一个SYN/ACK报文,表示对方正在同步并确认连接。
- Seq=0：此报文的序列号为0。
- Ack=1：此报文的确认号为1,表示已确认收到对方的序列号为0的数据报。
- Win=8192：此报文的窗口大小为8192。
- Len=0：此报文的长度为0。
- MSS=1460：此报文的最大报文段长度为1460。

> 9624 → 2443 [ACK] Seq=1 Ack=1 Win=64240 Len=0
- 这条网络数据包是一个ACK数据包,它用于回应2443端口发送的[SYN, ACK]数据包。
- 该数据包的SEQ字段值为1,表示9624端口已收到了从2443端口的序列号为0的数据包。
- 该数据包的ACK字段值为1,表示9624端口确认了收到了2443端口的序列号为0的数据包。
- 该数据包的Win字段值为64240,表示9624端口的接收窗口的大小为64240字节。
- Len字段的值为0,表示该数据包没有附带任何数据。

## [TCP Out-Of-Order]
    "[TCP Out-Of-Order]"是Wireshark中的一个特殊标识,指示当前数据包与正常的TCP数据流不在同一顺序中。
- 为什么会出现
    >这通常是由于网络中的丢包或抖动导致的。当数据包在网络中传输时,它们可能因为某些原因在路由器上的排队,并且当到达目的地时,它们的顺序可能已经改变。TCP 协议在接收端需要重新排序这些数据包,以确保正确的数据顺序。
- 有什么影响
    > 例如：
        1. 减缓数据传输速度：因为必须等待丢失的数据包重传或确认。
        2. 导致数据包重复或丢失：如果顺序错误的数据包发送多次,接收方可能会重复接收相同的数据包。
        3. 导致应用层错误：如果接收到的数据不按预期的顺序,应用程序可能无法正确处理数据。
        4. 因此,TCP 协议通过使用序列号和确认来维护数据的正确顺序,避免出现这样的情况。

### 怎么避免出现

- 减小数据包的大小：减小数据包的大小可以降低包的丢失率,从而减少 Out-Of-Order 的发生。
- 使用数据包重传：如果发现数据包丢失,可以使用重传来保证数据的正确性。
- 增加数据包缓存：可以在接收方缓存数据包,以保证数据的顺序。
- 使用流量控制：通过流量控制来保证数据包的发送速度与接收方的处理速度相匹配。

## [TCP Dup ACK 130#1]

    指在一次TCP会话中,发送端重复发送了确认包（ACK）给接收端。这种情况一般是由于网络延迟或者数据包丢失导致接收端未收到发送端的确认。
    为了避免出现这种情况,常见的方法包括：
    - 增加网络带宽：这可以减少数据包丢失的概率,从而避免TCP Dup ACK的出现。
    - 使用网络优化算法：例如拥塞控制算法,可以动态调整窗口大小,避免因网络拥堵导致的TCP Dup ACK。
    - 监测网络状况：及时监测网络状况,发现问题及时处理,也可以避免TCP Dup ACK的出现。
    - 避免使用不必要的数据包：减少不必要的数据包,也可以避免TCP Dup ACK的出现。

## [TCP retransmission]

    指TCP协议在发送端重新发送丢失的数据包的过程。这是由于网络中的丢包导致的。因此,在发送端,如果没有收到应答或确认包,则会重新发送丢失的数据包,以确保数据的完整性和可靠性。

## [TCP Window Update]

    表示在 TCP 通信中,发送方的可用窗口大小已经更新。

## [TCP Window Full]

    它表明发送方到接收方的数据传输暂时需要停止，因为接收方的接收缓冲区已满，无法再接收更多数据。发送方就会发送TCP Window Full

## Client Hello

``` data
Transport Layer Security                                                                    //TLS是在传输层上为应用程序提供安全性的协议。
    TLSv1.2 Record Layer: Handshake Protocol: Client Hello                                  //表示在这个TLS会话中,客户端发送了一个 "Client Hello" 消息。在 TLS 中,"Client Hello" 消息是客户端发送给服务器的第一个消息,用于向服务器请求建立一个安全的连接。该消息包含了客户端支持的协议版本、加密套件、随机数等信息,服务器根据这些信息来决定是否能够建立一个安全的连接。
        Content Type: Handshake (22)                                                        //值为22,表示这个数据包的内容类型是握手协议。
        Version: TLS 1.0 (0x0301)                                                           //表示使用的TLS协议版本是TLS 1.0,版本代码为0x0301
        Length: 248                                                                         //表示这个数据包的长度是248字节。
        Handshake Protocol: Client Hello                                                    //表示这个数据包的内容是客户端的握手协议。
            Handshake Type: Client Hello (1)                                                //值为1,表示这个握手协议是客户端的握手协议。
            Length: 244                                                                     //表示这个握手协议的长度是244字节。
            Version: TLS 1.2 (0x0303)                                                       //表示使用的TLS协议版本是TLS 1.2,版本代码为0x0303
            Random: 4d1b03edf4c90b9bc05d082e9d5865198f8cf94960611101…                       //表示客户端生成的随机数,长度为32字节,包含下面2个字段。
                GMT Unix Time: Dec 29, 2010 17:48:29.000000000 中国标准时间                  //表示客户端生成随机数的时间,这个时间是从1970年1月1日0时0分0秒开始计算的秒数。
                Random Bytes: f4c90b9bc05d082e9d5865198f8cf949606111014510caff…             //表示客户端生成的随机数,长度为28字节。
            Session ID Length: 32                                                           //表示客户端生成的会话ID的长度是32字节。
            Session ID: 8bc8740acc9f3515296bf18ee556960faf873b29de4d1131…                   //表示客户端生成的会话ID,长度为32字节。
            Cipher Suites Length: 20                                                        //表示客户端支持的加密套件的长度是20字节。
            Cipher Suites (10 suites)                                                       //表示客户端支持的加密套件,共10个。
                Cipher Suite: TLS_AES_256_GCM_SHA384 (0x1302)                               //使用AES-256加密算法,使用GCM模式对数据进行认证和加密,并使用SHA384哈希算法进行消息认证。
                Cipher Suite: TLS_CHACHA20_POLY1305_SHA256 (0x1303)                         //使用ChaCha20加密算法,使用Poly1305模式对数据进行认证和加密,并使用SHA256哈希算法进行消息认证。CHACHA20是一种对称加密算法,用于加密数据。POLY1305是一种用于认证数据的散列算法。SHA256是一种哈希算法,用于生成消息摘要。
                Cipher Suite: TLS_AES_128_GCM_SHA256 (0x1301)                               //使用AES-128加密算法,使用GCM模式对数据进行认证和加密,并使用SHA256哈希算法进行消息认证。
                Cipher Suite: TLS_RSA_WITH_AES_256_CBC_SHA (0x0035)                         //它通过使用 RSA 加密算法来验证客户端和服务器之间的身份,并通过 AES 256-bit 对称加密算法以 CBC 模式加密传输数据,使用 SHA-1 消息摘要算法计算数据的消息摘要。这个套件已经不被广泛推荐使用了,因为它不够安全
                Cipher Suite: TLS_RSA_WITH_AES_128_CBC_SHA (0x002f)                         //使用非对称加密 RSA 和对称加密 AES 的密码套件,AES 的密钥大小为 128 位,并使用 CBC 模式,消息验证代码 (MAC) 算法为 SHA-1,用于保证数据完整性。
                Cipher Suite: TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA (0xc014)                   //使用非对称加密算法ECDHE（Elliptic Curve Diffie-Hellman）来建立安全密钥,并使用AES256加密算法加密数据流,最后使用SHA（Secure Hash Algorithm）生成数字签名,以确保数据的完整性。
                Cipher Suite: TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA (0xc013)                   //使用非对称加密算法 RSA 和对称加密算法 AES,并使用加密哈希算法 SHA-1 来确保数据的完整性和安全性。其中,ECDHE 表示使用的是椭圆曲线密钥交换（Elliptic Curve Diffie-Hellman）,RSA 指的是签名算法,AES_128_CBC 表示使用的是 128 位的 AES 对称加密,并采用密码分组链接模式。
                Cipher Suite: TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 (0xc02f)                //提供了客户端和服务器之间的对称加密,并使用Elliptic Curve Diffie-Hellman密钥交换来生成临时密钥,以加密数据。 AES 128 GCM是对称加密算法,SHA-256是散列函数。服务器证书需要使用RSA加密算法签名,以保证服务器身份的安全。
                Cipher Suite: TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384 (0xc030)                //它使用了非对称加密算法ECDHE、RSA认证、AES-256加密算法以及GCM模式、SHA-384散列算法。非对称加密算法ECDHE保证了客户端和服务器之间的密钥协商过程的安全性,RSA认证则保证了客户端访问的是正确的服务器。AES-256加密算法保证了数据传输的安全性,GCM模式保证了数据的完整性,SHA-384散列算法则用于保证消息的完整性。
                Cipher Suite: TLS_EMPTY_RENEGOTIATION_INFO_SCSV (0x00ff)                    //TLS_EMPTY_RENEGOTIATION_INFO_SCSV是一个特殊的密码套件,它的作用是防止SSL中的中间人攻击。当客户端和服务器之间的连接已经建立起来之后,如果客户端想要重新建立连接,那么客户端就会发送一个TLS_EMPTY_RENEGOTIATION_INFO_SCSV密码套件,这样服务器就会知道客户端想要重新建立连接,然后服务器就会重新发送一个服务器证书,客户端就可以验证服务器证书的合法性,从而保证了连接的安全性。  是一种特殊的密码套件,它不提供加密功能,而是专门用于防止重新协商攻击。它的作用是确保在客户端和服务器之间的重新协商过程中,不存在安全漏洞。如果服务器支持这种密码套件,则客户端可以在协商的过程中发送空的重新协商信息,以证明客户端支持重新协商信息。
            Compression Methods Length: 1                                                   //压缩方法长度 即有多少个压缩方法在此握手消息中被使用。其值是一个字节,指定了后续的压缩方法的数量
            Compression Methods (1 method)                                                  //有一个压缩方法
                Compression Method: null (0)                                                //表示不使用任何压缩方法,"null" 被用作一个特殊的标识符,表示不进行数据压缩。
            Extensions Length: 151                                                          //扩展长度
            Extension: ec_point_formats (len=4)                                             //ec_point_formats是TLS协议的一种扩展,其中包含客户端对椭圆曲线公钥点格式支持的信息。在支持椭圆曲线密码体制的TLS连接中,它用于通知服务器客户端所支持的椭圆曲线公钥点格式。客户端可以在扩展中指定支持的格式,并请求服务器使用这些格式。
                Type: ec_point_formats (11)                                                 //值为11,表示ec_point_formats扩展
                Length: 4                                                                   //长度
                EC point formats Length: 3                                                  //椭圆曲线公钥点格式长度
                Elliptic curves point formats (3)                                           //椭圆曲线公钥点格式                               
                    EC point format: uncompressed (0)                                       //椭圆曲线公钥点格式为未压缩格式
                    EC point format: ansiX962_compressed_prime (1)                          //椭圆曲线公钥点格式为压缩格式
                    EC point format: ansiX962_compressed_char2 (2)                          //椭圆曲线公钥点格式为压缩格式
            Extension: supported_groups (len=14)                                            //supported_groups扩展是一种 TLS 协议中的客户端消息（Client Hello）中的扩展,它列出了客户端支持的密码套件的组。在 TLS 协商过程中,服务器将使用客户端支持的组中的一个来创建一个共享密钥,用于加密数据传输。
                Type: supported_groups (10)                                                 //值为10,表示supported_groups扩展                                  
                Length: 14                                                                  //长度14
                Supported Groups List Length: 12                                            //加密套件列表长度
                Supported Groups (6 groups)                                                 //表示客户端支持的公钥组的数量为 6 个。
                    Supported Group: x25519 (0x001d)                                        //表示支持 x25519 公钥组,并且在 TLS 协议中标识为 0x001d。
                    Supported Group: secp256r1 (0x0017)                                     //表示支持 secp256r1 公钥组,并且在 TLS 协议中标识为 0x0017。
                    Supported Group: x448 (0x001e)                                          //表示支持 x448 公钥组,并且在 TLS 协议中标识为 0x001e。
                    Supported Group: secp521r1 (0x0019)                                     //表示支持 secp521r1 公钥组,并且在 TLS 协议中标识为 0x0019。
                    Supported Group: secp384r1 (0x0018)                                     //表示支持 secp384r1 公钥组,并且在 TLS 协议中标识为 0x0018。
                    Supported Group: Unknown (0x00f9)                                       //表示客户端支持的一个未知的公钥组,并且在 TLS 协议中标识为 0x00f9。
            Extension: encrypt_then_mac (len=0)                                             //该扩展是TLS协议中的加密验证顺序方式,其中加密数据首先进行加密,再进行验证。验证通过后再对数据进行解密。
                Type: encrypt_then_mac (22)                                                 //值为22,表示encrypt_then_mac扩展
                Length: 0                                                                   //长度
            Extension: extended_master_secret (len=0)                                       //该扩展是TLS协议中的扩展主密钥扩展,该扩展用于在TLS握手过程中,对主密钥进行扩展,以提高安全性。
                Type: extended_master_secret (23)                                           //值为23,表示extended_master_secret扩展
                Length: 0                                                                   //长度
            Extension: post_handshake_auth (len=0)                                          //该扩展是TLS协议中的后握手认证扩展,该扩展用于在TLS握手过程中,对客户端进行后握手认证。
                Type: post_handshake_auth (49)                                              //值为49,表示post_handshake_auth扩展
                Length: 0                                                                   //长度
            Extension: signature_algorithms (len=48)                                        //该扩展是TLS协议中的签名算法扩展,该扩展用于在TLS握手过程中,对客户端支持的签名算法进行描述。
                Type: signature_algorithms (13)                                             //值为13,表示signature_algorithms扩展
                Length: 48                                                                  //长度
                Signature Hash Algorithms Length: 46                                        //签名算法列表长度
                Signature Hash Algorithms (23 algorithms)                                   //表示客户端支持的签名算法的数量为 23 个。
                    Signature Algorithm: ecdsa_secp256r1_sha256 (0x0403)                    //表示支持 ecdsa_secp256r1_sha256 签名算法,并且在 TLS 协议中标识为 0x0403。ecdsa_secp256r1_sha256由ECDSA和SHA256组成,其中ECDSA是椭圆曲线数字签名算法,SHA256是SHA2算法的一种,其摘要长度为256位。secp256r1是一种椭圆曲线,用来创建一个共享密钥,用于加密数据传输。
                        Signature Hash Algorithm Hash: SHA256 (4)                           //表示签名算法的哈希算法为 SHA256,其值为 4。
                        Signature Hash Algorithm Signature: ECDSA (3)                       //表示签名算法的签名算法为 ECDSA,其值为 3。
                    Signature Algorithm: ecdsa_secp384r1_sha384 (0x0503)                    //表示支持 ecdsa_secp384r1_sha384 签名算法,并且在 TLS 协议中标识为 0x0503。ecdsa_secp384r1_sha384由ECDSA和SHA384组成,其中ECDSA是椭圆曲线数字签名算法,SHA384是SHA2算法的一种,其摘要长度为384位。secp384r1是一种椭圆曲线,用来创建一个共享密钥,用于加密数据传输。
                        Signature Hash Algorithm Hash: SHA384 (5)                           //表示签名算法的哈希算法为 SHA384,其值为 5。
                        Signature Hash Algorithm Signature: ECDSA (3)                       //表示签名算法的签名算法为 ECDSA,其值为 3。
                    Signature Algorithm: ecdsa_secp521r1_sha512 (0x0603)                    //表示支持 ecdsa_secp521r1_sha512 签名算法,并且在 TLS 协议中标识为 0x0603。ecdsa_secp521r1_sha512由ECDSA和SHA512组成,其中ECDSA是椭圆曲线数字签名算法,SHA512是SHA2算法的一种,其摘要长度为512位。secp521r1是一种椭圆曲线,用来创建一个共享密钥,用于加密数据传输。
                        Signature Hash Algorithm Hash: SHA512 (6)                           //表示签名算法的哈希算法为 SHA512,其值为 6。
                        Signature Hash Algorithm Signature: ECDSA (3)                       //表示签名算法的签名算法为 ECDSA,其值为 3。
                    Signature Algorithm: ed25519 (0x0807)                                   //表示支持 ed25519 签名算法,并且在 TLS 协议中标识为 0x0807。ed25519签名算法使用的是EdDSA,但具体算法不可知
                        Signature Hash Algorithm Hash: Unknown (8)                          //表示证书使用的哈希算法的类型是未知的,由于标识码 "8" 未在常见的哈希算法标识码中出现过,因此说明这些算法不常见或不受支持。
                        Signature Hash Algorithm Signature: Unknown (7)                     //表示证书使用的签名算法的类型是未知的,由于标识码 "7" 未在常见的签名算法标识码中出现过,因此说明这些算法不常见或不受支持。
                    Signature Algorithm: ed448 (0x0808)                                     //表示支持 ed448 签名算法,并且在 TLS 协议中标识为 0x0808。ed448签名算法使用的是EdDSA,但具体算法不可知
                        Signature Hash Algorithm Hash: Unknown (8)                          //表示证书使用的哈希算法的类型是未知的,由于标识码 "8" 未在常见的哈希算法标识码中出现过,因此说明这些算法不常见或不受支持。
                        Signature Hash Algorithm Signature: Unknown (8)                     //表示证书使用的哈希算法的类型是未知的,由于标识码 "8" 未在常见的哈希算法标识码中出现过,因此说明这些算法不常见或不受支持。
                    Signature Algorithm: rsa_pss_pss_sha256 (0x0809)                        //表示该签名算法使用 RSA-PSS 和 SHA-256 算法。RSA-PSS是一种RSA签名算法,它在消息摘要上添加随机数以保证签名的安全性。SHA-256是哈希函数的一种,是从SHA-2系列算法中的一个。该算法的摘要长度为256位。
                        Signature Hash Algorithm Hash: Unknown (8)                          //表示证书使用的哈希算法的类型是未知的,由于标识码 "8" 未在常见的哈希算法标识码中出现过,因此说明这些算法不常见或不受支持。
                        Signature Hash Algorithm Signature: Unknown (9)                     //表示证书使用的签名算法的类型是未知的,由于标识码 "9" 未在常见的签名算法标识码中出现过,因此说明这些算法不常见或不受支持。
                    Signature Algorithm: rsa_pss_pss_sha384 (0x080a)                        //表示该签名算法使用 RSA-PSS 和 SHA-384 算法。RSA-PSS是一种RSA签名算法,它在消息摘要上添加随机数以保证签名的安全性。SHA-384是哈希函数的一种,是从SHA-2系列算法中的一个。该算法的摘要长度为384位。
                        Signature Hash Algorithm Hash: Unknown (8)                          //表示证书使用的哈希算法的类型是未知的,由于标识码 "8" 未在常见的哈希算法标识码中出现过,因此说明这些算法不常见或不受支持。
                        Signature Hash Algorithm Signature: Unknown (10)                    //表示证书使用的签名算法的类型是未知的,由于标识码 "10" 未在常见的签名算法标识码中出现过,因此说明这些算法不常见或不受支持。
                    Signature Algorithm: rsa_pss_pss_sha512 (0x080b)                        //表示该签名算法使用 RSA-PSS 和 SHA-512 算法。RSA-PSS是一种RSA签名算法,它在消息摘要上添加随机数以保证签名的安全性。SHA-512是哈希函数的一种,是从SHA-2系列算法中的一个。该算法的摘要长度为512位。
                        Signature Hash Algorithm Hash: Unknown (8)                          //表示证书使用的哈希算法的类型是未知的,由于标识码 "8" 未在常见的哈希算法标识码中出现过,因此说明这些算法不常见或不受支持。
                        Signature Hash Algorithm Signature: Unknown (11)                    //表示证书使用的签名算法的类型是未知的,由于标识码 "11" 未在常见的签名算法标识码中出现过,因此说明这些算法不常见或不受支持。
                    Signature Algorithm: rsa_pss_rsae_sha256 (0x0804)                       //表示该签名算法使用 RSA-PSS-RSAE 和 SHA-256 算法。RSA-PSS-RSAE 可以被用来进行数字签名的加密,而 RSA-PSS 则仅可用于数字签名,都是RSA的数字签名算法。SHA-256是哈希函数的一种,是从SHA-2系列算法中的一个。该算法的摘要长度为256位。
                        Signature Hash Algorithm Hash: Unknown (8)                          //表示证书使用的哈希算法的类型是未知的,由于标识码 "8" 未在常见的哈希算法标识码中出现过,因此说明这些算法不常见或不受支持。
                        Signature Hash Algorithm Signature: Unknown (4)                     //表示证书使用的签名算法的类型是未知的,由于标识码 "4" 未在常见的签名算法标识码中出现过,因此说明这些算法不常见或不受支持。
                    Signature Algorithm: rsa_pss_rsae_sha384 (0x0805)                       //表示该签名算法使用 RSA-PSS-RSAE 和 SHA-384 算法。RSA-PSS-RSAE被用来进行数字签名的加密。SHA-384是哈希函数的一种,是从SHA-2系列算法中的一个。该算法的摘要长度为384位。
                        Signature Hash Algorithm Hash: Unknown (8)                          //表示证书使用的哈希算法的类型是未知的,由于标识码 "8" 未在常见的哈希算法标识码中出现过,因此说明这些算法不常见或不受支持。
                        Signature Hash Algorithm Signature: Unknown (5)                     //表示证书使用的签名算法的类型是未知的,由于标识码 "5" 未在常见的签名算法标识码中出现过,因此说明这些算法不常见或不受支持。
                    Signature Algorithm: rsa_pss_rsae_sha512 (0x0806)                       //表示该签名算法使用 RSA-PSS-RSAE 和 SHA-512 算法。RSA-PSS-RSAE被用来进行数字签名的加密。SHA-512是哈希函数的一种,是从SHA-2系列算法中的一个。该算法的摘要长度为512位。
                        Signature Hash Algorithm Hash: Unknown (8)                          //表示证书使用的哈希算法的类型是未知的,由于标识码 "8" 未在常见的哈希算法标识码中出现过,因此说明这些算法不常见或不受支持。
                        Signature Hash Algorithm Signature: Unknown (6)                     //表示证书使用的签名算法的类型是未知的,由于标识码 "6" 未在常见的签名算法标识码中出现过,因此说明这些算法不常见或不受支持。
                    Signature Algorithm: rsa_pkcs1_sha256 (0x0401)                          //表示该签名算法使用 RSA-PKCS1 和 SHA-256 算法。RSA-PKCS1一种常见的数字签名标准,它采用的是 RSA 公钥密码系统,这种签名方式的安全性依赖于 RSA 的安全性。SHA-256是哈希函数的一种,用于生成数字指纹。当使用 "rsa_pkcs1_sha256" 进行数字签名时,先使用 SHA256 算法生成数据的散列值,再使用 RSA 算法进行签名。
                        Signature Hash Algorithm Hash: SHA256 (4)                           //表示证书使用的哈希算法的类型是 SHA-256。
                        Signature Hash Algorithm Signature: RSA (1)                         //表示证书使用的签名算法的类型是 RSA。
                    Signature Algorithm: rsa_pkcs1_sha384 (0x0501)                          //表示该签名算法使用 RSA-PKCS1 和 SHA-384 算法。RSA-PKCS1一种常见的数字签名标准,它采用的是 RSA 公钥密码系统,这种签名方式的安全性依赖于 RSA 的安全性。SHA-384是哈希函数的一种,用于生成数字指纹。当使用 "rsa_pkcs1_sha384" 进行数字签名时,先使用 SHA384 算法生成数据的散列值,再使用 RSA 算法进行签名。
                        Signature Hash Algorithm Hash: SHA384 (5)                           //表示证书使用的哈希算法的类型是 SHA-384。
                        Signature Hash Algorithm Signature: RSA (1)                         //表示证书使用的签名算法的类型是 RSA。
                    Signature Algorithm: rsa_pkcs1_sha512 (0x0601)                          //表示该签名算法使用 RSA-PKCS1 和 SHA-512 算法。RSA-PKCS1一种常见的数字签名标准,它采用的是 RSA 公钥密码系统,这种签名方式的安全性依赖于 RSA 的安全性。SHA-512是哈希函数的一种,用于生成数字指纹。当使用 "rsa_pkcs1_sha512" 进行数字签名时,先使用 SHA512 算法生成数据的散列值,再使用 RSA 算法进行签名。
                        Signature Hash Algorithm Hash: SHA512 (6)                           //表示证书使用的哈希算法的类型是 SHA-512。
                        Signature Hash Algorithm Signature: RSA (1)                         //表示证书使用的签名算法的类型是 RSA。
                    Signature Algorithm: SHA224 ECDSA (0x0303)                              //表示该签名算法是使用 SHA224 哈希算法和 ECDSA 算法生成的数字签名。
                        Signature Hash Algorithm Hash: SHA224 (3)                           //表示证书使用的哈希算法的类型是 SHA-224。
                        Signature Hash Algorithm Signature: ECDSA (3)                       //表示证书使用的签名算法的类型是 ECDSA。
                    Signature Algorithm: ecdsa_sha1 (0x0203)                                //表示该签名算法是使用 SHA1 哈希算法和 ECDSA 算法生成的数字签名。
                        Signature Hash Algorithm Hash: SHA1 (2)                             //表示使用SHA1算法生成消息摘要。
                        Signature Hash Algorithm Signature: ECDSA (3)                       //表示使用ECDSA算法对消息摘要进行签名。
                    Signature Algorithm: SHA224 RSA (0x0301)                                //表示该签名算法是使用 SHA224 哈希算法和 RSA 算法生成的数字签名。
                        Signature Hash Algorithm Hash: SHA224 (3)                           //表示使用SHA224算法生成消息摘要。
                        Signature Hash Algorithm Signature: RSA (1)                         //表示使用RSA算法对消息摘要进行签名。
                    Signature Algorithm: rsa_pkcs1_sha1 (0x0201)                            //表示该签名算法使用 RSA-PKCS1 和 SHA-1 算法。RSA-PKCS1一种常见的数字签名标准,它采用的是 RSA 公钥密码系统,这种签名方式的安全性依赖于 RSA 的安全性。SHA-1是哈希函数的一种,用于生成数字指纹。当使用 "rsa_pkcs1_sha1" 进行数字签名时,先使用 SHA1 算法生成数据的散列值,再使用 RSA 算法进行签名。
                        Signature Hash Algorithm Hash: SHA1 (2)                             //表示证书使用的哈希算法的类型是 SHA-1。 SHA-1 哈希算法的安全性已经受到质疑
                        Signature Hash Algorithm Signature: RSA (1)                         //表示证书使用的签名算法的类型是 RSA。
                    Signature Algorithm: SHA224 DSA (0x0302)                                //表示该签名算法是使用 SHA224 哈希算法和 DSA 算法生成的数字签名。
                        Signature Hash Algorithm Hash: SHA224 (3)                           //表示使用SHA224算法生成消息摘要。
                        Signature Hash Algorithm Signature: DSA (2)                         //表示使用DSA算法对消息摘要进行签名。
                    Signature Algorithm: SHA1 DSA (0x0202)                                  //表示该签名算法是使用 SHA1 哈希算法和 DSA 算法生成的数字签名。
                        Signature Hash Algorithm Hash: SHA1 (2)                             //表示使用SHA1算法生成消息摘要。
                        Signature Hash Algorithm Signature: DSA (2)                         //表示使用DSA算法对消息摘要进行签名。
                    Signature Algorithm: SHA256 DSA (0x0402)                                //表示该签名算法是使用 SHA256 哈希算法和 DSA 算法生成的数字签名。
                        Signature Hash Algorithm Hash: SHA256 (4)                           //表示使用SHA256算法生成消息摘要。
                        Signature Hash Algorithm Signature: DSA (2)                         //表示使用DSA算法对消息摘要进行签名。
                    Signature Algorithm: SHA384 DSA (0x0502)                                //表示该签名算法是使用 SHA384 哈希算法和 DSA 算法生成的数字签名。
                        Signature Hash Algorithm Hash: SHA384 (5)                           //表示使用SHA384算法生成消息摘要。
                        Signature Hash Algorithm Signature: DSA (2)                         //表示使用DSA算法对消息摘要进行签名。
                    Signature Algorithm: SHA512 DSA (0x0602)                                //表示该签名算法是使用 SHA512 哈希算法和 DSA 算法生成的数字签名。
                        Signature Hash Algorithm Hash: SHA512 (6)                           //表示使用SHA512算法生成消息摘要。
                        Signature Hash Algorithm Signature: DSA (2)                         //表示使用DSA算法对消息摘要进行签名。
            Extension: supported_versions (len=9)                                           //表示支持的 TLS 版本。
                Type: supported_versions (43)                                               //表示扩展类型为 supported_versions。
                Length: 9                                                                   //表示扩展数据的长度为 9 字节。
                Supported Versions length: 8                                                //表示支持的协议版本长度为 8 字节。
                Supported Version: TLS 1.3 (0x0304)                                         //表示支持的 TLS 版本为 1.3。
                Supported Version: TLS 1.2 (0x0303)                                         //表示支持的 TLS 版本为 1.2。
                Supported Version: TLS 1.1 (0x0302)                                         //表示支持的 TLS 版本为 1.1。
                Supported Version: TLS 1.0 (0x0301)                                         //表示支持的 TLS 版本为 1.0。
            Extension: psk_key_exchange_modes (len=2)                                       //表示它是一个预共享密钥（PSK）密钥交换模式扩展。
                Type: psk_key_exchange_modes (45)                                           //值为45,表示扩展类型为 psk_key_exchange_modes。
                Length: 2                                                                   //表示扩展数据的长度为 2 字节。
                PSK Key Exchange Modes Length: 1                                            //表示PSK密钥交换模式的数量为1
                PSK Key Exchange Mode: PSK with (EC)DHE key establishment (psk_dhe_ke) (1)  //表示该密钥交换模式是使用预共享密钥与（椭圆曲线）密钥交换的完整性保护（DHE）的密钥建立进行的。该模式的编号为1。
            Extension: key_share (len=38)                                                   //表示客户端公开了一个密钥来完成密钥交换。
                Type: key_share (51)                                                        //值为51,表示扩展类型为 key_share。
                Length: 38                                                                  //表示扩展数据的长度为 38 字节。
                Key Share extension                                                         //表示密钥交换扩展。
                    Client Key Share Length: 36                                             //表示客户端密钥交换的长度为 36 字节。
                    Key Share Entry: Group: x25519, Key Exchange length: 32                 //表示客户端使用 x25519 椭圆曲线密钥交换算法生成的密钥交换数据的长度为 32 字节。
                        Group: x25519 (29)                                                  
                        Key Exchange Length: 32                                     
                        Key Exchange: f0db3e30332b0b7e4163b51d29e86cbf74ddd767028d7db4…     //表示客户端的密钥交换内容,一串16进制的数字。

```
## Server Hello
``` data

Transport Layer Security                                                                    //TLS是在传输层上为应用程序提供安全性的协议。
    TLSv1.2 Record Layer: Handshake Protocol: Server Hello                                  //表示TLS协议版本为1.2,记录层为握手协议,握手类型为Server Hello。
        Content Type: Handshake (22)                                                        //表示内容类型为握手协议。
        Version: TLS 1.2 (0x0303)                                                           //表示TLS协议版本为1.2。
        Length: 57                                                                          //表示记录层的长度为57字节。
        Handshake Protocol: Server Hello                                                    //表示握手类型为Server Hello。
            Handshake Type: Server Hello (2)                                                //值为2,表示握手类型为Server Hello。
            Length: 53                                                                      //表示握手数据的长度为53字节。
            Version: TLS 1.2 (0x0303)                                                       //表示TLS协议版本为1.2。
            Random: c73f911c4df0de331232b0c95a90c3753f89386d502c825e…                       //表示服务器生成的随机数,长度为32字节,包含下面2个字段。
                GMT Unix Time: Dec  6, 2075 12:25:00.000000000 中国标准时间                   //表示服务器生成随机数的时间,长度为4字节。
                Random Bytes: 4df0de331232b0c95a90c3753f89386d502c825ea9d7d303…              //表示服务器生成随机数的内容,长度为28字节。
            Session ID Length: 0                                                              //表示会话ID的长度为0字节。
            Cipher Suite: TLS_RSA_WITH_AES_256_CBC_SHA (0x0035)                               //表示服务器选择的加密套件为TLS_RSA_WITH_AES_256_CBC_SHA。
            Compression Method: null (0)                                                      //表示服务器选择的压缩方法为null。
            Extensions Length: 13                                                               //表示扩展数据的长度为13字节。
            Extension: renegotiation_info (len=1)                                               //扩展是一种安全功能,用于禁止不安全的重新协商,它向对等方发送了没有重新协商数据的信息。服务端发送 Renegotiation Info 扩展是为了证明它在使用 TLS 重新协商的情况下的身份。该扩展提供了服务器的信息,证明它与先前的安全会话是相关的,并且已经建立了一个安全的通道。这对于提高安全性很重要,因为它防止了重新协商的攻击,例如中间人攻击。 
                Type: renegotiation_info (65281)                                                //表示扩展类型为65281,表示这是一个重新协商信息扩展。
                Length: 1                                                                       //表示扩展数据的长度为1字节。
                Renegotiation Info extension                                                    //表示这是一个重新协商信息扩展
                    Renegotiation info extension length: 0                                      //表示重新协商信息扩展的长度为0字节。
            Extension: encrypt_then_mac (len=0)                                                 //扩展用于强制 TLS 握手协议中的数据加密和数据认证的顺序,使得在加密数据之后再对数据进行认证。这样可以防止攻击者通过篡改数据来破坏安全性。
                Type: encrypt_then_mac (22)                                                     //表示扩展类型为22,表示这是一个加密后的MAC扩展。
                Length: 0                                                                       //表示扩展数据的长度为0字节。
            Extension: extended_master_secret (len=0)                                           //扩展则是用于保护会话密钥的安全性,防止攻击者通过利用握手协议的中间状态破坏会话密钥的安全性。该扩展能够通过扩展整个 TLS 会话的安全性来保护会话密钥。
                Type: extended_master_secret (23)                                               //表示扩展类型为23,表示这是一个扩展的主密钥扩展。
                Length: 0                                                                       //表示扩展数据的长度为0字节。这说明这个扩展没有携带任何附加数据,因此服务端发送这个扩展仅是为了告诉客户端支持这种安全机制。

```

## Certificate
``` data
//服务端发送Certificate 消息用于提供服务器的证书,以证明其身份。在客户端收到Certificate消息后,可以对服务器证书进行验证,以确保与安全的服务器建立连接。
Transport Layer Security
    TLSv1.2 Record Layer: Handshake Protocol: Certificate                                                               //表示这是一个握手协议,类型为Certificate。
        Content Type: Handshake (22)                                                                                    //表示这是一个握手协议。
        Version: TLS 1.2 (0x0303)                                                                                       //表示这是一个TLSv1.2协议。
        Length: 1443                                                                                                    //表示这个消息的长度为1443字节。
        Handshake Protocol: Certificate                                                                                 //表示这是一个握手协议,类型为Certificate。
            Handshake Type: Certificate (11)                                                                            //值为11,表示这是一个Certificate消息。
            Length: 1439                                                                                                //表示这个消息的长度为1439字节。
            Certificates Length: 1436                                                                                   //表示证书链的长度为1436字节。
            Certificates (1436 bytes)                                                                                   //表示证书链的长度为1436字节。
                Certificate Length: 764                                                                                 //表示证书的长度为764字节。
                Certificate: 308202f830820261a003020102020109300d06092a864886… (id-at-commonName=10.19.12.123)          //表示证书的内容。
                    signedCertificate                                                                                   //是一个字段,其中包含CA颁发给服务器的数字证书。该证书用作服务器身份的证明,并为客户端提供了一种验证服务器真实性的方法。证书包括服务器域名、公钥和 CA 的数字签名等信息,客户端使用这些信息来验证服务器的身份。
                        version: v3 (2)                                                                                 //表示证书的版本号为3。
                        serialNumber: 9                                                                                 //表示证书的序列号为9。
                        signature (sha1WithRSAEncryption)                                                               //是指数字证书签名使用的算法是用SHA-1哈希算法结合RSA算法进行签名。签名算法的作用是对报文的内容进行数字签名,以证明报文的完整性和真实性。
                            Algorithm Id: 1.2.840.113549.1.1.5 (sha1WithRSAEncryption)                                  //该签名算法的算法标识为 "1.2.840.113549.1.1.5",也就是SHA-1 with RSA算法的算法标识。           
                        issuer: rdnSequence (0)                                                                         //表示证书的颁发者。rdnSequence一种X.509证书格式。
                            rdnSequence: 6 items (id-at-commonName=XCSCA,id-at-organizationalUnitName=XCSCA,id-at-organizationName=XCSCA,id-at-localityName=cs,id-at-stateOrProvinceName=hn,id-at-countryName=zg)   //该字段包含了6项信息,分别是：通用名称、组织单位名称、组织名称、地区名称、省份名称、国家名称。
                                RDNSequence item: 1 item (id-at-countryName=zg)                                         //表示证书的颁发者的国家名称为zg。
                                    RelativeDistinguishedName item (id-at-countryName=zg)                               //表示一项相对可识别名称的序列,包含一个项(该项目的属性为 "countryName",值为 "zg")
                                        Id: 2.5.4.6 (id-at-countryName)                                                 //表示该属性的OID为2.5.4.6,它是 "countryName"的 OID。
                                        CountryName: zg                                                                 //表示该属性的值为 zg。
                                RDNSequence item: 1 item (id-at-stateOrProvinceName=hn)                                 //表示证书的颁发者的省份名称为hn。
                                    RelativeDistinguishedName item (id-at-stateOrProvinceName=hn)                       //该项目的属性为 "stateOrProvinceName",值为 "hn"
                                        Id: 2.5.4.8 (id-at-stateOrProvinceName)                                         //表示该属性的OID为2.5.4.8,它是 "stateOrProvinceName"的 OID。
                                        DirectoryString: printableString (1)                                            //表示值的字符串类型为"printableString",表示一个可打印字符串。
                                            printableString: hn                                                         //表示该属性的值为 hn。
                                RDNSequence item: 1 item (id-at-localityName=cs)                                        //表示证书的颁发者的地区名称为cs。
                                    RelativeDistinguishedName item (id-at-localityName=cs)                              //该项目的属性为 "localityName",值为 "cs"
                                        Id: 2.5.4.7 (id-at-localityName)                                                //表示该属性的OID为2.5.4.7,它是 "localityName"的 OID。
                                        DirectoryString: printableString (1)                                            //表示值的字符串类型为"printableString",表示一个可打印字符串。
                                            printableString: cs                                                         //表示该属性的值为 cs。
                                RDNSequence item: 1 item (id-at-organizationName=XCSCA)                                 //表示证书的颁发者的组织名称为XCSCA。
                                    RelativeDistinguishedName item (id-at-organizationName=XCSCA)                       //该项目的属性为 "organizationName",值为 "XCSCA"
                                        Id: 2.5.4.10 (id-at-organizationName)                                           //表示该属性的OID为2.5.4.10,它是 "organizationName"的 OID。
                                        DirectoryString: printableString (1)                                            //表示值的字符串类型为"printableString",表示一个可打印字符串。
                                            printableString: XCSCA                                                      //表示该属性的值为 XCSCA。
                                RDNSequence item: 1 item (id-at-organizationalUnitName=XCSCA)                           //表示证书的颁发者的组织单位名称为XCSCA。
                                    RelativeDistinguishedName item (id-at-organizationalUnitName=XCSCA)                 //该项目的属性为 "organizationalUnitName",值为 "XCSCA"
                                        Id: 2.5.4.11 (id-at-organizationalUnitName)                                     //表示该属性的OID为2.5.4.11,它是 "organizationalUnitName"的 OID。
                                        DirectoryString: printableString (1)                                            //表示值的字符串类型为"printableString",表示一个可打印字符串。
                                            printableString: XCSCA                                                      //表示该属性的值为 XCSCA。
                                RDNSequence item: 1 item (id-at-commonName=XCSCA)                                       //表示证书的颁发者的通用名称为XCSCA。
                                    RelativeDistinguishedName item (id-at-commonName=XCSCA)                             //该项目的属性为 "commonName",值为 "XCSCA"
                                        Id: 2.5.4.3 (id-at-commonName)                                                  //表示该属性的OID为2.5.4.3,它是 "commonName"的 OID。
                                        DirectoryString: printableString (1)                                            //表示值的字符串类型为"printableString",表示一个可打印字符串。
                                            printableString: XCSCA                                                      //表示该属性的值为 XCSCA。
                        validity                                                                                        //表示证书的有效期。
                            notBefore: utcTime (0)                                                                      //表示证书的有效期的开始时间。
                                utcTime: 22-08-23 05:40:00 (UTC)                                                        //表示证书的有效期的开始时间为 2022-08-23 05:40:00。
                            notAfter: utcTime (0)                                                                       //表示证书的有效期的结束时间。
                                utcTime: 23-08-23 05:40:00 (UTC)                                                        //表示证书的有效期的结束时间为 2023-08-23 05:40:00。
                        subject: rdnSequence (0)                                                                        //表示证书的主题。
                            rdnSequence: 1 item (id-at-commonName=10.19.12.123)                                         
                                RDNSequence item: 1 item (id-at-commonName=10.19.12.123)                                //表示证书的主题的通用名称为10.19.12.123。
                                    RelativeDistinguishedName item (id-at-commonName=10.19.12.123)                      //该项目的属性为 "commonName",值为 "10.19.12.123"
                                        Id: 2.5.4.3 (id-at-commonName)                                                  //表示该属性的OID为2.5.4.3,它是 "commonName"的 OID。
                                        DirectoryString: printableString (1)                                            //表示值的字符串类型为"printableString",表示一个可打印字符串。
                                            printableString: 10.19.12.123                                               //表示该属性的值为10.19.12.123
                        subjectPublicKeyInfo                                                                            //表示证书的公钥信息。
                            algorithm (rsaEncryption)                                                                   //表示证书中使用的公钥算法是 RSA 加密。
                                Algorithm Id: 1.2.840.113549.1.1.1 (rsaEncryption)                                      //该签名算法的算法标识为 "1.2.840.113549.1.1.1",也就是RSA算法的算法标识。
                            subjectPublicKey: 3082010a0282010100d8ddee661c6c56bef609cb4bc8a86d…                         //是对公钥进行了 ASN.1 编码,其中包含了证书中的模数 modulus 和公钥指数 publicExponent。在使用RSA加密时,需要同时使用Modulus和PublicExponent。通过使用这两个值,可以对任意明文数据进行加密,以获得密文,然后可以使用密钥解密密文,以恢复原始明文。
                                modulus: 0x00d8ddee661c6c56bef609cb4bc8a86df098ddf927d49052…                            //表示证书的公钥的模数。是一个大质数,用于加密和解密数据。
                                publicExponent: 65537                                                                   //表示证书的公钥的指数。是一个质数,用于对加密数据进行指数运算,以加密数据。
                        extensions: 6 items                                                                             //表示证书的扩展信息。包含了6个扩展信息。
                            Extension (id-ce-basicConstraints)                                                          //是一种扩展,其中包含对证书颁发者的限制信息。
                                Extension Id: 2.5.29.19 (id-ce-basicConstraints)                                        //这种扩展的标识符为2.5.29.19。
                                critical: True                                                                          //该扩展为“关键”,意味着如果该扩展不能被识别,那么整个证书就不能被认可。
                                BasicConstraintsSyntax [0 length]                                                       //字段是一个空数组,即 [0 length],意味着该证书不是一个证书颁发机构的证书。
                            Extension (id-ce-subjectKeyIdentifier)                                                      //该扩展包含了证书主题公钥的唯一标识符,该标识符通常是一个摘要值,用于识别该证书所代表的公钥。
                                Extension Id: 2.5.29.14 (id-ce-subjectKeyIdentifier)                                    //该扩展的标识符为2.5.29.14
                                SubjectKeyIdentifier: 3feae3ba0ea38a896a736c880730a5cc7850d05a                          //表示该证书的主题公钥的唯一标识符。
                            Extension (id-ce-keyUsage)                                                                  //这是一个X.509证书的扩展字段,用于描述证书关联的公钥的用途。
                                Extension Id: 2.5.29.15 (id-ce-keyUsage)                                                //扩展的标识符是2.5.29.15
                                Padding: 5                                                                              //表示填充5个字节。用于对齐数据,以确保数据的格式正确。
                                KeyUsage: e0                                                                            //表示该扩展的值为0xe0,即二进制的11100000。
                                    1... .... = digitalSignature: True                                                  //表示该证书的公钥可以用于数字签名。
                                    .1.. .... = contentCommitment: True                                                 //表示允许使用该公钥进行内容承诺。
                                    ..1. .... = keyEncipherment: True                                                   //表示允许使用该公钥进行密钥加密。
                                    ...0 .... = dataEncipherment: False                                                 //表示不允许使用该公钥进行数据加密。
                                    .... 0... = keyAgreement: False                                                     //表示不允许使用该公钥进行密钥协商。
                                    .... .0.. = keyCertSign: False                                                      //表示不允许使用该公钥签名其他证书 (证书签名)。
                                    .... ..0. = cRLSign: False                                                          //表示不允许使用该公钥签名证书吊销列表 (CRL)。
                                    .... ...0 = encipherOnly: False                                                     //表示不允许使用该公钥进行加密。
                                    0... .... = decipherOnly: False                                                     //表示不允许使用该公钥进行解密。
                            Extension (id-ce-subjectAltName)                                                            //是一种X.509证书扩展,用于为证书持有者提供其他名称,与证书主题名称不同。它可以包含域名,IP地址,电子邮件地址等。这种扩展有助于保护免受证书造假攻击,并帮助提高可用性,因为它可以允许一个证书与多个域名关联。
                                Extension Id: 2.5.29.17 (id-ce-subjectAltName)                                          //表示该扩展字段的 OID 为 2.5.29.17
                                GeneralNames: 2 items                                                                   //表示该扩展字段的值为一个数组,数组中有两个元素。
                                    GeneralName: iPAddress (7)                                                          //表示该数组中的第一个元素是一个 IP 地址。
                                        iPAddress: 10.19.12.123                                                         //表示该 IP 地址为10.19.12.123
                                    GeneralName: dNSName (2)                                                            //表示该数组中的第二个元素是一个域名。
                                        dNSName: 10.19.12.123                                                           //表示该域名为10.19.12.123
                            Extension (ns_cert_exts.cert_type)                                                          //这是一个 Netscape 扩展字段,用于描述证书关联的公钥的用途。
                                Extension Id: 2.16.840.1.113730.1.1 (ns_cert_exts.cert_type)                            //表示该扩展字段的 OID 为 2.16.840.1.113730.1.1
                                Padding: 6                                                                              //表示填充6个字节。用于对齐数据,以确保数据的格式正确。
                                CertType: 40                                                                            //表示该扩展字段的值为0x40,即二进制的01000000。
                                    0... .... = ssl-client: False                                                       //表示该证书的公钥不可以用于 SSL 客户端认证。
                                    .1.. .... = ssl-server: True                                                        //表示该证书的公钥可以用于 SSL 服务器认证。
                                    ..0. .... = smime: False                                                            //表示该证书的公钥不可以用于 S/MIME。
                                    ...0 .... = object-signing: False                                                   //表示该证书的公钥不可以用于对象签名。
                                    .... 0... = reserved-for-future-use: False                                          //表示该证书的公钥不可以用于未来的用途。
                                    .... .0.. = ssl-ca: False                                                           //表示该证书的公钥不可以用于 SSL 证书认证。
                                    .... ..0. = smime-ca: False                                                         //表示该证书的公钥不可以用于 S/MIME 证书认证。
                                    .... ...0 = object-signing-ca: False                                                //表示该证书的公钥不可以用于对象签名证书认证。
                            Extension (ns_cert_exts.comment)                                                            //主要是提供证书注释信息,用于证书管理。
                                Extension Id: 2.16.840.1.113730.1.13 (ns_cert_exts.comment)                             //表示该扩展字段的 OID 为 2.16.840.1.113730.1.13
                                Comment: xca certificate                                                                //表示该扩展字段的值为 xca certificate。        
                    algorithmIdentifier (sha1WithRSAEncryption)                                                         //表示该证书的签名算法为 sha1WithRSAEncryption。
                        Algorithm Id: 1.2.840.113549.1.1.5 (sha1WithRSAEncryption)                                      //表示该签名算法的 OID 为 1.2.840.113549.1.1.5。
                    Padding: 0                                                                                          //该数据没有使用任何填充。
                    encrypted: cb1dacda38ef7a2e6b45a0601112d6fefd1dafc43c9bce4d…                                        //指签名数据加密后得到的密文。
                Certificate Length: 666                                                                                 //表示该证书的长度为666个字节。
                Certificate: 30820296308201ffa003020102020101300d06092a864886… (id-at-commonName=XCSCA,id-at-organizationalUnitName=XCSCA,id-at-organizationName=XCSCA,id-at-localityName=cs,id-at-stateOrProvinceName=hn,id-at-countryName=zg)     //表示该证书的内容。
                    signedCertificate                                                                                   //表示该证书的签名数据。
                        version: v3 (2)                                                                                 //表示该证书的版本为 v3。
                        serialNumber: 1                                                                                 //表示该证书的序列号为 1。
                        signature (sha1WithRSAEncryption)                                                               //表示该证书的签名算法为 sha1WithRSAEncryption。
                            Algorithm Id: 1.2.840.113549.1.1.5 (sha1WithRSAEncryption)                                  //表示该签名算法的 OID 为 1.2.840.113549.1.1.5。
                        issuer: rdnSequence (0)                                                                         //表示证书的颁发者。rdnSequence一种X.509证书格式。
                            rdnSequence: 6 items (id-at-commonName=XCSCA,id-at-organizationalUnitName=XCSCA,id-at-organizationName=XCSCA,id-at-localityName=cs,id-at-stateOrProvinceName=hn,id-at-countryName=zg)       //该字段包含了6项信息,分别是：通用名称、组织单位名称、组织名称、地区名称、省份名称、国家名称。
                                RDNSequence item: 1 item (id-at-countryName=zg)                                         //表示该证书的颁发者的国家名称为 zg。
                                    RelativeDistinguishedName item (id-at-countryName=zg)                               //表示一项相对可识别名称的序列,包含一个项(该项目的属性为 "countryName",值为 "zg")
                                        Id: 2.5.4.6 (id-at-countryName)                                                 //表示该属性的OID为2.5.4.6,它是 "countryName"的 OID。
                                        CountryName: zg                                                                 //表示该属性的值为 zg。
                                RDNSequence item: 1 item (id-at-stateOrProvinceName=hn)                                 //表示该证书的颁发者的省份名称为 hn。
                                    RelativeDistinguishedName item (id-at-stateOrProvinceName=hn)                       //表示一项相对可识别名称的序列,包含一个项(该项目的属性为 "stateOrProvinceName",值为 "hn")
                                        Id: 2.5.4.8 (id-at-stateOrProvinceName)                                         //表示该属性的OID为2.5.4.8,它是 "stateOrProvinceName"的 OID。
                                        DirectoryString: printableString (1)                                            //表示值的字符串类型为"printableString",表示一个可打印字符串。
                                            printableString: hn                                                         //表示该属性的值为 hn。
                                RDNSequence item: 1 item (id-at-localityName=cs)                                        //表示该证书的颁发者的地区名称为 cs。
                                    RelativeDistinguishedName item (id-at-localityName=cs)                              //表示一项相对可识别名称的序列,包含一个项(该项目的属性为 "localityName",值为 "cs")
                                        Id: 2.5.4.7 (id-at-localityName)                                                //表示该属性的OID为2.5.4.7,它是 "localityName"的 OID。
                                        DirectoryString: printableString (1)                                            //表示值的字符串类型为"printableString",表示一个可打印字符串。
                                            printableString: cs                                                         //表示该属性的值为 cs。
                                RDNSequence item: 1 item (id-at-organizationName=XCSCA)                                 //表示该证书的颁发者的组织名称为 XCSCA。
                                    RelativeDistinguishedName item (id-at-organizationName=XCSCA)                       //表示一项相对可识别名称的序列,包含一个项(该项目的属性为 "organizationName",值为 "XCSCA")
                                        Id: 2.5.4.10 (id-at-organizationName)                                           //表示该属性的OID为2.5.4.10,它是 "organizationName"的 OID。
                                        DirectoryString: printableString (1)                                            //表示值的字符串类型为"printableString",表示一个可打印字符串。
                                            printableString: XCSCA                                                      //表示该属性的值为 XCSCA。
                                RDNSequence item: 1 item (id-at-organizationalUnitName=XCSCA)                           //表示该证书的颁发者的组织单元名称为 XCSCA。
                                    RelativeDistinguishedName item (id-at-organizationalUnitName=XCSCA)                 //表示一项相对可识别名称的序列,包含一个项(该项目的属性为 "organizationalUnitName",值为 "XCSCA")
                                        Id: 2.5.4.11 (id-at-organizationalUnitName)                                     //表示该属性的OID为2.5.4.11,它是 "organizationalUnitName"的 OID。
                                        DirectoryString: printableString (1)                                            //表示值的字符串类型为"printableString",表示一个可打印字符串。
                                            printableString: XCSCA                                                      //表示该属性的值为 XCSCA。
                                RDNSequence item: 1 item (id-at-commonName=XCSCA)                                       //表示该证书的颁发者的通用名称为 XCSCA。
                                    RelativeDistinguishedName item (id-at-commonName=XCSCA)                             //表示一项相对可识别名称的序列,包含一个项(该项目的属性为 "commonName",值为 "XCSCA")
                                        Id: 2.5.4.3 (id-at-commonName)                                                  //表示该属性的OID为2.5.4.3,它是 "commonName"的 OID。
                                        DirectoryString: printableString (1)                                            //表示值的字符串类型为"printableString",表示一个可打印字符串。
                                            printableString: XCSCA                                                      //表示该属性的值为 XCSCA。
                        validity                                                                                        //表示证书的有效期。
                            notBefore: utcTime (0)                                                                      //表示证书的有效期的开始时间。
                                utcTime: 21-11-02 09:22:00 (UTC)                                                        //表示证书的有效期的开始时间为 2002-11-21 09:22:00 (UTC)。
                            notAfter: utcTime (0)                                                                       //表示证书的有效期的结束时间。
                                utcTime: 31-11-02 09:22:00 (UTC)                                                        //表示证书的有效期的结束时间为 2002-11-31 09:22:00 (UTC)。
                        subject: rdnSequence (0)                                                                        //表示证书的主体。
                            rdnSequence: 6 items (id-at-commonName=XCSCA,id-at-organizationalUnitName=XCSCA,id-at-organizationName=XCSCA,id-at-localityName=cs,id-at-stateOrProvinceName=hn,id-at-countryName=zg)       //该字段包含了6项信息,分别是：通用名称、组织单位名称、组织名称、地区名称、省份名称、国家名称。
                                RDNSequence item: 1 item (id-at-countryName=zg)                                         //表示该证书的主体的国家名称为 zg。
                                    RelativeDistinguishedName item (id-at-countryName=zg)                               //表示一项相对可识别名称的序列,包含一个项(该项目的属性为 "countryName",值为 "zg")
                                        Id: 2.5.4.6 (id-at-countryName)                                                 //表示该属性的OID为
                                        CountryName: zg                                                                 //表示该属性的值为 zg。
                                RDNSequence item: 1 item (id-at-stateOrProvinceName=hn)                                 //表示该证书的主体的省份名称为 hn。
                                    RelativeDistinguishedName item (id-at-stateOrProvinceName=hn)                       //表示一项相对可识别名称的序列,包含一个项(该项目的属性为 "stateOrProvinceName",值为 "hn")
                                        Id: 2.5.4.8 (id-at-stateOrProvinceName)                                         //表示该属性的OID为
                                        DirectoryString: printableString (1)                                            //表示值的字符串类型为"printableString",表示一个可打印字符串。
                                            printableString: hn                                                         //表示该属性的值为 hn。
                                RDNSequence item: 1 item (id-at-localityName=cs)                                        //表示该证书的主体的地区名称为 cs。
                                    RelativeDistinguishedName item (id-at-localityName=cs)                              //表示一项相对可识别名称的序列,包含一个项(该项目的属性为 "localityName",值为 "cs")
                                        Id: 2.5.4.7 (id-at-localityName)                                                //表示该属性的OID为
                                        DirectoryString: printableString (1)                                            //表示值的字符串类型为"printableString",表示一个可打印字符串。
                                            printableString: cs                                                         //表示该属性的值为 cs。
                                RDNSequence item: 1 item (id-at-organizationName=XCSCA)                                 //表示该证书的主体的组织名称为 XCSCA。
                                    RelativeDistinguishedName item (id-at-organizationName=XCSCA)                       //表示一项相对可识别名称的序列,包含一个项(该项目的属性为 "organizationName",值为 "XCSCA")
                                        Id: 2.5.4.10 (id-at-organizationName)                                           //表示该属性的OID为
                                        DirectoryString: printableString (1)                                            //表示值的字符串类型为"printableString",表示一个可打印字符串。
                                            printableString: XCSCA                                                      //表示该属性的值为 XCSCA。
                                RDNSequence item: 1 item (id-at-organizationalUnitName=XCSCA)                           //表示该证书的主体的组织单位名称为 XCSCA。
                                    RelativeDistinguishedName item (id-at-organizationalUnitName=XCSCA)                 //表示一项相对可识别名称的序列,包含一个项(该项目的属性为 "organizationalUnitName",值为 "XCSCA")
                                        Id: 2.5.4.11 (id-at-organizationalUnitName)                                     //表示该属性的OID为
                                        DirectoryString: printableString (1)                                            //表示值的字符串类型为"printableString",表示一个可打印字符串。
                                            printableString: XCSCA                                                      //表示该属性的值为 XCSCA。
                                RDNSequence item: 1 item (id-at-commonName=XCSCA)                                       //表示该证书的主体的通用名称为 XCSCA。
                                    RelativeDistinguishedName item (id-at-commonName=XCSCA)                             //表示一项相对可识别名称的序列,包含一个项(该项目的属性为 "commonName",值为 "XCSCA")
                                        Id: 2.5.4.3 (id-at-commonName)                                                  //表示该属性的OID为
                                        DirectoryString: printableString (1)                                            //表示值的字符串类型为"printableString",表示一个可打印字符串。
                                            printableString: XCSCA                                                      //表示该属性的值为 XCSCA。
                        subjectPublicKeyInfo                                                                            //表示该证书的主体公钥信息。
                            algorithm (rsaEncryption)                                                                   //表示证书中使用的公钥算法是 RSA 加密。
                                Algorithm Id: 1.2.840.113549.1.1.1 (rsaEncryption)                                      //表示该算法的OID为 rsaEncryption。
                            subjectPublicKey: 30818902818100f4f9ad3d92320aff0f6a23d3db07cd2159…                         //表示该证书的主体公钥的值。是对公钥进行了 ASN.1 编码,其中包含了证书中的模数 modulus 和公钥指数 publicExponent。在使用RSA加密时,需要同时使用Modulus和PublicExponent。通过使用这两个值,可以对任意明文数据进行加密,以获得密文,然后可以使用密钥解密密文,以恢复原始明文。
                                modulus: 0x00f4f9ad3d92320aff0f6a23d3db07cd215994862abf712b…                            //表示该证书的主体公钥的模数。是一个大质数,用于加密和解密数据。
                                publicExponent: 65537                                                                   //表示该证书的主体公钥的公钥指数。是一个质数,用于对加密数据进行指数运算,以加密数据。
                        extensions: 5 items                                                                             //表示该证书的扩展信息。包含5项扩展信息。
                            Extension (id-ce-basicConstraints)                                                          //是一种扩展,其中包含对证书颁发者的限制信息。
                                Extension Id: 2.5.29.19 (id-ce-basicConstraints)                                        //表示该扩展的OID为2.5.29.19 。
                                critical: True                                                                          //该扩展为“关键”,意味着如果该扩展不能被识别,那么整个证书就不能被认可。
                                BasicConstraintsSyntax                                                                  //表示该扩展的值的语法。
                                    cA: True                                                                            //表示该证书的颁发者是一个证书颁发机构。
                            Extension (id-ce-subjectKeyIdentifier)                                                      //该扩展包含了证书主题公钥的唯一标识符,该标识符通常是一个摘要值,用于识别该证书所代表的公钥。
                                Extension Id: 2.5.29.14 (id-ce-subjectKeyIdentifier)                                    //表示该扩展的OID为
                                SubjectKeyIdentifier: 7d0fb4e7cf9db640ff45f00d5359417720d27fd9                          //表示该证书的主题公钥的唯一标识符。
                            Extension (id-ce-keyUsage)                                                                  //该扩展包含了证书主体公钥的用途。
                                Extension Id: 2.5.29.15 (id-ce-keyUsage)                                                //表示该扩展的OID为
                                Padding: 1                                                                              //表示填充1个字节。 这是用于对齐数据的,以确保数据的格式正确。
                                KeyUsage: 06                                                                            //表示该扩展的值为0x06,即00000110。
                                    0... .... = digitalSignature: False                                                 //表示该证书的主体公钥不用于数字签名。
                                    .0.. .... = contentCommitment: False                                                //表示该证书的主体公钥不用于内容提交。
                                    ..0. .... = keyEncipherment: False                                                  //表示该证书的主体公钥不用于密钥加密。
                                    ...0 .... = dataEncipherment: False                                                 //表示该证书的主体公钥不用于数据加密。
                                    .... 0... = keyAgreement: False                                                     //表示该证书的主体公钥不用于密钥协商。
                                    .... .1.. = keyCertSign: True                                                       //表示该证书的主体公钥用于证书签名。
                                    .... ..1. = cRLSign: True                                                           //表示该证书的主体公钥用于证书吊销列表签名。
                                    .... ...0 = encipherOnly: False                                                     //表示该证书的主体公钥不用于加密。
                                    0... .... = decipherOnly: False                                                     //表示该证书的主体公钥不用于解密。
                            Extension (ns_cert_exts.cert_type)                                                          //该扩展包含了证书的类型。
                                Extension Id: 2.16.840.1.113730.1.1 (ns_cert_exts.cert_type)                            //表示该扩展的OID为
                                Padding: 0                                                                              //没有任何填充数据
                                CertType: 07                                                                            //表示该扩展的值为0x07,即00000111。
                                    0... .... = ssl-client: False                                                       //表示该证书的主体公钥不用于SSL客户端。
                                    .0.. .... = ssl-server: False                                                       //表示该证书的主体公钥不用于SSL服务端。
                                    ..0. .... = smime: False                                                            //表示该证书的主体公钥不用于S/MIME。
                                    ...0 .... = object-signing: False                                                   //表示该证书的主体公钥不用于对象签名。
                                    .... 0... = reserved-for-future-use: False                                          //表示该证书的主体公钥不用于未来的用途。
                                    .... .1.. = ssl-ca: True                                                            //表示该证书的主体公钥用于SSL证书认证。
                                    .... ..1. = smime-ca: True                                                          //表示该证书的主体公钥用于S/MIME证书认证。
                                    .... ...1 = object-signing-ca: True                                                 //表示该证书的主体公钥用于对象签名证书认证。
                            Extension (ns_cert_exts.comment)                                                            //该扩展包含了证书的注释。
                                Extension Id: 2.16.840.1.113730.1.13 (ns_cert_exts.comment)                             //表示该扩展的OID为
                                Comment: xca certificate                                                                //表示该证书的注释为xca certificate。
                    algorithmIdentifier (sha1WithRSAEncryption)                                                         //该算法标识符表示了证书的签名算法。
                        Algorithm Id: 1.2.840.113549.1.1.5 (sha1WithRSAEncryption)                                      //表示该算法标识符的OID为
                    Padding: 0                                                                                          //没有任何填充数据
                    encrypted: 8dd4d42d4d4c9709824437c00682f0cfa9d47a85c14749f9…                                        //表示该证书的签名值。

```

## Server Hello Done

``` data
服务端发送 Server Hello Done 消息,表示服务端的 Server Hello 消息已经发送完毕,客户端可以开始发送客户端的密钥交换消息了。

TLSv1.2 Record Layer: Handshake Protocol: Server Hello Done                                 //表示这是一个TLS握手协议的Server Hello Done消息。
    Content Type: Handshake (22)                                                            //表示这是一个握手消息。
    Version: TLS 1.2 (0x0303)                                                               //表示这是一个TLSv1.2协议的消息。
    Length: 4                                                                               //表示这个消息的长度为4个字节。
    Handshake Protocol: Server Hello Done                                                   //表示这是一个Server Hello Done消息。
        Handshake Type: Server Hello Done (14)                                              //值为14,表示这是一个Server Hello Done消息。
        Length: 0                                                                           //表示这个消息的长度为0个字节。                    

```



> TLSv1.2 Record Layer: Alert (Level: Warning, Description: Close Notify)
-     表示在这个TLS会话中发生了一个关闭通知警告。
-     在 TLS 中,当两个通信方决定结束通信时,它们可以发送一个关闭通知警告,以告知对方正在关闭通信。这是一个正常的结束流程,因此它的警告级别是 "Warning",而不是 "Fatal"。





## wireshark解密数据
>    通过服务端私钥解密数据的前提条件
- 服务器选择的密码套件未使用 （EC）DHE
- 不支持tls1.3
- 必须包含握手消息

> 配置方法：
-   打开wireshark，选择编辑->首选项->Protocols->TLS->RSA keys list->Edit
-   配置服务端地址端口和私钥文件和密码(如果有的话)




## syn中包含协商的mtu值
    MSS Value: 1260  这个值就是mtu