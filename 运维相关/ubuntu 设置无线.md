# 查看无线网卡的信息
    
    iwconfig

# 启动无线网卡项

    sudo ip link set wlan0 up

# 扫描所检测到的无线网络
    
    sudo iw dev wlan0 scan | less

# WPA或者WPA2协议，则需要使用一个叫做wpasupplicant的工具

    sudo apt install wpasupplicant

# 生成无线配置

    wpa_passphrase ESSID PWD > xxx.conf

# 后台启动
    
    sudo wpa_supplicant -B -i wlan0 -Dwext -c ./xxx.conf

# 查看连接状态
    iwconfig wlan0

# 获取ip地址
    dhclient wlan0