# 服务

## 系统服务

### 路径

    /usr/lib/systemd/system/

### 安装命令

    systemctl enable mboxd.service  开机自启
    systemctl start mboxd.service   立即启动

### 卸载命令

    systemctl daemon-reload             重新加载
    systemctl stop mboxd.service        立即停止
    systemctl disable mboxd.service     停止开机自启

```ini
; mboxd.service 名称
[Unit]
Description=mboxd service
After=network.target
Wants=network.target

[Service]
Type=simple
User=root
Group=root
WorkingDirectory=/opt/apps/mboxd/bin/
ExecStart=/opt/apps/mboxd/bin/mboxd system service --time=0 unix:///tmp/mboxd.sock
Restart=always
TimeoutStopSec=2s


[Install]
WantedBy=graphical.target
Alias=mboxd.service

```

## 用户服务

### 路径

    /etc/systemd/user/

### 安装命令

    USER=`users | cut -d ' ' -f 1`
    sudo -u $USER1 XDG_RUNTIME_DIR=/run/user/$(id -u $USER1) systemctl --user enable UniMBoxService.service     开机自启
    sudo -u $USER1 XDG_RUNTIME_DIR=/run/user/$(id -u $USER1) systemctl --user start UniMBoxService.service      立即启动

### 卸载命令

    systemctl daemon-reload             重新加载
    systemctl stop mboxd.service        立即停止
    systemctl disable mboxd.service     停止开机自启

### 例子

```ini
; xxxx.service 名称
[Unit]
Description=UniMBoxService for linux
After=default.target

[Service]
Type=simple
ExecStart=/opt/apps/mbox-app/bin/UniMBoxDesktop.sh
Restart=on-failure
RestartSec=5s
NotifyAccess=all

[Install]
WantedBy=default.target
```