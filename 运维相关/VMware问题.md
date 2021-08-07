# VMware 获取该虚拟机的所有权失败
    方法： 到虚拟机目录下删除后缀为lck的文件(lock)，即可！
# vm虚拟机无法显示共享目录
    sudo vmhgfs-fuse .host:/ /mnt/hgfs -o allow_other