#!/bin/bash

# 输出iptables信息
# 作者 麦文迪

# 定义输出文件
output_file="iptables_log.txt"

# 获取当前时间的Unix时间戳
current_time=$(date +%s)

# 获取iptables信息
iptables_info=$(iptables -L -v -n)

# 将信息追加到文件
echo "------ + $current_time + iptables info" >> "$output_file"
echo "$iptables_info" >> "$output_file"
echo "" >> "$output_file"  # 添加空行以便区分每次记录

# 提示用户
echo "iptables信息已追加到文件: $output_file"