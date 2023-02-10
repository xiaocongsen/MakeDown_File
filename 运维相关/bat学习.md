bat学习

``` bat
rem 脚本功能替换文件第二行内容
rem 关闭回显
echo off
rem 开启变量延迟 直接输出会导致丢失感叹号
setlocal enabledelayedexpansion
set "f=./modify_version.py"
(for /f "tokens=1* delims=:" %%a in ('findstr /n .* "%f%"') do (
    set /a x+=1
    if !x! equ 2 (
        echo newVersion="%var%"
    ) else (
        rem 关闭变量延迟 直接输出可以保留全部字符
        Setlocal disableDelayedExpansion
        echo;%%b
        endlocal
    )
))>"tmp"&move "tmp" "./modify_version.py"
endlocal

rem 脚本功能删除第二行内容
(For /F "tokens=1* delims=:" %%a In ('Findstr /n .* modify_version.py') Do If 2 NEQ %%a ECHO(%%b)>$
Move $ modify_version.py
```








``` bat
#/bin/bash
var=$(ls -l /var/lib/ceph/osd/ceph-*/block | awk -F "/var/lib/ceph/osd/ceph-" '{print $2}' | awk -F "/block -> /dev/ceph-" '{print $1}' |wc -l)
echo var

begin=$(lsblk | sed -n '/ceph/=' | awk 'NR==1')
let begin-=1
end=$(lsblk | sed -n '/ceph/=' | awk 'END{print}')
index=0
if  [ ! -n "$1" ] ;then
        echo "not ceph rdb name"
        exit 1
fi

for i in $(seq $end -1 $begin)
do
    num=$(ls -l /var/lib/ceph/osd/ceph-*/block | awk -F "/var/lib/ceph/osd/ceph-" '{print $2}' | awk -F "/block -> /dev/ceph-" '{print $1}' | awk 'NR=='$i'')
    id=$(ls -l /var/lib/ceph/osd/ceph-*/block | awk -F "/var/lib/ceph/osd/ceph-" '{print $2}' | awk -F "/block -> /dev/ceph-" '{print $2}' | awk 'NR=='$i'' | sed 's/-//g')
    echo $num == $id
    lsblk | grep ceph | sed 's/-//g' | awk -F "└─" '{print $2}' | awk '{print $1}'
done


for i in $(seq $var -1 1)
do
    num=$(ls -l /var/lib/ceph/osd/ceph-*/block | awk -F "/var/lib/ceph/osd/ceph-" '{print $2}' | awk -F "/block -> /dev/ceph-" '{print $1}' | awk 'NR=='$i'')
    if [num == ]
    id=$(ls -l /var/lib/ceph/osd/ceph-*/block | awk -F "/var/lib/ceph/osd/ceph-" '{print $2}' | awk -F "/block -> /dev/ceph-" '{print $2}' | awk 'NR=='$i'' | sed 's/-//g')
    echo $num == $id
    lsblk | grep ceph | sed 's/-//g' | awk -F "└─" '{print $2}' | awk '{print $1}'
done
```