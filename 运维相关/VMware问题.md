# 问题

## VMware 获取该虚拟机的所有权失败

    方法： 到虚拟机目录下删除后缀为lck的文件(lock)，即可！

## vm虚拟机无法显示共享目录

    sudo vmhgfs-fuse .host:/ /mnt/hgfs -o allow_other

## Ubuntu 20.04无法连接网络(网络图标丢失)的解决方案

    sudo service network-manager stop
    sudo rm /var/lib/NetworkManager/NetworkManager.state
    sudo service network-manager start

## Linux虚拟机和Windows物理机之间无法复制粘贴

    sudo apt-get autoremove open-vm-tools   #卸载已有的工具
    sudo apt-get install open-vm-tools      #安装open-vm-tools
    sudo apt-get install open-vm-tools-desktop  #安装open-vm-tools-desktop

