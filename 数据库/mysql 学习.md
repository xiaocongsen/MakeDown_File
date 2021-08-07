# mysql 语句
    desc tabl_name；            --显示表结构

# mysql 


# MySQL 主从配置
## 主服务器配置
1. 修改配置文件 mysql.cnf
[mysqld]
log-bin=mysql-bin #开启二进制日志
server-id=1 #设置server-id，必须唯一
1. 重启 mysql，创建用于同步的用户帐号
create user '用户名'@'从服务器的IP' identified by '密码';
grant replication slave on *.* to '用户名'@'从服务器的IP;
flush privileges;
1. 查看 master 状态，记录文件名和位置
show master status;

## 从服务器配置
1. 修改配置文件 mysql.cnf
server-id=2

1. 重启 mysql，执行同步 sql 语句
change master to
master_host='主服务器IP',
master_user='同步帐号的用户名',
master_password='同步账号的密码',
master_log_file='主服务器文件名',
master_log_pos='主服务器文件位置';
1. 启动 slave 同步进程
start slave;

1. 查看 slave 状态
show slave status\G

当slave_io_running和slave_sql_running为yes时代表同步成功，接下来可以在主服务器操作数据，在从服务器进行验证




# 错误
    [1045 Access denied for user 'root'@'localhost' (using password:YES)]
    刚安装好的mysql 远程登陆报这个错误 5.8以上的版本是限制root用户远程登陆的！
    解决方法创建一个和root一样权限的用户
##  创建新用户：
    use mysql           //选择库
    select host, user from user;    查询用户表
    CREATE USER 'username'@'%' IDENTIFIED BY 'password';    创建新用户
##  赋予权限：
    5.7以上的命令
    GRANT ALL PRIVILEGES ON *.* TO 'username'@'%'   
    或者5.6一下的命令
    GRANT ALL PRIVILEGES ON *.* TO 'username'@'%' IDENTIFIED BY 'password' WITH GRANT OPTION;
##  刷新使其生效
    flush privileges;