#!/bin/bash
CPU_LOAD=0         # 启动的 CPU 线程数（默认值）
MEMORY_STEP=0    # 每次增加的内存量（MB）
INTERVAL=5       # 时间间隔（秒）
KEEP_TIME=0
PIDS=()            # 存储子进程 ID

# 消耗 CPU 的函数
burn_cpu() {
    while :; do :; done
}

# 消耗内存的函数
burn_memory() {
    index=0
    while :; do
        stress-ng --vm 1 --vm-bytes ${MEMORY_STEP}M --vm-keep &
        echo "已分配 $(( (index + 1) * MEMORY_STEP )) MB 内存"
        index=$((index + 1))
        sleep $INTERVAL
    done
}

# 参数解析（新增 -i）
OPTIND=1
while getopts "c:m:i:t:" opt; do
    case $opt in
        c) CPU_LOAD=$OPTARG ;;       # 设置 CPU 线程数
        m) MEMORY_STEP=$OPTARG ;;    # 设置每次消耗内存量
        i) INTERVAL=$OPTARG ;;       # 设置间隔时间
	t) KEEP_TIME=$OPTARG ;;
        *)
           echo "用法: $0 [-c 最大CPU线程数] [-m 每次消耗的内存量(MB)] [-i 间隔时间(秒)]"
	   ;;
    esac
done

echo $CPU_LOAD
echo $INTERVAL

# 参数合法性检查
if [[ $CPU_LOAD -le 0 && $MEMORY_STEP -le 0 ]]; then
    echo "至少需要指定一个参数：CPU 线程数或内存消耗量"
    echo "用法: $0 [-c 最大CPU线程数] [-m 内存消耗量(MB)] [-i 间隔(秒)]"
fi

echo "开始逐步消耗系统资源……"

# 执行资源消耗逻辑
if [[ $CPU_LOAD -gt 0 && $MEMORY_STEP -eq 0 ]]; then
    for ((i = 1; i <= CPU_LOAD; i++)); do
        burn_cpu &
        PIDS+=($!)
        echo "已启动 $i 个 CPU 线程"
        sleep $INTERVAL
    done
fi

if [[ $MEMORY_STEP -gt 0 && $CPU_LOAD -eq 0 ]]; then
    burn_memory &
    PIDS+=($!)
fi

if [[ $CPU_LOAD -gt 0 && $MEMORY_STEP -gt 0 ]]; then
    for ((i = 1; i <= CPU_LOAD; i++)); do
        burn_cpu &
        PIDS+=($!)
        echo "已启动 $i 个 CPU 线程"
        sleep $INTERVAL
    done

    burn_memory &
    PIDS+=($!)
fi

sleep $KEEP_TIME

for pid in "${PIDS[@]}"; do
	kill -9 "$pid" 2>/dev/null
done

rm -f mem_stress_*

echo "释放系统缓存..."
sync && echo 3 | sudo tee /proc/sys/vm/drop_caches

echo "所有资源已释放，脚本退出。"

# echo "用法: $0 [-c 最大CPU线程数] [-m 内存消耗量(MB)] [-i 间隔(秒)]"
# bash mycrash.sh -i 3 -c 8    每3s增加一个线程，最多有8个线程
