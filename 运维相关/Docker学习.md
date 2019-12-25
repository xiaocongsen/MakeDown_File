
# Docker 使用记录
## Ubuntu安装
``` Shell
        sudo apt-get update -y  ##更新
        sudo apt-get install -y apt-transport-https ca-certificates curl  software-properties-common    ##安装 tools
        curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -    ##添加apt-key
        sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu artful stable"   ##安装 docker-ce
        sudo apt-get update -y
        sudo apt-get install -y docker-ce   ##安装 docker-ce
```
## Mac安装
``` shell
    brew cask install docker
```
- [mac软件安装链接](https://download.docker.com/mac/stable/Docker.dmg)
- [Mac可视化工具安装链接](https://download.docker.com/kitematic/Kitematic-Mac.zip)

## CentOS安装
``` shell
    yum -y install docker-io
```
## 镜像加速
    鉴于国内网络问题，后续拉取 Docker 镜像十分缓慢，我们可以需要配置加速器来解决，我使用的是网易的镜像地址：http://hub-mirror.c.163.com。
    路径 /etc/docker/daemon.json（Linux）
    %programdata%\docker\config\daemon.json（Windows）
    加入:
    {"registry-mirrors": ["http://hub-mirror.c.163.com"]}

## Docker使用命令记录
``` shell
service docker start    启动docker
docker run -i -t ubuntu /bin/bash 运行一个ubuntu系统的容器并执行/bin/bash
docker run -d -p 5000:5000 training/webapp python app.py -p指定映射端口
docker stats --help		stats命令使用帮助
docker ps -a			查看所有的容器
docker logs			查看容器内的标准输出
docker start 			启动容器
docker stop			停止容器
docker rm 			删除不需要的容器
docker pull ubuntu	 	载入镜像
docker top （NAMES)	查看容器内部运行的进程
docker port (NAMES | ID)	查看容器端口的映射情况
docker images 		列出本地主机上的镜像。
docker search 		寻找适合我们的镜像
docker exec -it (NAMES | ID) COMMAND	进入某个容器
docker attach  (NAMES | ID) 	进入某个容器
docker build -t typeecho:0.1 .  通过Dockerfile文件生成一个容器环境
docker inspect --format '{{ .NetworkSettings.IPAddress }}' <container-ID>   通过ID获取容器IP地址
docker cp /root/www 96f7f14e99ab:/www/   将本机中的文件拷备到容器中去

docker commit -a "typeecho" -m "my" a404c6c174a2  typeecho:v1  将容器保存为新镜像

docker load < xxx.tar.gz        加载某个镜像
```
## Docker实践
        想使用Docker搭建一个博客网站，在ubuntu搭建好后移植到Mac下，来熟悉Docker的使用！
### 实践中遇到的问题
1. 在Docker的ubuntu环境下安装mysql 安装无法成功
``` shell
apt-get install mysql-server mysql-client libmysqlclient-dev
```
安装完成图片

运行图片


2. 通过typecho自带Dockerfile文件生成容器
``` shell
docker build -t typeecho:0.1 .
```
- 容器没有分配IP地址


        解决方法：
            忘了记录网站 几条命领

- typecho的Dockerfile没有执行拷贝博客网页文件到容器内
        
        解决方法：
        docker cp /root/www 96f7f14e99ab:/www/   将本机中的文件拷备到容器中去

- 已经启动的容器无法重新映射端口

        解决方法：
        百度好久无发实现， 只能打包后通过命令运行打包后到容器并映射好端口启动
        docker commit -a "typeecho" -m "my" a404c6c174a2  typeecho:v1
        docker run -d -p 80:80 -v /server/docker-apps/typecho:/www typeecho:v1
        打开网站报 503错误

- 转战mac本地客户端 通过客户端下载的容器不带mysql 不管怎么设置链接都有问题

已卒



docker run -d --name=typecho-blog --restart always --mount type=tmpfs,destination=/tmp -v /srv/http/typecho:/data -e PHP_TZ=Asia/Shanghai -e PHP_MAX_EXECUTION_TIME=600 -p 10086:80 80x86/typecho:amd64
docker run -itd --name mysql-blog -p 3306:3306 -e MYSQL_ROOT_PASSWORD=XIAO2321ml mysql

docker run -d --name=typecho-blog --network testnet --network-alias typecho-blog --restart always --mount type=tmpfs,destination=/tmp -v /srv/http/typecho:/data -e PHP_TZ=Asia/Shanghai -e PHP_MAX_EXECUTION_TIME=600 -p 10086:80 80x86/typecho:amd64
docker run -itd --name mysql-blog --network testnet --network-alias mysql-blog -p 3306:3306 -e MYSQL_ROOT_PASSWORD=XIAO2321ml mysql:5.7.28



docker run --name my-nginx -p 8080:80 -v /data/www:/er/www -v /data/conf/conf.d:/etc/nginx/conf.d -d nginx

docker run --name my-php -d -v /Users/xiaocs/Desktop/www:/var/www/html:ro php:fpm                                  //ro 表示只读
docker run --name my-nginx -p 80:80 -d -v /Users/xiaocs/Desktop/www:/usr/share/nginx/html:ro -v /Users/xiaocs/Desktop/conf/:/etc/nginx/conf.d:ro --link my-php:php nginx
docker run -p 3306:3306 --name my-mysql -v /Users/xiaocs/Desktop/conf/mysql:/etc/mysql -v /Users/xiaocs/Desktop/logs:/logs -v /Users/xiaocs/Desktop/data:/mysql_data -e MYSQL_ROOT_PASSWORD=123456 -d mysql
docker run -p 3306:3306 --name my-mysql -v /Users/xiaocs/Desktop/logs:/logs -e MYSQL_ROOT_PASSWORD=123456 -d --link my-php mysql




docker run --name my-php -p 9000:9000 -v /Users/xiaocs/Desktop/www:/var/www/html:ro -v /Users/xiaocs/Desktop/conf/php:/usr/local/etc/php --link my-mysql:mysql -d php:fpm
docker run --name my-nginx -p 80:80 -d -v /Users/xiaocs/Desktop/www:/usr/share/nginx/html:ro -v /Users/xiaocs/Desktop/conf/:/etc/nginx/conf.d:ro --link my-php:php nginx
