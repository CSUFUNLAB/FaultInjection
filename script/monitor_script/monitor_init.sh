#!/bin/bash

rm -rf /home/orangepi/data
rm /home/orangepi/nohup.out
rm /root/nohup.out
mkdir /home/orangepi/data
chmod 777 /home/orangepi/data

ip route add 192.168.12.0/24 via 192.168.12.1

/home/orangepi/monitor_script/monitor_ap_signal.sh &
/home/orangepi/monitor_script/monitor_mem.sh &
/home/orangepi/monitor_script/monitor_traffic.sh &




