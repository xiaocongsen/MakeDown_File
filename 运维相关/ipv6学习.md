# 学习

## 介绍
    16个字节 128位 2个字符等于一个字节 最大FFFF
    ABCD:EF01:2345:6789:ABCD:EF01:2345:6789
    缩写：
    2001:0DB8:0000:0023:0008:0800:200C:417A→ 2001:DB8:0:23:8:800:200C:417A
    FF01:0:0:0:0:0:0:1101 → FF01::1101
    0:0:0:0:0:0:0:1 → ::1
    0:0:0:0:0:0:0:0 → ::
    内嵌IPv4地址表示法：前面96个位使用冒分十六进制表示，后面32位使用IPV4点分十进制表示
    ::192.168.0.1
    ::FFFF:192.168.0.1
    子网前缀长度:
    FC00::/7    前缀保留最左边7个比特位
    FC00::/96   前缀保留最左右96个比特位


## ipv6特殊地址段

IPv6中有很多地址段具有特殊的意义，它们用于不同的目的，包括但不限于测试、环回、多播、未指定地址、全局单播地址等。下面是一些重要的特殊地址段：

- 未指定地址 (Unspecified Address):
    地址: ::/128
    用途: 表示一个尚未确定的地址，常用于初始化状态或错误条件。
- 环回地址 (Loopback Address):
    地址: ::1/128
    用途: 用于发送数据包回到发送主机，类似于IPv4中的127.0.0.1。
- 链路本地地址 (Link-Local Address):
    地址: FE80::/10
    用途: 用于同一链路内的节点间通信，不被路由器转发。
- 唯一本地地址 (Unique Local Address):
    地址: FC00::/7  (包括FD00::/8和不常用的FC00::/8)
    用途: 私有网络内使用，不被全球互联网路由。
- 多播地址(组播地址) (Multicast Address):
    地址: FF00::/8
    用途: 用于向一组节点发送数据包，分为不同范围的多播地址，如节点本地、链路本地、站点本地和组织本地多播。

全局单播地址 (Global Unicast Address):
地址: 2000::/3
用途: 全球唯一，用于互联网上的端到端通信。
嵌入IPv4的IPv6地址 (IPv4-Mapped and IPv4-Compatible IPv6 Address):
地址: ::/96, ::ffff:0:0/96
用途: 用于IPv4和IPv6之间的转换，其中::ffff:0:0/96用于IPv4映射地址，::/96用于IPv4兼容地址。
6to4地址 (6to4 Address):
地址: 2002::/16
用途: 一种过渡技术，允许IPv6节点通过IPv4网络通信。
teredo地址 (Teredo Address):
地址: 2001::/32中的某些范围
用途: 另一种过渡技术，允许IPv6通过NAT穿越IPv4网络。
6rd地址 (6rd Address):
地址: 2001:0000::/32中的某些范围
用途: 服务提供商部署的另一种IPv6-over-IPv4隧道技术。
任播地址 (Anycast Address):
地址: 全局单播地址的一个子集，没有专门的地址段，但其特性取决于如何在网络中配置和使用。
以上地址段是IPv6中最常见的特殊用途地址，它们帮助实现网络的各种功能，并确保IPv6地址空间的有效利用。




    inet_pton(AF_INET6, " 1111::1", &addr)：这个函数会把一个点分十进制或冒号分十六进制的字符串转换成一个struct in6_addr。
    inet_ntop(AF_INET6, &addr, str, sizeof(str))：这个函数会把一个struct in6_addr转换成一个点分十进制或冒号分十六进制的字符串。

*pcChr == 

void PrintRoute6(MIB_IPFORWARD_ROW2 *route) {
    MIB_IPFORWARD_TABLE2 *pIpForwardTable = NULL;
        DWORD dwRetVal = 0;

        dwRetVal = GetIpForwardTable2(AF_INET6, &pIpForwardTable);

        if (dwRetVal == NO_ERROR) {
            printf("Number of entries: %lu\n", pIpForwardTable->NumEntries);

            for (ULONG i = 0; i < pIpForwardTable->NumEntries; i++) {
                char destAddr[INET6_ADDRSTRLEN];
                char nextHop[INET6_ADDRSTRLEN];

                inet_ntop(AF_INET6, &(pIpForwardTable->Table[i].DestinationPrefix.Prefix.Ipv6.sin6_addr), destAddr, sizeof(destAddr));
                inet_ntop(AF_INET6, &(pIpForwardTable->Table[i].NextHop.Ipv6.sin6_addr), nextHop, sizeof(nextHop));

                DDM_Log(DDM_LOG_ROUTE, DDM_LOG_LEVEL_INFO, "Destination: %s/%u", destAddr, pIpForwardTable->Table[i].DestinationPrefix.PrefixLength);
                DDM_Log(DDM_LOG_ROUTE, DDM_LOG_LEVEL_INFO, "Next Hop: %s", nextHop);
                DDM_Log(DDM_LOG_ROUTE, DDM_LOG_LEVEL_INFO, "Interface Index: %lu", pIpForwardTable->Table[i].InterfaceIndex);
                DDM_Log(DDM_LOG_ROUTE, DDM_LOG_LEVEL_INFO, "Metric: %lu", pIpForwardTable->Table[i].Metric);
            }
        } else {
            DDM_Log(DDM_LOG_ROUTE, DDM_LOG_LEVEL_INFO, "GetIpForwardTable2 failed with error: %lu\n", dwRetVal);
        }

}




MIB_IPFORWARD_ROW2 bestRoute;
       SOCKADDR_INET destAddr = {{0}};
       SOCKADDR_INET bestSourceAddr = {{0}};

       // 设置目标地址
       destAddr.si_family = AF_INET6;
       inet_pton(AF_INET6, "2402:4e00:1013:e500:0:9671:f018:4947", &destAddr.Ipv6.sin6_addr);

       // 获取最佳路由
       DWORD result = GetBestRoute2(NULL, 0, NULL, &destAddr, 0, &bestRoute, &bestSourceAddr);
       if (result == NO_ERROR) {
           char bestSource[INET6_ADDRSTRLEN];
           char nextHop[INET6_ADDRSTRLEN];

           inet_ntop(AF_INET6, &bestSourceAddr.Ipv6.sin6_addr, bestSource, sizeof(bestSource));
           inet_ntop(AF_INET6, &bestRoute.NextHop.Ipv6.sin6_addr, nextHop, sizeof(nextHop));

           DDM_Log(DDM_LOG_ROUTE, DDM_LOG_LEVEL_ERROR, "Best route found!");
           DDM_Log(DDM_LOG_ROUTE, DDM_LOG_LEVEL_ERROR, "Best source address: %s", bestSource);
           DDM_Log(DDM_LOG_ROUTE, DDM_LOG_LEVEL_ERROR, "Next hop: %s", nextHop);
           DDM_Log(DDM_LOG_ROUTE, DDM_LOG_LEVEL_ERROR, "Interface Index: %lu", bestRoute.InterfaceIndex);
           DDM_Log(DDM_LOG_ROUTE, DDM_LOG_LEVEL_ERROR, "Metric: %lu", bestRoute.Metric);
       } else {
           DDM_Log(DDM_LOG_ROUTE, DDM_LOG_LEVEL_ERROR, "GetBestRoute2 failed with error: %lu", result);
       }
