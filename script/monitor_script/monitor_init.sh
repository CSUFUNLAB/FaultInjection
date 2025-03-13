#!/bin/bash

rm -rf /home/orangepi/data
mkdir /home/orangepi/data
chmod 777 /home/orangepi/data

/home/orangepi/monitor_script/monitor_ap_signal.sh &
/home/orangepi/monitor_script/monitor_mem.sh &
/home/orangepi/monitor_script/monitor_traffic.sh &




