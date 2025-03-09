#!/bin/bash

# 扫描某个网段是否有连接
# 作者 曹再杰

# 初始化变量
ADDRESS_RANGE=""
INTERVAL=10  # 默认时间间隔为 10 秒

# 解析命令行参数
while getopts "p:t:" opt; do
  case $opt in
    p) ADDRESS_RANGE=$OPTARG ;;  # -p 参数: 地址范围
    t) INTERVAL=$OPTARG ;;       # -t 参数: 时间间隔
    \?) echo "Usage: $0 -p <address-range> -t <interval>"
        exit 1 ;;
  esac
done

# 检查是否提供了地址范围
if [ -z "$ADDRESS_RANGE" ]; then
    echo "Usage: $0 -p <address-range> -t <interval>"
    exit 1
fi

# 检查时间间隔是否为有效的数字
if ! [[ "$INTERVAL" =~ ^[0-9]+$ ]]; then
    echo "Error: Interval must be a positive integer."
    exit 1
fi

# 清空文件内容
> nmap_results.txt

# 循环执行 nmap 扫描并保存结果
while true; do
    # 执行 nmap 扫描并追加到 nmap_results.txt
    echo "Running nmap scan on $ADDRESS_RANGE at $(date)" >> nmap_results.txt
    nmap -T4 -sS $ADDRESS_RANGE >> nmap_results.txt
    
    # 等待指定的时间间隔
    echo "Waiting for $INTERVAL seconds before the next scan..." >> nmap_results.txt
    sleep $INTERVAL
done
