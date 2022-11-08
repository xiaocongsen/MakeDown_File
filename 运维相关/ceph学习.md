# 一 
1. 添加 release key ：

    wget -q -O- 'https://download.ceph.com/keys/release.asc' | sudo apt-key add -

2. 添加Ceph软件包源，用Ceph稳定版（如 cuttlefish 、 dumpling 、 emperor 、 firefly 等等）替换掉 {ceph-stable-release} 。例如：

    echo deb http://download.ceph.com/debian-firefly/ $(lsb_release -sc) main | sudo tee /etc/apt/sources.list.d/ceph.list

3. 更新你的仓库，并安装 ceph-deploy ：

    sudo apt update && sudo apt install ceph-deploy

- 错误：No module named 'ceph_deploy'
    apt install python-setuptools
    python 版本问题  ，注意检查系统的python版本，此处ceph-deploy实则为通过执行python脚本来实现安装，更改/usr/bin/ceph-deploy文件中   更改python2.6的部分为2.7（此处应当与系统python版本相同）   更改第一行文件为#！/usr/bin/python2.7  重新执行ceph-deploy new即可
# 二 CEPH 节点安装
你的管理节点必须能够通过 SSH 无密码地访问各 Ceph 节点。如果 ceph-deploy 以某个普通用户登录，那么这个用户必须有无密码使用 sudo 的权限。

1. 安装 NTP
    sudo apt install ntp
2. 安装 SSH 服务器
    sudo apt install openssh-server
3. 创建部署 CEPH 的用户

4. 允许无密码 SSH 登录

5. 引导时联网
Ceph 的各 OSD 进程通过网络互联并向 Monitors 上报自己的状态。如果网络默认为 off ，那么 Ceph 集群在启动时就不能上线，直到你打开网络。

6. 确保联通性
    用 ping 短主机名（ hostname -s ）的方式确认网络联通性。解决掉可能存在的主机名解析问题。

7. 开放所需端口
    Ceph Monitors 之间默认使用 6789 端口通信， OSD 之间默认用 6800:7300 这个范围内的端口通信。

8. 终端（ TTY ）
在 CentOS 和 RHEL 上执行 ceph-deploy 命令时可能会报错。如果你的 Ceph 节点默认设置了 requiretty ，执行 sudo visudo 禁用它，并找到 Defaults requiretty 选项，把它改为 Defaults:ceph !requiretty 或者直接注释掉，这样 ceph-deploy 就可以用之前创建的用户（创建部署 Ceph 的用户 ）连接了。





# ceph集群
## 使用Proxmox VE和VMware搭建ceph
1. 下载Proxmox VE镜像并选择debian在vm下安装
    需要安装3台
    hostname 分别为pve pve1 pve2
    ip 分别为200 201 202
2. 修改源
    都执行：
```shell
    vi /etc/apt/sources.list
    #deb http://deb.debian.org/debian buster main contrib
    deb http://mirrors.163.com/debian buster main contrib
    #deb http://deb.debian.org/debian buster-updates main contrib
    deb http://mirrors.163.com/debian buster-updates main contrib
    
    # security updates
    #deb http://security.debian.org buster/updates main contrib
    deb http://mirrors.163.com/debian-security buster/updates main contrib

    vi /etc/apt/sources.list.d/pve-enterprise.list 
    #deb https://enterprise.proxmox.com/debian/pve buster pve-enterprise
    
    #deb http://download.proxmox.com/debian/pve buster pve-no-subscription
    deb http://mirrors.ustc.edu.cn/proxmox/debian/pve buster pve-no-subscription

    vi /etc/apt/sources.list.d/ceph.list 
    #deb http://download.proxmox.com/debian/ceph-nautilus buster main
    deb http://mirrors.ustc.edu.cn/proxmox/debian/ceph-nautilus buster main

    apt update
    apt upgrade
```
    sed -i_orig "s/data.status !== 'Active'/false/g" /usr/share/pve-manager/js/pvemanagerlib.js         //关闭订阅

3. 建立ceph集群
    1) 一个节点建立集群pvecm create <clustername>
    2) 其他节点加入集群pvecm add 创建集群机器的ip 查看集群状态pvecm node或者pvecm status 
    3) 安装:pveceph install 安装最新的ceph-nautilus版
    4) 配置ceph网络每个节点都执行pveceph init -network 10.8.20.0/24
    5) 创建ceph集群存储Mon监控 都执行pveceph createmon     查看状态ceph mon stat
    6) 创建mgr    都执行pveceph createmgr
    6) 创建ceph集群存储OSD服务 都执行pveceph createosd /dev/sdb    查看状态ceph osd stat或者ceph osd tree
    7) 创建存储池 ceph osd pool create pvepool 128 128
    8)  同步集群机器时间 安装apt install ntpdate 执行ntpdate 120.25.108.11  并设置定时任务echo "0 * * * * /usr/sbin/ntpdate 120.25.108.11 > /dev/null 2>&1" >> /etc/crontab

4. 建立ceph客户端

5. Proxmox VE内ceph命令
    pveceph pool ls
    pveceph pool destroy poolname
    pveceph pool create poolname
    pveceph osd create /dev/sdb
    

# 命令
service ceph start  mon.node1   启动mon进程 node1就是hostname
service ceph start mds.node1    启动msd进程
service ceph start osd.0    启动osd进程

ceph-deploy purge node1             
ceph-deploy purgedata node1         删除一个节点的所有ceph数据包

ceph health detail  查看集群的监控状态
ceph -w             查看ceph的实时运行状态
ceph -s             检查状态信息
ceph df             查看ceph存储空间
ceph auth get-or-create client.admin mds 'allow' osd 'allow *' mon 'allow *' > /etc/ceph/ceph.client.admin.keyring
ceph auth get-or-create client.admin mds 'allow' osd 'allow *' mon 'allow *' -o /etc/ceph/ceph.client.admin.keyring     为ceph创建一个admin用户并未admin用户创建一个秘钥，把秘钥保存到/etc/ceph目录下
ceph auth get-or-create osd.0 mon 'allow rwx' osd 'allow *' -o /var/lib/ceph/osd/ceph-0/keyring     为osd.0创建一个用户并创建一个key
ceph auth get-or-create mds.node1 mon 'allow rwx' osd 'allow *' mds 'allow *' -o /var/lib/ceph/mds/ceph-node1/keyring       为mds.node1创建一个用户并创建一个key
ceph auth list      查看ceph集群中的认证用户及相关的key
ceph auth del osd.0 删除急群众的一个认证用户
ceph daemon mon.node1 config show | more    查看集群的详细配置
ceph-conf --name mon.node1 --show-config-value log_fileconfig     查看cephlog日志所在的目录

## monitor命令
ceph mon stat       查看monitor的状态信息
ceph quorum_status  查看mon的选举状态
ceph mon dump       查看monitor的映射信息
ceph mon remove node1   删除一个mon节点
ceph mon getmap -o 1.txt    获得一个正在运行的mon map，并保存在1.txt文件中
monmaptool --print 1.txt    查看上面获得的map数据
ceph-mon -i node4 --inject-monmap 1.txt     把上面的mon map注入新加入的节点
ceph-conf --name mon.node1 --show-config-value admin_socket     查看mon的amin socket
ceph daemon mon.node1  mon_statu    查看mon的详细状态
ceph mon remove os-node1        删除一个mon节点

## MDS命令
ceph mds stat           查看MDS的状态
ceph mds dump           查看msd的映射信息
ceph  mds rm 0 mds.node1            删除一个MSD节点
ceph mds set max_file_size 1024000000000        设置cephfs文件系统存储方式最大单个文件尺寸

## OSD命令
ceph osd stat       显示OSD状态
ceph osd df         查看osd使用量
ceph osd tree       查看osd的目录树
ceph osd perf       查看所有osd性能 0证明很快
ceph osd dump       查看osd映射信息
ceph osd down 0     down掉一个osd硬盘 #down掉osd.0节点
ceph osd rm 0       在集群中删除一个osd硬盘
ceph osd crush rm osd.0      从crush map中删除osd
ceph osd getmaxosd  查看最大osd的个数
ceph osd setmaxosd 10       设置最大的osd的个数（当扩大osd节点的时候必须扩大这个值）
设置osd crush的权重为1.0
ceph osd reweight 3 0.5     设置osd.3的权重 影响PG的到OSD的映射关系          设置reweight的值
ceph osd crush set 3 3.0 host=node4     设置osd crush的权重为1.0
ceph osd crush reweight osd.3 1.0       设置osd crush的权重为1.0            设置weight的值 一般是固定值
ceph osd out osd.3      把一个osd节点逐出集群
ceph osd in osd.3       把逐出的osd加入集群
ceph osd pause          暂停osd （暂停后整个集群不再接收数据）
ceph osd unpause        再次开启osd （开启后再次接收数据）
ceph --admin-daemon /var/run/ceph/ceph-osd.2.asok config show | less查看一个集群osd.2参数的配置

## PG组
ceph pg dump        查看PG组的映射信息
ceph pg map 0.3f    查看一个PG的map
ceph pg stat        查看PG状态
ceph pg  0.26 query 查询一个pg的详细信息
ceph pg dump_stuck unclean      查看pg中stuck的unclean状态
ceph pg dump_stuck inactive     查看pg中stuck的inactive状态
ceph pg dump_stuck stale        查看pg中stuck的stale状态
ceph pg dump --format plain     显示一个集群中的所有的pg统计
ceph pg {pg-id} mark_unfound_lost revert    恢复一个丢失的pg

## Pool
ceph osd lspools    查看ceph集群中的pool数量
ceph osd pool create pvepool 100    在ceph集群中创建一个pool  通常在创建pool之前，需要覆盖默认的pg_num，官方推荐：若少于5个OSD， 设置pg_num为128。5~10个OSD，设置pg_num为512。10~50个OSD，设置pg_num为4096。超过50个OSD，可以参考pgcalc计算。
ceph osd pool set-quota data max_objects 10000  为一个ceph pool配置配额
ceph osd pool delete pvepool  pvepool  --yes-i-really-really-mean-it    在集群中删除一个pool
rados df   显示集群中pool的详细信息
ceph osd pool mksnap data   date-snap   给一个pool创建一个快照
ceph osd pool rmsnap data date-snap     删除pool的快照
ceph osd pool get data pg_num           查看data池的pg数量
ceph osd pool set data target_max_bytes 100000000000000 设置data池的最大存储空间为100T（默认是1T)
ceph osd pool set data size 3           设置data池的副本数是3
ceph osd pool set data min_size 1       设置data池能接受写操作的最小副本为1
ceph osd dump | grep 'replicated size'  查看集群中所有pool的副本尺寸
ceph osd pool set data pg_num 100       设置一个pool的pg数量
ceph osd pool set data pgp_num 100      设置一个pool的pgp数量
ceph osd pool set-quota data max_objects 2048000    设置一个池中object的大小

## rados指令
rados lspools   查看ceph集群中有多少个poll（只是查看pool）
rados df        查看ceph集群中有多少个pool,并且每个pool容量及利用情况
rados mkpool pvepool   创建一个pool
rados ls -p pvepool | more  查看ceph pool中的ceph object （这里的object是以块形式存储的）
rados create test-object -p pvepool    创建一个对象object
rados rm test-object-1 -p pvepool      删除一个对象

## rbd命令
rbd ls images       查看ceph中一个pool里的所有镜像
rbd info -p images --image 74cb427c-cee9-47d0-b467-af217a67e60a 查看ceph pool中一个镜像的信息
rbd create -p pvepool --size 10000 iscsi     在pvepool池中创建一个命名为iscsi的10000M的镜像
rbd -p pvepool info iscsi    #查看新建的镜像的信息
rbd rm  -p pvepool  iscsi      删除一个镜像
rbd resize -p pvepool --size 20000 iscsi 调整一个镜像的尺寸
rbd   snap ls  -p pvepool zhanguo      给一个镜像创建一个快照
rbd info pvepool/iscsi
rbd snap ls -p pvepool volume-7687988d-16ef-4814-8a2c-3fbd85e928e4     查看一个镜像文件的快照  
rbd snap unprotect pvepool/volume-7687988d-16ef-4814-8a2c-3fbd85e928e4@snapshot-ee7862aa-825e-4004-9587-879d60430a12     删除一个镜像文件的一个快照快照的写保护
rbd snap rm pvepool/volume-7687988d-16ef-4814-8a2c-3fbd85e928e4@snapshot-ee7862aa-825e-4004-9587-879d60430a12     删除一个镜像文件的一个快照快照
rbd snap purge -p pvepool volume-7687988d-16ef-4814-8a2c-3fbd85e928e4  删除一个镜像文件的所有快照
rbd export -p pvepool --image 74cb427c-cee9-47d0-b467-af217a67e60a /root/aaa.img 把ceph pool中的一个镜像导出到本地
rbd export -p pvepool --image volume-470fee37-b950-4eef-a595-d7def334a5d6 /var/lib/glance/ceph-pool/pvepool/Message-JiaoBenJi-10.40.212.24  把ceph pool中的一个镜像导出到云盘
rbd import /root/aaa.img -p pvepool --image 74cb427c-cee9-47d0-b467-af217a67e60a     把一个镜像导入ceph中 （但是直接导入是不能用的，因为没有经过openstack,openstack是看不到的）


## Ceph CRUSH规则
ceph osd getcrushmap -o test        先把crush map以二进制的形式导入到test文本
crushtool -d test -o test1          再用crushtool工具把test里的二进制数据转换成文本形式保存到test1文档里

crushtool -c test1 -o test2    #先把test1 转换成二进制形式
ceph osd setcrushmap -i test2  #再把test2导入集群

# 其他
ceph crash ls-new   查看新的错误记录列表
ceph crash info <crash-id>  查看有关的特定错误
ceph crash archive <crash-id>   存档某条历史错误信息
ceph crash archive-all  存档所有历史错误信息 存档后不再提示

ceph osd set noout      关闭集群自动平衡
pveceph createosd /dev/sde  
ceph osd crush add osd.9 1 host=pve
ceph osd stop osd.{num}
ceph osd start osd.{num}
ceph osd unset noout

ceph osd set noscrub            设置标志位防止集群做清洗操作
ceph osd set nodeep-scrub       防止集群进行深度清洗操作
ceph tell osd.* injectargs '--osd_scrub_begin_hour 0'       设置osd scrub开始时间
ceph tell osd.* injectargs '--osd_scrub_end_hour 5'         设置osd scrub结束时间
ceph tell mon.* injectargs '--osd_scrub_begin_hour 0'       设置mon scrub开始时间
ceph tell mon.* injectargs '--osd_scrub_end_hour 5'         设置mon scrub结束时间
ceph osd unset noscrub              
ceph osd unset nodeep-scrub             关闭标志位


rados rmpool rbd rbd --yes-i-really-really-mean-it 
rbd ls pvepool      查询所有在pvepool创建的image
rbd diff pvepool/iscsi-1 | awk '{ SUM += $2 }END { print SUM/1024 /1024 "MB" }'     查看iscsi-1真实占用多少空间

# 移除osd
    ceph osd out $1
    sleep 2
    systemctl stop ceph-osd@$1.service
    sleep 2
    ceph osd crush remove osd.$1
    sleep 2
    ceph auth del osd.$1
    sleep 2
    ceph osd rm $1
    sleep 2
    if [ -d "/var/lib/ceph/osd/ceph-$1" ];then
        umount /var/lib/ceph/osd/ceph-$1
        sleep 2
        rm -rf /var/lib/ceph/osd/ceph-$1
    fi
    lvremove -y /dev/ceph-3aa53c5f-1cec-4be8-93d2-e74c4c8cadc7/osd-block-8962ee76-8f69-42b4-ae5c-66284ced7152
    vgremove -y ceph-3aa53c5f-1cec-4be8-93d2-e74c4c8cadc7
    pvremove -y /dev/sdb
```shell
    ceph osd out $1
    sleep 1
    systemctl stop ceph-osd@$1.service
    sleep 1
    ceph osd crush remove osd.$1
    sleep 1
    ceph auth del osd.$1
    sleep 1
    ceph osd rm $1
    sleep 1
    if [ -d "/var/lib/ceph/osd/ceph-$1" ];then
        umount /var/lib/ceph/osd/ceph-$1

    fi
    var=$(ls -l /var/lib/ceph/osd/ceph-*/block | awk -F "/var/lib/ceph/osd/ceph-" '{print $2}' | awk -F "/block -> /dev/ceph-" '{print $1}' |wc -l)
    for i in $(seq $var -1 1)
    do
        num=$(ls -l /var/lib/ceph/osd/ceph-*/block | awk -F "/var/lib/ceph/osd/ceph-" '{print $2}' | awk -F "/block -> /dev/ceph-" '{print $1}' | awk 'NR=='$i'')
        if [ $num == $1 ] ;then
            id=$(ls -l /var/lib/ceph/osd/ceph-*/block | awk -F "/var/lib/ceph/osd/ceph-" '{print $2}' | awk -F "/block -> /dev/ceph-" '{print $2}' | awk 'NR=='$i'')
            lvremove -y /dev/ceph-$id
            id=$(ls -l /var/lib/ceph/osd/ceph-*/block | awk -F "/var/lib/ceph/osd/ceph-" '{print $2}' | awk -F "/block -> /dev/ceph-" '{print $2}' | awk 'NR=='$i''| awk -F "/osd"  '{print $1}')
            vgremove -y ceph-$id
        fi
    done
    if [ -d "/var/lib/ceph/osd/ceph-$1" ];then
        rm -rf /var/lib/ceph/osd/ceph-$1
    fi        
```

# 增加osd
    pveceph createosd /dev/sde
    ceph osd tree
    ceph osd crush add osd.0 0 host=pve     设置crush reweight(weight)的值,为了不影响集群性能 加入CRUSH map时应该把OSD的初始weight设为0
    ceph osd crush reweight osd.0 0.3       然后调整weight每次增大百分之20、逐步增大 CRUSH 权重。例如每次增加总量的0.2  总量是1则为1T
    ceph osd pool set data pg_num 100       调整pg数量

# 调整reweight
1. 可以手动调整 ceph osd reweight {osd_id} {reweight_value}
2. 小范围慢慢调整：
    1)按照OSD当前空间利用率(reweight-by-utilization)来调整
        ceph osd test-reweight-by-utilization   #预估变化 显示调整后的参数
        ceph osd reweight-by-utilization
    2)按照PG在OSD之间的分布(reweight-by-pg)来调整
        ceph osd test-reweight-by-pg
        ceph osd reweight-by-pg
    需要多次执行 直到不再调整为止
3. 自动调整
    ceph balancer status                查看集群balancer状态
    ceph mgr module enable balancer     启用模块（默认）
    ceph balancer on                    启用自动平衡
    ceph balancer mode upmap        设置模式  upmap（修改PG mapping）  crush-compat（修改weight）
    ceph osd set-require-min-compat-client luminous     设置兼容版本（upmap模式需要此设置）

# 申请rbd块空间
    rbd create --size 4096 pvepool/iscsi-image2      创建rdb块4g大小的iscsi-image2
    rbd info pvepool/iscsi-image2        查看块信息
    rbd rm pvepool/iscsi-image2          删除块
    
# osd参数介绍
    osd_heartbeat_interval # osd之间传递心跳的间隔时间
    osd_heartbeat_grace # 一个osd多久没心跳，就会被集群认为它down了
    mon_osd_min_down_reporters # 确定一个osd状态为down的最少报告来osd数
    mon_osd_min_down_reports # 一个OSD必须重复报告一个osd状态为dow的次数
    mon_osd_down_out_interval # 当osd停止响应多长时间，将其标记down和out
    mon_osd_report_timeout # monitor宣布失败osd为down前的等待时间
    osd_mon_report_interval_min # 一个新的osd加入集群时，等待多长间，开始向monitor报告osd_mon_report_interval_max # monitor允osd报告的最大间隔，超时就认为它down了
    osd_mon_heartbeat_interval # osd向monitor报告心跳的时间

    osd_pool_default_min_size = 1       #PG 处于 degraded 状态不影响其 IO 能力,min_size是一个PG能接受IO的最小副本数

# 重设full_ratio
ceph osd set-full-ratio 0.85
ceph osd set-nearfull-ratio 0.75
ceph osd set-backfillfull-ratio 0.75
通知mon修改nearfull及full阈值
ceph tell mon.* injectargs "--mon-osd-nearfull-ratio 0.95"
ceph tell mon.* injectargs "--mon-osd-full-ratio 0.97"
ceph tell osd.* injectargs "--mon-osd-nearfull-ratio 0.95"
ceph tell osd.* injectargs "--mon-osd-full-ratio 0.97"

# linux挂载
    rbd create --size 4096 pvepool/iscsi-1
    rbd info pvepool/iscsi-1 
    rbd map pvepool/iscsi-1 可能报错提示执行rbd feature disable pvepool/iscsi-1 object-map fast-diff deep-flatten
    mount -o discard /dev/rbd0p1 ./data/


# ceph报错
    错误信息：application not enabled on 1 pool(s)
    解决方法ceph osd pool application enable pvepool rbd
    

noscrub flag(s) set	                                        防止集群做清洗操作	
full flag(s) set	                                        使集群到达设置的full_ratio值。会导致集群阻止写入操作	
nodeep-scrub flag(s) set	                                防止集群进行深度清洗操作	
pause flag(s) set	                                        集群将会阻止读写操作，但不会影响集群的in、out、up或down状态。集群扔保持正常运行,就是客户端无法读写	
noup flag(s) set	                                        防止osd进入up状态	
nodown flag(s) set	                                        防止osd进入down状态	
noout flag(s) set	                                        防止osd进入out状态	
noin flag(s) set	                                        防止osd纳入ceph集群。有时候我们新加入OSD，并不想立马加入集群，可以设置该选项	
nobackfill flag(s) set	                                	防止集群进行数据回填操作	
norebalance flag(s) set		                                防止数据均衡操作	
norecover flag(s) set		                                避免关闭OSD的过程中发生数据迁移	
notieragent flag(s) set		
osds exist in the crush map but not in the osdmap	        osd crush weight有值但是osd weight无值	
application not enabled on 1 pool(s)	                    没有定义池的使用类型	
osds have slow requests	                                    慢查询	
Monitor clock skew detected	                                时钟偏移	
bigdata failing to advance its oldest client/flush tid	    客户端和MDS服务器之间通信使用旧的tid	
Many clients (34) failing to respond to cache pressure	    如果某个客户端的响应时间超过了 mds_revoke_cap_timeout （默认为 60s ）这条消息就会出现	
mons down, quorum	                                        Ceph Monitor down	
in osds are down	                                        OSD down后会出现	
cache pools are missing hit_sets	                        使用cache tier后会出现	
has mon_osd_down_out_interval set to 0	                    has mon_osd_down_out_interval set to 0	
is full	                                                    pool满后会出现	
near full osd	                                            near full osd	
unscrubbed pgs	                                            有些pg没有scrub	
pgs stuck	                                                PG处于一些不健康状态的时候，会显示出来	
requests are blocked	                                    slow requests会警告	
osds have slow requests	                                    slow requests会警告	
recovery	                                                需要recovery的时候会报	
at/near target max	                                        使用cache tier的时候会警告	
too few PGs per OSD	                                        每个OSD的PG数过少	
too many PGs per OSD	                                    too many PGs per OSD	
 > pgp_num	                                                > pgp_num	
has many more objects per pg than average (too few pgs?)    每个Pg上的objects数过多	
no osds	                                                    部署完就可以看到，运行过程中不会出现	
full osd	                                                OSD满时出现	
pgs are stuck inactive for more than	                    Pg处于inactive状态，该Pg读写都不行	
scrub errors	                                            scrub 错误出现，是scrub错误?还是scrub出了不一致的pg


# ceph.conf
```ini
[global]
        auth_client_required = cephx                                #客户端认证
        auth_cluster_required = cephx                               #集群认证
        auth_service_required = cephx                               #服务认证
        public_network = 192.168.70.0/24                            #公共网络(monitorIP段) 
        cluster_network = 192.168.70.0/24                           #集群网络
        fsid = c7fc8165-7826-4c1e-9504-d863ba01692d                 #集群标识ID 
        mon_initial_members=pve,pve1,pve2
        mon_allow_pool_delete = true
        mon_host = 192.168.70.200 192.168.70.201 192.168.70.202     #monitor IP 地址
        osd_pool_default_min_size = 2
        osd_pool_default_size = 3
        mon_osd_cache_size=512                                  #要缓存到内存中的osdmap的最大数量 大集群可以增加1024
        mon_osd_max_initial_pgs=1024                            #创建一个池的最大pg数（若用户指定了比这更多的PG，那集群将在创建池之后分割PG，以便达到目标）
        mon_osd_full_ratio = 0.85                                #默认0.95#报停水位，集群任意一个OSD使用率大于等于此数值时，集群将被标记为full，此时集群停止接受客户端的写入请求
        mon_osd_nearfull_ratio = 0.70                            #默认0.85#告警水位，集群中的任一OSD空间使用率大于等于此数值时，集群将被标记为NearFull，此时集群将产生告警，并提示所有已经处于NearFull状态的OSD
        mon_osd_min_in_ratio=0.75                               #在把 OSD 标记为 out 前，保持处于 in 状态的 OSD 最小比例。
        mon_osd_min_up_ratio=0.3                                #在把 OSD 标记为 down 前，保持处于 up 状态的 OSD 最小比例。
        mon_osd_backfillfull_ratio=0.75                         #默认0.90#OSD空间使用率大于等于此数值时，拒绝PG通过Backfill方式迁出或者继续迁入本OSD
        osd_failsafe_full_ratio=0.90                            #默认0.97#PG执行包含写操作的op时，防止所在的OSD磁盘空间被100%写满的最后一道屏障，超过此限制时，op将直接被丢弃
        mon_max_pg_per_osd=250                                  #集群允许的每个OSD的最大pg数（单个节点OSD少的情况，建议可以调大些）
        osd_scrub_chunk_min=5                                   #在单个块中要清洗的对象的最小数量
        osd_scrub_chunk_max=25                                  #在单个块中要清洗的最大对象数
        osd_max_scrubs=1                                        #单个OSD上的最大并发清洗数量
        osd_deep_scrub_randomize_ratio = 0.01
        max_open_files = 131072                                 #如果设置了该选项，Ceph会设置系统的max open fds

[mon]
        mon_data = /var/lib/ceph/mon/ceph-$id                   #$id == pve
        mon_clock_drift_allowed = 1                             #默认值0.05#monitor间的clock drift
        mon_osd_min_down_reporters = 13                         #默认值1#向monitor报告down的最小OSD数
        mon_osd_down_out_interval = 600      #默认值300      #标记一个OSD状态为down和out之前ceph等待的秒数

[osd]
        osd_data = /var/lib/ceph/osd/ceph-$id
        osd_mkfs_type = xfs                             #格式化系统类型
        filestore_xattr_use_omap=true                   #为XATTRS使用object map，EXT4文件系统时使用，XFS或者btrfs也可以使用
        filestore_max_sync_interval=15                  #从日志到数据盘最大同步间隔(seconds)
        filestore_min_sync_interval=10                  #从日志到数据盘最小同步间隔(seconds)
        filestore_queue_max_ops=25000                   #数据盘最大接受的操作数
        filestore_queue_max_bytes=10485760              #数据盘一次操作最大字节数(bytes)	
        filestore_queue_committing_max_ops=5000         #数据盘能够commit的操作数	
        filestore_queue_committing_max_bytes=10485760000 #数据盘能够commit的最大字节数(bytes)	
        filestore_op_threads=32                             #并发文件系统操作数	
        osd_journal_size=20000                          #OSD日志大小(MB)
        journal_max_write_bytes=1073714824              #journal一次性写入的最大字节数(bytes)	
        journal_max_write_entries=10000                 #journal一次性写入的最大记录数
        journal_queue_max_bytes=10485760000             #journal一次性最大在队列中的字节数(bytes)
        osd_min_pg_log_entries = 30000                      #默认值3000 #修建PGLog是保留的最大PGLog数
        osd_max_pg_log_entries = 100000                     #默认值10000 #修建PGLog是保留的最大PGLog数        journal_queue_max_ops=50000
        osd_mon_heartbeat_interval= 40                      #默认值30 #OSD ping一个monitor的时间间隔（默认30s）
        ms_dispatch_throttle_bytes = 1048576000             #默认值 104857600 #等待派遣的最大消息数
        objecter_inflight_ops = 819200                      #默认值1024 #客户端流控，允许的最大未发送io请求数，超过阀值会堵塞应用io，为0表示不受限
        
        osd_max_write_size=512                              #默认值90 #OSD一次可写入的最大值(MB)
        osd_client_message_size_cap=2147483648              #默认值100 #客户端允许在内存中的最大数据(bytes)
        osd_deep_scrub_stride=131072                        #默认值524288 #在Deep Scrub时候允许读取的字节数(bytes)
        osd_op_threads=8                                    #默认值2#推荐16和8并发文件系统操作数
        osd_op_log_threshold = 50                           #默认值5 #一次显示多少操作的log
        osd_crush_chooseleaf_type = 0                       #默认值为1 #CRUSH规则用到chooseleaf时的bucket的类型
        osd_disk_threads=4                                  #默认值1#OSD密集型操作例如恢复和Scrubbing时的线程
        osd_map_cache_size=1024                             #默认值500 #保留OSD Map的缓存(MB)
        osd_map_cache_bl_size=128                           #默认值50 #OSD进程在内存中的OSD Map缓存(MB)
        osd_mount_options_xfs="rw,noexec,nodev,noatime,nodiratime,nobarrier"    #默认值rw,noatime,inode64  #Ceph OSD xfs Mount选项
        osd_recovery_op_priority=4                          #默认值10#推荐值2和4#恢复操作优先级，取值1-63，值越高占用资源越高
        osd_recovery_max_active=10                          #默认值15##同一时间内活跃的恢复请求数   控制在单个OSD上，最多可以有多少个PG同时recover
        osd_max_backfills=4                                 #默认值10 #一个OSD允许的最大backfills数
        osd_crush_update_on_start = false                   #如果初始化的时候，把该值设置为true，然后重启所有osd。不然创建完pool会提示：100.000% pgs unknown100.000% pgs unknown。所有osd都加入，集群ok后，再统一把该值设置为false, 然后重启所有osd     注意：只要新创建了pool， 所有osd 设置：osd_crush_update_on_start = true 才会进行调整
        osd_scrub_begin_hour = 2                            #scrub开始时间
        osd_scrub_end_hour = 6                              #scrub结束时间

        osd_recovery_max_chunk = 1048576                    #默认值8388608. 设置恢复数据块的大小，以防网络阻塞
        osd_recovery_sleep=0                               #默认值0. revocery的间隔
        osd_recovery_max_single_start = 1                   #默认值5. 这个值限定了每个pg可以启动recovery操作的最大数。

[client]
        keyring = /etc/pve/priv/$cluster.$name.keyring
        rbd_cache=true                                      #默认值true      #RBD缓存
        rbd_cache_size=268435456                            #默认值33554432  #RBD缓存大小(bytes)
        rbd_cache_max_dirty=134217728                       #默认值25165824 #缓存为write-back时允许的最大dirty字节数(bytes)，如果为0，使用write-through
        rbd_cache_max_dirty_age=5                           #默认值1#优化参数30和5#在被刷新到存储盘前dirty数据存在缓存的时间(seconds)
        rbd_cache_writethrough_until_flush = false          #默认值true  #该选项是为了兼容linux-2.6.32之前的virtio驱动，避免因为不发送flush请求，数据不回写      #设置该参数后，librbd会以writethrough的方式执行io，直到收到第一个flush请求，才切换为writeback方式。
        rbd_cache_max_dirty_object = 2 #默认值0              #最大的Object对象数，默认为0，表示通过rbd cache size计算得到，librbd默认以4MB为单位对磁盘Image进行逻辑切分     #每个chunk对象抽象为一个Object；librbd中以Object为单位来管理缓存，增大该值可以提升性能
        rbd_cache_target_dirty = 16777216                     #默认值16777216 #开始执行回写过程的脏数据大小，不能超过 rbd_cache_max_dirty
```



# 推荐的基准测试工具：

    fio -ioengine=rbd。运行以下命令：

    fio -ioengine=rbd -direct=1 -name=test -bs=4M -iodepth=16 -rw=write -pool=rpool_hdd -runtime=60 -rbdname=testimg

    fio -ioengine=rbd -direct=1 -name=test -bs=4k -iodepth=1 -rw=randwrite -pool=rpool_hdd -runtime=60 -rbdname=testimg

    fio -ioengine=rbd -direct=1 -name=test -bs=4k -iodepth=128 -rw=randwrite -pool=rpool_hdd -runtime=60 -rbdname=testimg

    然后重复执行rw=read/randread。

    这是为了进行如下测试：

    可能的最佳延迟

    线性带宽

    随机访问iops

    从空的RBD镜像读取非常快，因此在测试之前预先填满rbd镜像磁盘。











    "mon_osd_backfillfull_ratio": "0.900000",
    "mon_osd_full_ratio": "0.950000",
    "mon_osd_nearfull_ratio": "0.850000",
    "osd_failsafe_full_ratio": "0.970000",
    "osd_pool_default_cache_target_full_ratio": "0.800000",

    full_ratio 0.95
    backfillfull_ratio 0.9
    nearfull_ratio 0.85