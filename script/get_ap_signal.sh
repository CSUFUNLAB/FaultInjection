#!/bin/bash

# 扫描连接wifi（orangepi）的wifi信号强度
# 目前没有写提取wifi name的功能，直接取第6行
# 需要sudo
# 作者 马孝宇

# 默认值
INTERVAL=5  # 默认间隔 5 秒
TIMES=5     # 默认执行 5 次

# 解析命令行参数
while getopts "i:t:" opt; do
    case $opt in
        i) INTERVAL=$OPTARG ;;
        t) TIMES=$OPTARG ;;
        *) echo "Usage: $0 [-i interval] [-t times]" && exit 1 ;;
    esac
done

# 定义输出文件名
NMCLI_OUTPUT="nmcli_list.txt"
IWLIST_OUTPUT="iwlist_scan.txt"

# 执行指定次数的扫描
for i in $(seq 1 $TIMES); do
#    echo "====================="
#    echo "$(date) 开始第 $i 次扫描 WiFi..."

    # 执行 nmcli 扫描并保存结果
    nmcli dev wifi list | grep "orangepi" >> "$NMCLI_OUTPUT"
#    echo "nmcli 结果已保存到: $(pwd)/$NMCLI_OUTPUT"

    # 执行 iwlist 扫描并提取第 6 行
    sudo iwlist wlan0 scan | sed -n '6p' >> "$IWLIST_OUTPUT"
#    echo "iwlist 结果（第5行）已保存到: $(pwd)/$IWLIST_OUTPUT"

    # 如果不是最后一次，就等待指定间隔
    if [ $i -lt $TIMES ]; then
#        echo "扫描完成，$INTERVAL 秒后进行下一次扫描..."
        sleep $INTERVAL
    fi
done

# echo "所有 $TIMES 次扫描完成！"
