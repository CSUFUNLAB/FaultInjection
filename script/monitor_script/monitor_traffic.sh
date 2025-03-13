#!/bin/bash

# 每秒监控 CPU、内存和网络流量，并保存到文件中
# 作者 彭富

OUTPUT_DIR="/home/orangepi/data"

while true; do
    # 获取网络流量
    echo "$(date +%s)" >> $OUTPUT_DIR/iftop_record.txt
    sudo stdbuf -oL iftop -t -s 1 -i wlan0 2>/dev/null | grep "Total" >> $OUTPUT_DIR/iftop_record.txt
    echo "=======================" >> $OUTPUT_DIR/iftop_record.txt

    sleep 30
done


