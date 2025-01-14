# mboxd学习

tar -C /home/xcs/xxx -c . | mboxd import - name:tag

mboxd save -o image.tar name:tag              # 创建镜像

mboxd export my_container > my_container.tar    # 导出容器


sudo apt install xserver-xephyr               # 虚拟显示器
Xephyr -screen 800*600 :2                     # 创建虚拟显示器

sudo mboxd create --cap-add=NET_RAW --cgroups disabled --net=xxx --ip=192.168.0.2 -v /tmp/.X11-unix:/tmp/.X11-unix/:ro -v /home/"$LOGNAME"/.config/fcitx/:/home/mboxd/.config/fcitx -e DBUS_SESSION_BUS_ADDRESS="$DBUS_SESSION_BUS_ADDRESS" -v /var/lib/dbus/machine-id:/var/lib/dbus/machine-id:ro -v /run/user/1000/bus:/run/user/1000/bus --init 1b /init.sh         # 创建容器


sudo tar --exclude=/boot/* --exclude=/lib/modules/* --exclude=/dev/* --exclude=/usr/src/* --exclude=/usr/share/man/* --exclude=/usr/local/share/man/* --exclude=/usr/lib/modules/* --exclude=/home/xcs/Desktop/* --exclude=/home/xcs/Documents/* --exclude=/home/xcs/Downloads/* --exclude=/home/xcs/Music/* --exclude=/home/xcs/Pictures/* --exclude=/home/xcs/Videos/* --exclude=/home/xcs/.local/share/* --exclude=/media/* --exclude=/opt/* --exclude=/etc/pam.d_bak --exclude=/etc/fstab_bak --exclude=/run/* --exclude=/mnt/* --exclude=/proc/* --exclude=/srv/* --exclude=/sys/* --exclude=/tmp/* --exclude=/var/lib/samba/private/msg.sock/* -cf /home/xcs/image.tar  /etc /dev /opt /run /usr /var /bin /lib /sbin /boot /home/xcs/Downloads /home/xcs/Documents /home/xcs/Desktop /home/xcs/Music /home/xcs/Pictures /home/xcs/Videos /home/xcs/.config  /home/xcs/.icons /home/xcs/.local /home/xcs/.Templates /home/xcs/.bashrc /home/xcs/.bash_logout /home/xcs/.profile /media /mnt /proc /root /srv /sys /tmp /libx32 /lib64 /lib32 /init.sh


tar -rvf ../image.tar -C . .       将当前目录的文件添加到../image.tar包中

mboxd run -itd --cap-add=NET_RAW --cgroups disabled -v /tmp/.X11-unix:/tmp/.X11-unix/:ro -v /var/lib/dbus/machine-id:/var/lib/dbus/machine-id:ro -v /run/user/1000/bus:/run/user/1000/bus -e DBUS_SESSION_BUS_ADDRESS="$DBUS_SESSION_BUS_ADDRESS" a0 /bin/bash

sudo mboxd exec -it cc /bin/bash

sudo mboxd start --log-level debug 66

sudo systemctl restart sdpdaemonsrv



## libpod学习
    curl --unix-socket /tmp/mboxd.sock -X GET -H "Content-Type: application/json" http://d/v4.0.0/libpod/containers/crazy_payne/exists
    curl --unix-socket /tmp/mboxd.sock -X POST -H "Content-Type: application/json" -d  @postfile.json http://d/v4.0.0/libpod/containers/create


    curl --unix-socket /tmp/mboxd.sock -X POST -H "Content-Type: application/json" -d  "http://d/v4.0.0/libpod/images/import"


```json

{"reference":"uos%3A7f6221a8-6706-466c-bb18-3d9b3270ce25","url":"/opt/apps/com.leagsoft.mboxd/files/imagebase/baseimages.tar"}


{"name":"xxxx","driver":"bridge","network_interface":"cni-podman3","subnets":[{"subnet":"192.166.0.0/24","gateway":"192.166.0.1"},{"subnet":"fe81::/80","gateway":"fe81::1"}],"ipv6_enabled":true,"internal":true,"dns_enabled":false}



// postfile.json
{
 "Name": "my_UOS",
 "Image": "120fae33",
 "COMMAND": [
  "/bin/tini",
  "--",
  "/init.sh"
 ],
 "Env": {
  "DBUS_SESSION_BUS_ADDRESS": "unix:path=/run/user/1000/bus"
 },
 "networkmode": "host",
 "capabilities": [
  "net_raw"
 ],
 "Mounts": [
  {
   "Type": "bind",
   "Source": "/run/user/1000/bus",
   "Destination": "/run/user/1000/bus",
   "Options": [
    "nosuid",
    "nodev",
    "rbind",
    "rw"
   ],
   "Propagation": "rprivate"
  },
  {
   "Type": "bind",
   "Source": "/tmp/.X11-unix",
   "Destination": "/tmp/.X11-unix/",
   "Options": [
    "rbind",
    "ro"
   ],
   "Propagation": "rprivate"
  },
  {
   "Type": "bind",
   "Source": "/home/xcs/.config/fcitx",
   "Destination": "/home/mboxd/.config/fcitx",
   "Options": [
    "rbind",
    "rw"
   ],
   "Propagation": "rprivate"
  },
  {
   "Type": "bind",
   "Source": "/var/lib/dbus/machine-id",
   "Destination": "/var/lib/dbus/machine-id",
   "Options": [
    "rbind",
    "ro"
   ],
   "Propagation": "rprivate"
  }
 ],
 "CgroupMode": "host",
 "CgroupManager": "cgroupfs",
 "CgroupMode": "host"
}
```

curl --unix-socket /tmp/mboxd.sock -X PUT -H "Content-Type: application/json" --data-binary @files.tar  http://d/v4.0.0/libpod/containers/1a/archive?noOverwriteDirNonDir&path=/home/mboxd/

```c++
// host_file必须是tar文件格式
bool mboxd_api::host_file_to_container(const std::string &host_file, const std::string &vm_file,
                                       const std::string &container_id,std::string &err_info)
{
    std::string url_list_image = fmt::format("{}/libpod/containers/1a/archive", http_url_);

    cpr::Url url(url_list_image);
    cpr::UnixSocket socket(address_.c_str());
    std::ifstream file_stream(host_file, std::ios::binary);
    std::string file_contents((std::istreambuf_iterator<char>(file_stream)), std::istreambuf_iterator<char>());
    // 设置请求参数
    cpr::Parameters parameters = {
        {"copyUIDGID", "true"},
        {"noOverwriteDirNonDir", "true"},
        {"path", vm_file}  // 替换为容器中要提取文件的目标路径
    };
    cpr::Response res = cpr::Put(cpr::Url{url}, cpr::Header{{"Content-Type", "application/x-tar"}},cpr::Body{file_contents}, cpr::UnixSocket{socket}, cpr::Parameters{parameters});
//    cpr::Response res = cpr::Put(url, cpr::File{host_file},socket);

    std::cout << res.text << std::endl;
    std::error_code ec;
    bool rc = errors(res.status_code + MBOXD_ERROR_TYPES_IMAGE, ec);
    if (rc) {
        return rc;
    }

    return false;

}
```

## mboxd编译环境搭建

    sudo apt-get install libgpgme-dev
    gpgme.pc文件拷贝到/usr/lib/x86_64-linux-gnu/pkgconfig/
    sudo apt-get install libseccomp-dev

```sh
# gpgme.pc的内容
prefix=/usr
exec_prefix=${prefix}
includedir=${prefix}/include
libdir=${prefix}/lib/x86_64-linux-gnu
host=x86_64-pc-linux-gnu
mtcflags=
mtlibs=-pthread

Name: gpgme
Description: GPG Runtime
Version: 1.35
Cflags: 
Libs: -L${prefix}/lib/x86_64-linux-gnu -lgpgme
URL: https://www.gnupg.org/software/libgpgme/index.html

```



{"import_file_name":["1.txt","2.txt"], "import_con_path":"home/usr/", nocover_file":true}