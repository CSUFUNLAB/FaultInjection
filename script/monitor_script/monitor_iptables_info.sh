#!/bin/bash

# 输出iptables信息
# 作者 麦文迪

# 定义输出文件
output_file="/home/orangepi/data/iptables_log.txt"

while true; do
	# 获取当前时间的Unix时间戳
	echo "$(date +%s)" >> "$output_file"
	# 获取iptables信息
	echo "$(iptables -L -v -n)" >> "$output_file"
	echo "=======================" >> "$output_file"
	sleep 30	
done

