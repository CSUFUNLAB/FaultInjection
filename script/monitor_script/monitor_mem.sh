#!/bin/bash

# 每秒监控 CPU、内存和网络流量，并保存到文件中
# 作者 彭富

OUTPUT_DIR="/home/orangepi/data"

while true; do
    # 获取 CPU 和内存使用情况
    echo "$(date +%s)" >> $OUTPUT_DIR/top_record.txt
    top -b -n 1 | head -n 10 >> $OUTPUT_DIR/top_record.txt
    echo "=======================" >> $OUTPUT_DIR/top_record.txt

    sleep 30
done


