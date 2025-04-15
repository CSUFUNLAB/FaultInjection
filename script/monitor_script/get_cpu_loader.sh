#!/bin/bash

# 从 top 输出中提取 %Cpu(s) 行
cpu_line=$(top -bn1 | grep "%Cpu(s)")

# 提取 idle 值（第8列，通常是 id）
idle=$(echo $cpu_line | awk '{print $8}')

# 计算 100 - idle
usage=$(echo "100 - $idle" | bc)

echo "cpu_loader[$usage]"
