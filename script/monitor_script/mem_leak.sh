#!/bin/bash
MEMORY_STEP=200  # 每次增加的内存量（MB）
MEMORY_TIMES=10  # 增加次数
INTERVAL=2       # 时间间隔（秒）
KEEP_TIME=30
PIDS=()          # 存储子进程 ID

# 消耗内存的函数
burn_memory() {
    available_memory=$(free -m | grep Mem | awk '{print $7}')
    if [[ $available_memory -lt 100 ]]; then  # 如果剩余内存小于100MB，停止分配内存
        echo "剩余内存不足，停止分配内存，脚本退出"
        return
    fi
    stress-ng --vm 1 --vm-bytes ${MEMORY_STEP}M --vm-keep &
    echo "已分配 $(( (i) * MEMORY_STEP )) MB 内存"
    # 打印当前的可用内存
    echo "当前可用内存: ${available_memory} MB"
    sleep $INTERVAL
}

# 参数解析（新增 -i）
OPTIND=1
while getopts "m:i:t:s:" opt; do
    case $opt in
        m) MEMORY_STEP=$OPTARG ;;    # 设置每次消耗内存量
        s) MEMORY_TIMES=$OPTARG ;;
        i) INTERVAL=$OPTARG ;;       # 设置间隔时间
        t) KEEP_TIME=$OPTARG ;;
        *)
           echo "用法: $0 [-m 每次消耗的内存量(MB)] [-s 消耗次数] [-i 间隔时间(秒)] [-t 持续时间]"
           ;;
    esac
done

echo "开始逐步消耗系统资源……"

# 执行资源消耗逻辑
if [[ $MEMORY_STEP -gt 0 ]]; then
    for ((i = 1; i <= MEMORY_TIMES; i++)); do
        burn_memory
        sleep $INTERVAL
    done
fi

sleep $KEEP_TIME

echo "清理操作：释放资源..."
# 1) 先强制杀掉你自己启动的所有 burn_cpu/burn_memory 子进程
# for pid in "${PIDS[@]}"; do
#     kill -9 "$pid" 2>/dev/null
#     wait "$pid" 2>/dev/null
# done

# 2) 再把可能残留的 stress-ng 实例也干掉
pkill stress-ng 2>/dev/null
wait

# 3) 清理文件系统缓存
sync && echo 3 | sudo tee /proc/sys/vm/drop_caches

# 4) （可选）删除 mem_stress_* 临时文件
rm -f mem_stress_*

echo "所有资源已释放，脚本退出。"

# echo "用法: $0 [-c 最大CPU线程数] [-m 内存消耗量(MB)] [-i 间隔(秒)]"
# bash mycrash.sh -i 3 -c 8    每3s增加一个线程，最多有8个线程

