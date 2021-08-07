
# tgt

# 安装    
        apt -y install tgt
## 如需要支持:
    查看是否支持rbd     tgtadm --lld iscsi --mode system --op show|grep rbd  
    安装rbd             apt -y install tgt-rbd
    systemctl restart tgt  重启tgt
# 建立iscsi目标
    vim /etc/tgt/conf.d/iscsi.conf
    写入:

```
    结合ceph的配置
    <target example:iscsi>
            driver iscsi
            bs-type rbd
            backing-store pvepool/iscsi-image2
            incominguser laopi xcs1234567890i
            outgoinguser laopiout xcs1234567890o
    </target>
```
    systemctl restart tgt  重启tgt

# tgtadm命令
tgtadm [OPTIONS]... [-C --control-port <port>] [-L --lld <driver>] [-o --op <operation>] [-m --mode <mode>] [-t --tid <id>]
                           [-T --targetname <targetname>] [-y --blocksize <size>] [-Y --device-type <type>] [-l --lun <lun>]
                           [-b --backing-store <path>] [-f --bsoflags {direct|sync}] [-S --bsopts {backing-store opt string}]
                           [-E --bstype <type>] [-I --initiator-address <address>] [-Q --initiator-name <name>] [-n --name <parameter>]
                           [-v --value <value>] [-P --params <param=value[,param=value...]>] [-F --force] [-h --help]
tgtadm --lld [driver] --op [operation] --mode [mode] [OPTION]...
driver驱动程序是iSCSI
mode指定操作的对象，为target，logicalunit等
operation对指定的对象所要做的操作，有delete,new,bind,show,unbind等
OPTION常用选项:
-t --tid :用来指定Target的ID
-T --targetname ：指定Target名称
Target的名称格式：
iqn.xxxx-yy.reversedoamin.STRING[:substring]
其中：
iqn为iqn前缀；
xxxx为年份；
yy为月份；
reversedomain为所在域名的反写；
STRING为字符串；
substring为子字符串；
如 iqn.2015-11.com.a.web:server1
-l --lun :指定lun的号码
-b --backing-store：关联到指定lun上的后端存储设备，此例为分区
-E, --bstype <type>
-I --initiator-address：指定可以访问Target的IP地址
具体用法请man  tgtadm
-C 可以在主机上运行多个tgtd并发实例。此参数用于控制tgtadm命令将在哪个实例上运行。
-y 创建的块设备的默认块大小为512字节。此参数可用于创建具有不同块大小的块设备。
-Y 创建LUN时，此参数指定要创建的设备的类型。默认为磁盘。
如：
1. 创建一个target      tgtadm --lld iscsi --op new --mode target --tid 1 -T iqn.2013-05.com.magedu:tsan.disk1
2. 显示所有             tgtadm --lld iscsi --op show --mode target
3. 显示刚创建的target   tgtadm --lld iscsi --op show --mode target --tid 1
4. 创建LUN，号码为1     tgtadm --lld iscsi --op new --mode logicalunit --tid 1 --lun 1 -b /dev/sda1
5. 开放给192.168.0.0/24网络中的主机访问(其中的-I相当于--initiator-address)：
tgtadm --lld iscsi --op bind --mode target --tid 1 -I 192.168.85.0/24
6. 创建一个账户 tgtadm --lld iscsi --op new --mode account --user administrator --password 123456 或者 tgtadm --lld iscsi --op show --mode account
7. 将帐户分配给目标
    tgtadm --lld iscsi --op bind --mode account --tid 1 --user administrator 或者 tgtadm --lld iscsi --op show --mode target
8. 设置 outgoing account
    需要新一个账户 假如有个新账户abc  tgtadm --lld iscsi --op bind --mode account --tid 1 --user abc --outgoing 查看 tgtadm --lld iscsi --op show --mode target
9. 删除某个target
    tgtadm --lld iscsi --op delete --mode target --tid=3
10. 获取当前内存的tgt配置 
    tgt-admin --dump > ./targets.conf
11. 删除某个账户
     tgtadm --lld iscsi --op delete --mode account --user root
12. 设置和取消只读
    tgtadm --lld iscsi --mode logicalunit --op update --tid 1 --lun 1 --params readonly=1   设置
    tgtadm --lld iscsi --mode logicalunit --op update --tid 2 --lun 1 --params readonly=0   取消
13. 设置成可移动设备
    tgtadm --lld iscsi --mode logicalunit --op update --tid 2 --lun 1 --params removable=1


# 新建target
    tgtadm --lld iscsi --op new --mode target --tid 3 -T example:iscsi2     //创建目标
    tgtadm --lld iscsi --op new --mode logicalunit --tid 3 --lun 1 -b /dev/sdb      //建立和硬盘直接的联系
    tgtadm --op bind --mode target --tid 3 --lld iscsi -I ALL       //设置任意授权
    tgtadm --lld iscsi --op new --mode account --user root --password 110001101123      //密码必须12位以上 不然无法连接
    tgtadm --lld iscsi --op bind --mode account --tid 1 --user root
    
# 注销target
    tgtadm --lld iscsi --op unbind --mode account --tid 1 --user root
    tgtadm --lld iscsi --op delete --mode account --user root
    tgtadm --lld iscsi --op delete --mode target --tid=1


# 新建rbd target
    tgtadm --lld iscsi --op new --mode target --tid 1 -T example:iscsi1     //创建目标
    tgtadm --lld iscsi --op new --mode logicalunit --lun 1 --tid 1 -E rbd -b pvepool/iscsi-image1   //对应tid 设置参数
    tgtadm --op update --mode lu --tid 3 --lun 1 --lld iscsi --params vendor_id=iscsi,product_id=sheep,product_rev=0010,scsi_sn=37a3b5,scsi_id=37a3b5,removable=0   //更新？？参数
    tgtadm --op bind --lld iscsi --mode target --tid 3 -I 192.168.70.1 //设置访问权限  tgtadm --op bind --mode target --tid 3 --lld iscsi -I ALL       //设置任意授权
    tgtadm --op unbind --lld iscsi --mode target --tid 3 -I ALL 去掉任意访问的授权
    tgtadm --lld iscsi --op new --mode account --user administrator --password 123456   //创建账户
    tgtadm --lld iscsi --op bind --mode account --tid 3 --user administrator            //绑定账户





# win客户端连接
## 命令模式
### 设置msiscsi服务的启动状态
    sc config msiscsi start= auto
    net start msiscsi
### 使用iscsicli命令连接Target
    iscsicli QAddTargetPortal 192.168.70.130       发现tgt服务端
    iscsicli ListTargets                            显示连接目标列表    
    iscsicli QloginTarget example:iscsi1 [CHAP Username] [CHAP Password]            连接example:iscsi1 如果有chap 但是如果通过PersistentLoginTarget设置了永久连接ListPersistentTargets还是能查到
    iscsicli reporttargetmappings                   查看当前电脑连接信息
    iscsicli logouttarget <SessionId>               断开指定连接 SessionId通过reporttargetmappings获得
    iscsicli PersistentLoginTarget example:iscsi1 T * * * * * * * * * * * * * * * 0     配置持久登录到目标example:iscsi1
    iscsicli ListPersistentTargets                  查看当前电脑的永久连接信息
    iscsicli RemovePersistentTarget ROOT\ISCSIPRT\0000_0 example:iscsi1 * 192.168.70.130 3260      断开指定连接并清除永久连接状态
    iscsicli QAddTarget example:iscsi2 192.168.70.130      增加新目标(不管存在不存在)到连接目标列表ListTargets就可以查看到了
    iscsicli RemoveTarget example:iscsi3            删除目标列表

# linux客户端连接
发现target
             iscsiadm -m discovery -t st -p 127.0.0.1:3260
创建块设备，创建完成后lsblk可以发现新设备
             iscsiadm -m node --targetname tgt1 --portal 127.0.0.1:3260 -l
删除设备
            iscsiadm -m node --targetname tgt1 --portal 127.0.0.1:3260 -u
            iscsiadm -m node -o delete -T tgt1 --portal 127.0.0.1:3260



# win命令磁盘格式化
    diskpart                        进入diskpart
    list disk                       显示磁盘列表
    select disk 3
    clean
    convert gpt   或者convert mbr
    list partition                  显示所选磁盘上的分区列表。
    create partition primary        创建了指定分区
    format fs=fat32 quick label='个人空间'      快速格式化磁盘并命名 大空间分区必须fs=ntfs
    assign                          分配了驱动器号或装载点
    exit                            退出diskpart

# win的diskpart所有命令
    ACTIVE      - 将选中的分区标记为活动的分区。
    ADD         - 将镜像添加到一个简单卷。
    ASSIGN      - 给所选卷分配一个驱动器号或装载点。
    ATTRIBUTES  - 操纵卷或磁盘属性。
    ATTACH      - 连接虚拟磁盘文件。
    AUTOMOUNT   - 启用和禁用基本卷的自动装载。
    BREAK       - 中断镜像集。
    CLEAN       - 从磁盘清除配置信息或所有信息。
    COMPACT     - 尝试减少文件的物理大小。
    CONVERT     - 在不同的磁盘格式之间转换。
    CREATE      - 创建卷、分区或虚拟磁盘。
    DELETE      - 删除对象。
    DETAIL      - 提供对象详细信息。
    DETACH      - 分离虚拟磁盘文件。
    EXIT        - 退出 DiskPart。
    EXTEND      - 扩展卷。
    EXPAND      - 扩展虚拟磁盘上可用的最大大小。
    FILESYSTEMS - 显示卷上当前和支持的文件系统
    FORMAT      - 格式化卷或分区
    GPT         - 给选择的 GPT 分区分配属性。
    HELP        - 显示命令列表。
    IMPORT      - 导入磁盘组。
    INACTIVE    - 将所选分区标为不活动。
    LIST        - 显示对象列表。
    MERGE       - 将子磁盘与其父磁盘合并。
    ONLINE      - 使当前标为脱机的对象联机。
    OFFLINE     - 使当前标记为联机的对象脱机。
    RECOVER     - 刷新所选包中所有磁盘的状态。
                尝试恢复无效包中的磁盘，并
                重新同步具有过时丛或奇偶校验数据
                的镜像卷和 RAID5 卷。
    REM         - 不起任何作用。用来注释脚本。
    REMOVE      - 删除驱动器号或装载点分配。
    REPAIR      - 用失败的成员修复一个 RAID-5 卷。
    RESCAN      - 重新扫描计算机，查找磁盘和卷。
    RETAIN      - 在一个简单卷下放置一个保留分区。
    SAN         - 显示或设置当前启动的操作系统的 SAN 策略。
    SELECT      - 将焦点移动到对象。
    SETID       - 更改分区类型。
    SHRINK      - 减小选定卷。
    UNIQUEID    - 显示或设置磁盘的 GUID 分区表(GPT)标识符或
                主启动记录(MBR)签名。


# ceph客户端自动创建iscsi目标脚本
``` shell
#/bin/bash
    if  [ ! -n "$1" ] ;then
            echo "not ceph rdb name"
            exit 1
    fi
    if  [ ! -n "$2" ] ;then
            echo "not iscsi name"
            exit 1
    fi
    if  [ ! -n "$3" ] ;then
            echo "not tid id"
            exit 1
    fi
    if  [ ! -n "$4" ] ;then
            echo "not ip"
            exit 1
    fi
    if  [ ! -n "$5" ] ;then
            echo "not user name"
            exit 1
    fi
    if  [ ! -n "$6" ] ;then
            echo "not password"
            exit 1
    fi
    echo "成功"
    rbd create --size 512 pvepool/$1
    tgtadm --lld iscsi --op new --mode target --tid $3 -T $2
    tgtadm --lld iscsi --op new --mode logicalunit --lun 1 --tid $3 -E rbd -b pvepool/$1
    tgtadm --op bind --lld iscsi --mode target --tid $3 -I $4
    tgtadm --lld iscsi --op new --mode account --user $5 --password $6
    tgtadm --lld iscsi --op bind --mode account --tid $3 --user $5
```





iscsicli AddTarget <TargetName> <TargetAlias> <TargetPortalAddress>
                           <TargetPortalSocket> <Target flags>
                           <Persist> <Login Flags> <Header Digest> <Data Digest>
                           <Max Connections> <DefaultTime2Wait>
                           <DefaultTime2Retain> <Username> <Password> <AuthType>
                           <Mapping Count> <Target Lun> <OS Bus> <Os Target>
                           <OS Lun> ...

iscsicli RemoveTarget <TargetName>

iscsicli AddTargetPortal <TargetPortalAddress> <TargetPortalSocket>
                         [HBA Name] [Port Number]
                         <Security Flags>
                         <Login Flags> <Header Digest> <Data Digest>
                         <Max Connections> <DefaultTime2Wait>
                        <DefaultTime2Retain> <Username> <Password> <AuthType>

iscsicli RemoveTargetPortal <TargetPortalAddress> <TargetPortalSocket> [HBA Name] [Port Number]

iscsicli RefreshTargetPortal <TargetPortalAddress> <TargetPortalSocket> [HBA Name] [Port Number]

iscsicli ListTargets [ForceUpdate]

iscsicli ListTargetPortals

iscsicli TargetInfo <TargetName> [Discovery Mechanism]

iscsicli LoginTarget <TargetName> <ReportToPNP>
                     <TargetPortalAddress> <TargetPortalSocket>
                     <InitiatorInstance> <Port number> <Security Flags>
                    <Login Flags> <Header Digest> <Data Digest>
                    <Max Connections> <DefaultTime2Wait>
                    <DefaultTime2Retain> <Username> <Password> <AuthType> <Key>
                    <Mapping Count> <Target Lun> <OS Bus> <Os Target>
                    <OS Lun> ...

iscsicli LogoutTarget <SessionId>

iscsicli PersistentLoginTarget <TargetName> <ReportToPNP>
                     <TargetPortalAddress> <TargetPortalSocket>
                    <InitiatorInstance> <Port number> <Security Flags>
                    <Login Flags> <Header Digest> <Data Digest>
                    <Max Connections> <DefaultTime2Wait>
                    <DefaultTime2Retain> <Username> <Password> <AuthType> <Key>
                    <Mapping Count> <Target Lun> <OS Bus> <Os Target>
                    <OS Lun> ...

iscsicli ListPersistentTargets

iscsicli RemovePersistentTarget <Initiator Name> <TargetName>
                               <Port Number>
                               <Target Portal Address>
                                <Target Portal Socket>

iscsicli AddConnection <SessionId> <Initiator Instance>
                      <Port Number> <Target Portal Address>
                      <Target Portal Socket> <Security Flags>
                      <Login Flags> <Header Digest> <Data Digest>
                      <Max Connections> <DefaultTime2Wait>
                      <DefaultTime2Retain> <Username> <Password> <AuthType> <Key>

iscsicli RemoveConnection <SessionId> <ConnectionId>
iscsicli ScsiInquiry <SessionId> <LUN> <EvpdCmddt> <PageCode>

iscsicli ReadCapacity <SessionId> <LUN>

iscsicli ReportLUNs <SessionId>

iscsicli ReportTargetMappings

iscsicli ListInitiators

iscsicli AddiSNSServer <iSNS Server Address>

iscsicli RemoveiSNSServer <iSNS Server Address>

iscsicli RefreshiSNSServer <iSNS Server Address>

iscsicli ListiSNSServers

iscsicli FirewallExemptiSNSServer

iscsicli NodeName <node name>

iscsicli SessionList <Show Session Info>

iscsicli CHAPSecret <chap secret>

iscsicli TunnelAddr <Initiator Name> <InitiatorPort> <Destination Address> <Tunnel Address> <Persist>

iscsicli GroupKey <Key> <Persist>

iscsicli BindPersistentVolumes

iscsicli BindPersistentDevices

iscsicli ReportPersistentDevices

iscsicli AddPersistentDevice <Volume or Device Path>

iscsicli RemovePersistentDevice <Volume or Device Path>

iscsicli ClearPersistentDevices

iscsicli Ping <Initiator Name> <Address> [Request Count] [Request Size] [Request Timeout]

iscsicli GetPSKey <Initiator Name> <initiator Port> <Id Type> <Id>

iscsicli PSKey <Initiator Name> <initiator Port> <Security Flags> <Id Type> <Id> <Key> <persist>
快速命令

iscsicli QLoginTarget <TargetName>  [CHAP Username] [CHAP Password]

iscsicli QAddTarget <TargetName> <TargetPortalAddress>

iscsicli QAddTargetPortal <TargetPortalAddress>
                          [CHAP Username] [CHAP Password]

iscsicli QAddConnection <SessionId> <Initiator Instance>
                        <Target Portal Address>
                        [CHAP Username] [CHAP Password]

目标映射:
    <Target Lun> 是目标用于公开 LUN 的 LUN 值。
                 其形式必须为 0x0123456789abcdef
    <OS Bus> 是 OS 应该用于公开 LUN 的总线编号
    <OS Target> 是 OS 应该用于公开 LUN 的目标编号
    <OS LUN> 是 OS 应该用于公开 LUN 的 LUN 编号

负载 ID 类型:
    ID_IPV4_ADDR 为      1 - ID 格式为 1.2.3.4
    ID_FQDN 为           2 - ID 格式为 ComputerName
    ID_IPV6_ADDR 为      5 - ID 形式为 IPv6 地址
安全标志:
    TunnelMode 为          0x00000040
    TransportMode 为       0x00000020
    PFS Enabled 为         0x00000010
    Aggressive Mode 为     0x00000008
    Main mode 为           0x00000004
    IPSEC/IKE Enabled 为   0x00000002
    Valid Flags 为         0x00000001

登录标志:
    ISCSI_LOGIN_FLAG_REQUIRE_IPSEC                0x00000001
        该操作需要 IPSec

    ISCSI_LOGIN_FLAG_MULTIPATH_ENABLED            0x00000002
        为此发起程序上的目标启用多路

AuthType:
    ISCSI_NO_AUTH_TYPE = 0，
        不使用任何 iSCSI 带内身份验证

    ISCSI_CHAP_AUTH_TYPE = 1，
        单向 CHAP (使用目标对发起程序进行身份验证)

    ISCSI_MUTUAL_CHAP_AUTH_TYPE = 2
        相互 CHAP (使用目标和发起程序相互进行身份验证)

目标标志:
    ISCSI_TARGET_FLAG_HIDE_STATIC_TARGET            0x00000002
        如果设置了此标志，则只有在动态发现目标时，
        才会报告该目标。

    ISCSI_TARGET_FLAG_MERGE_TARGET_INFORMATION      0x00000004
        如果设置了此标志，则会将传递的目标信息与该目标的
        已静态配置的任何目标信息合并

可以将 CHAP 机密、CHAP 密码和 IPSec 预共享密钥指定为文本字符串
或十六进制值的序列。对于在命令行上指定的值，只有头两个字母为 0x 时，
才会将该值当作十六进制值，而其他形式的值则会始终被当作字符串。

例如 0x12345678 指定了一个 4 字节的机密

除了以 0x 开头的数值之外，其他所有数值都会被假定为十进制值。
以 0x 开头的数值会被假定为十六进制值

也可以在命令行模式中运行 iscsicli。在命令行模式中，可以直接从控制台
输入 iscsicli 命令。若要进入命令行模式，
只需不带任何参数运行 iscsicli