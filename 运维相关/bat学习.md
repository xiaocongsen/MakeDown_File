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