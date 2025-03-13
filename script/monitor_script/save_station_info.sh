#!/bin/bash

# 输出ap连接的sta信息
# 作者 张子扬

# 示例数据 a=$(iw dev ap0 station dump | grep Station | awk '{print $2}')
# a="9c:b8:b4:5d:f7:6c
# 9c:b8:b4:5d:f7:6e
# e2:5b:a3:55:5f:35"
a=$(iw dev ap0 station dump | grep Station | awk '{print $2}')

#b=$(arp -n)
# b="Address                  HWtype  HWaddress           Flags Mask            Iface
# 192.168.12.186           ether   e2:5b:a3:55:5f:35   C                     ap0
# 192.168.12.38                    (incomplete)                              ap0
# 192.168.12.169           ether   9c:b8:b4:5d:f7:6e   C                     ap0
# 192.168.4.1              ether   00:00:a4:0b:f1:f5   C                     eth0
# 192.168.12.167           ether   9c:b8:b4:5d:f7:6c   C                     ap0"
b=$(arp -n)
# 输出文件
output_file="output.txt"
dates=$(date +%s)

echo "$dates" >> "$output_file"
echo "Address,HWaddress" >> "$output_file"

echo "$a" | while read -r mac; do

    match=$(echo "$b" | awk -v mac="$mac" '$3 == mac {print $1 "," $3}')
    if [ -n "$match" ]; then
        echo "$match" >> "$output_file"
    fi
done

echo "结果已保存到 $output_file"