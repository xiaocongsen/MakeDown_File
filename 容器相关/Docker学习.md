
# Docker 使用记录

## Ubuntu安装

``` Shell
        sudo apt update -y  ##更新
        sudo apt install -y apt-transport-https ca-certificates curl  software-properties-common    ##安装 tools
        curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -    ##添加apt-key
        sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu artful stable"   ##安装 docker-ce
        sudo apt update -y
        sudo apt install -y docker-ce   ##安装 docker-ce
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

## Docker使用命令记录

``` shell
service docker start    启动docker
docker run -i -t ubuntu /bin/bash 运行一个ubuntu系统的容器并执行/bin/bash
docker run -d -p 5000:5000 training/webapp python app.py -p指定映射端口
docker stats --help		stats命令使用帮助
docker ps -a			查看所有的容器
docker logs			查看容器内的标准输出
docker start 			启动容器
docker restart 			重启容器
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

        想使用Docker搭建一个博客网站，在ubuntu搭建好后移植到Mac下，来熟悉Docker的使用！
### 实践中遇到的问题

1. 在Docker的ubuntu环境下安装mysql 安装无法成功

``` shell
apt install mysql-server mysql-client libmysqlclient-dev
```

        安装完成图片

        运行图片

2. 通过typecho自带Dockerfile文件生成容器

``` shell
docker build -t typeecho:0.1 .
```

- 容器没有分配IP地址

        解决方法：
            忘了记录网站 几条命领

- typecho的Dockerfile没有执行拷贝博客网页文件到容器内

        解决方法：
        docker cp /root/www 96f7f14e99ab:/www/   将本机中的文件拷备到容器中去

- 已经启动的容器无法重新映射端口

        解决方法：
        百度好久无发实现， 只能打包后通过命令运行打包后到容器并映射好端口启动
        docker commit -a "typeecho" -m "my" a404c6c174a2  typeecho:v1
        docker run -d -p 80:80 -v /server/docker-apps/typecho:/www typeecho:v1
        打开网站报 503错误

- 转战mac本地客户端 通过客户端下载的容器不带mysql 不管怎么设置链接都有问题

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


# mysql

        docker run -itd --name my-mysql -p 3306:3306 -e MYSQL_ROOT_PASSWORD=XIAO2321ml mysql
        docker run -itd --name my-mysql -p 3306:3306 -e MYSQL_ROOT_PASSWORD=XIAO2321ml mysql:5.7.28

# 个人shadowsock服务器

        docker run -d --name shadowsocks -p 12345:12345 oddrationale/docker-shadowsocks -s 0.0.0.0 -p 12345 -k welcome -m aes-256-cfb
        -d参数允许 docker 常驻后台运行
        -p来指定要映射的端口，这里端口号统一保持一致即可。例如：12345
        -s服务器 IP 地址，不用动
        -k后面设置你的 VPN 的密码，比如：welcome
        -m指定加密方式


# 个人博客typecho

        docker run -d --name=typecho-blog --network testnet --network-alias typecho-blog --restart always --mount type=tmpfs,destination=/tmp -v /srv/http/typecho:/data -e PHP_TZ=Asia/Shanghai -e PHP_MAX_EXECUTION_TIME=600 -p 10086:80 80x86/typecho:amd64
        docker run -itd --name mysql-blog --network testnet --network-alias mysql-blog -p 3306:3306 -e MYSQL_ROOT_PASSWORD=XIAO2321ml mysql:5.7.28

# 个人博客halo

         docker run --rm -it -d --name halo -p 18090:8090  -v ~/.halo:/root/.halo ruibaby/halo



# 个人云盘seafile

        docker run -d --name seafile -e SEAFILE_SERVER_HOSTNAME=110.43.33.114 -e SEAFILE_ADMIN_EMAIL=xiaocongsen@163.com -e SEAFILE_ADMIN_PASSWORD=XIAO2321 -v /opt/seafile-data:/shared -p 11180:80 seafileltd/seafile:latest

# 个人docker阿里云镜像地址

        sudo vim /etc/docker/daemon.json
        增加：
        {
        "registry-mirrors": ["https://j4e4x18w.mirror.aliyuncs.com"]
        }



# 个人下载器aria2(感觉无用)

        docker create --name=aria2 -e PUID=1026 -e PGID=100 -e TZ=Asia/Shanghai -e SECRET=yourtoken -p 6881:6881 -p 6881:6881/udp -p 6800:6800 -v /path/to/appdata/config:/config -v /path/to/downloads:/downloads --restart unless-stopped superng6/aria2

        docker run -d --name ariang -p 80:80 -p 443:443 -e PUID=1000 -e PGID=1000 -e ENABLE_AUTH=true -e RPC_SECRET=HelloWord -e DOMAIN=192.168.88.135 -e ARIA2_SSL=false -e ARIA2_USER=xiaocs -e ARIA2_PWD=XIAO2321 -e ARIA2_EXTERNAL_PORT=443 -v /aria2/data:/data -v /aria2/app/a.db:/app/filebrowser.db -v /aria2/key:/app/conf/key -v /aria2/conf:/app/conf wahyd4/aria2-ui

        docker run -d --name ariang -p 28088:80 -p 443:443 -e PUID=1000 -e PGID=1000 -e ENABLE_AUTH=true -e RPC_SECRET=HelloWord -e ARIA2_SSL=false -e ARIA2_USER=xiaocs -e ARIA2_PWD=XIAO2321 -e ARIA2_EXTERNAL_PORT=443 -v /aria2/data:/data wahyd4/aria2-ui

        docker create --name=aria2 -v /aria2/data/:/mnt -v /aria2/config:/config -e PGID=0 -e PUID=0 -e TZ=Asia/Shanghai -e SECRET=XIAO2321 -p 6800:6800 -p 8080:8080 --restart always auska/docker-aria2

        docker run -d --name aria2-with-webui -p 6800:6800 -p 6880:80 -p 6888:8080 -v /aria2/data:/data -v /aria2/conf:/conf -e SECRET=XIAO2321 xujinkai/aria2-with-webui

        docker run -d --name aria2-pro -p 6800:6800 -p 6888:6888 -p 6888:6888/udp -e PUID=0 -e PGID=0 -e RPC_SECRET=XIAO2321 -v /aria2/config:/config -v /aria2/data:/downloads p3terx/aria2-pro


# v2ray

``` json
        //服务端配置
        {
                "log" : {
                        "access": "/var/log/v2ray/access.log",
                        "error": "/var/log/v2ray/error.log",
                        "loglevel": "warning"
                },
                "inbounds": [{
                        "port": 18666,
                        "protocol": "vmess",
                        "settings": {
                        "clients": [
                                {
                                        "id": "0d0c643a-178f-dddd-b9f3-04f6f80dfcvf",
                                        "level": 1,             //1个人 0共享
                                        "alterId": 64,          //0~100 必须和客户端一直
                                        "security": "auto"      //加密模式
                                }
                                ]
                        }
                }],
                "outbounds": [{
                "protocol": "freedom",
                "settings": {}
                }]
        }
```

        docker run --restart=always --name=v2ray --net=host -v /etc/v2ray/config.json:/etc/v2ray/config.json -v /var/log/v2ray:/var/log/v2ray -i -t -d v2ray/official:latest


# gitlab

        docker pull gitlab/gitlab-ce
        docker run -d -p 443:443 -p 80:80 -p 222:22 --name gitlab --restart=always -v /home/gitlab/config:/etc/gitlab -v /home/gitlab/logs:/var/log/gitlab -v /home/gitlab/data:/var/opt/gitlab gitlab/gitlab-ce                               //--restart=always docker容器重启时跟随启动


