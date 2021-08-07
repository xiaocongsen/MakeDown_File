# proxmox VE安装
## 下载镜像
    https://pve.proxmox.com/wiki/Downloads
写入u盘或者使用vm安装

## 修改proxmoxVE IP地址
    /etc/network/interfaces
    /etc/issue
    /etc/hosts

## 修改源地址
    方法一：
    vim /etc/apt/sources.list.d/pve-enterprise.list
    修改为deb http://download.proxmox.wiki/debian/pve buster pve-no-subscription
    vim /etc/apt/sources.list
    修改为：deb http://mirrors.aliyun.com/debian buster main contrib
    deb http://mirrors.aliyun.com/debian buster-updates main contrib
    deb http://mirrors.aliyun.com/debian-security/ buster/updates main contrib
    方法二：
    echo "#deb https://enterprise.proxmox.com/debian/pve buster pve-enterprise" > /etc/apt/sources.list.d/pve-enterprise.list
    wget -q -O- 'http://download.proxmox.com/debian/proxmox-ve-release-5.x.gpg' | apt-key add -
    wget -q -O- 'http://download.proxmox.com/debian/proxmox-ve-release-6.x.gpg' | apt-key add -
    echo "deb http://download.proxmox.com/debian/pve buster pve-no-subscription" > /etc/apt/sources.list.d/pve-no-subscription.list
    echo "deb https://mirrors.ustc.edu.cn/proxmox/debian/pve buster pve-no-subscription" > /etc/apt/sources.list.d/pve-no-subscription.list
    sed -i.bak "s#http://download.proxmox.com/debian#https://mirrors.ustc.edu.cn/proxmox/debian#g" /usr/share/perl5/PVE/CLI/pveceph.pm
    sed -i.bak "s#ftp.debian.org/debian#mirrors.163.com/debian#g" /etc/apt/sources.list
    sed "s#security.debian.org#mirrors.163.com/debian-security#g" /etc/apt/sources.list

    2. 
    apt update
    apt upgrade -y

## 去除订阅提示
    sed -i "s/data.status !== 'Active'/false/g" /usr/share/javascript/proxmox-widget-toolkit/proxmoxlib.js
    init 6 成功