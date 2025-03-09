#!/bin/bash

# 监控cpu mem 流量
# 作者 彭富

# 未完成，需要在一个脚本内实现这些功能

iftop -t -s 10 -i eth0 | grep "Total" > iftop_total_output.txt
top -n 1 >> top_output.txt
