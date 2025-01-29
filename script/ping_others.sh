#!/bin/bash

sleep 5

# 主机ip
{
    ping 192.168.3.4 -w 3
} &

# 其他 board ip
for i in {1..10}
do
{
    ping -c 1 "192.168.13.$i"
} &
done

wait
