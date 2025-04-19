import os
import json
import csv
import re

# 添加处理iperf error

def parse_multiple_json(file_content):
    decoder = json.JSONDecoder()
    pos = 0
    objs = []
    while pos < len(file_content):
        try:
            obj, idx = decoder.raw_decode(file_content, pos)
            if isinstance(obj, dict):
                objs.append(obj)
            else:
                print(f"警告：解析到非字典对象 {obj}，跳过")
            pos = idx
        except json.JSONDecodeError:
            pos = file_content.find("{", pos + 1)
            if pos == -1:
                break
    return objs


def determine_file_type(start_info):
    if not isinstance(start_info, dict):
        print("警告：start格式错误，无法判断文件类型")
        return "error"
    if "accepted_connection" in start_info:
        return "server"
    elif "connected" in start_info:
        return "client"
    return "error"


def determine_file_type_by_name(json_filepath):
    point_pos = json_filepath.rfind('.')
    type_str = json_filepath[point_pos - 1]
    if type_str == '0':
        return "client"
    elif type_str == '1':
        return "server"
    else:
        return "error"


def has_fuzzy_error(content):
    error_patterns = [
        r"iperf3\s*:\s*error\s*-\s*unable to send control message.*bad file descriptor",
        r"iperf3\s*:\s*error\s*-\s*the server has terminated"
    ]
    for pattern in error_patterns:
        if re.search(pattern, content, re.IGNORECASE | re.DOTALL):
            return True
    return False


def process_json_file(json_filepath):
    with open(json_filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    json_objs = parse_multiple_json(content)
    rows = []
    file_type = determine_file_type_by_name(json_filepath)

    for obj in json_objs:
        if not isinstance(obj, dict):
            print(f"警告：文件 {json_filepath} 包含无效 JSON 对象 {obj}，跳过")
            continue

        try:
            start_info = obj.get("start", {})
            timestamp = start_info.get("timestamp", {})
            timesecs = timestamp.get("timesecs", 0)
            test_start = start_info.get("test_start", {})
            protocol = test_start.get("protocol", "")
            blksize = test_start.get("blksize", 0)

            intervals = obj.get("intervals", [])
            if timestamp and not intervals:
                row = {
                    "file_type": file_type,
                    "timesecs": timesecs,
                }
                rows.append(row)
            for interval in intervals:
                streams = interval.get("streams", [])
                for stream in streams:
                    if isinstance(stream, dict):
                        row = {
                            "file_type": file_type,
                            "timesecs": timesecs,
                            "protocol": protocol,
                            "blksize": blksize
                        }
                        row.update(stream)
                        rows.append(row)
        except KeyError as e:
            print(f"错误：文件 {json_filepath} 缺少关键字段 {e}，跳过")

    if has_fuzzy_error(content):
        print(f"检测到模糊错误日志于 {json_filepath}，记录为异常数据")
        rows.append({
            "file_type": file_type,
            "timesecs": -1,
            "protocol": -1,
            "blksize": -1
        })

    return rows


def generate_csv(rows, output_filepath):
    all_keys = set()
    for row in rows:
        all_keys.update(row.keys())

    common_fields = ["file_type", "timesecs", "protocol", "blksize"]
    other_fields = sorted(list(all_keys - set(common_fields)))
    fieldnames = common_fields + other_fields

    with open(output_filepath, "w", newline='', encoding="utf-8") as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for row in rows:
            complete_row = {key: row.get(key, 0) for key in fieldnames}
            writer.writerow(complete_row)


def process_all_json_in_folder(root_folder):
    try:
        for dirpath, _, filenames in os.walk(root_folder):
            json_files = [f for f in filenames if f.lower().endswith(".json")]
            if not json_files:
                continue

            for json_file in json_files:
                json_filepath = os.path.join(dirpath, json_file)
                print(f"处理文件: {json_filepath}")
                rows = process_json_file(json_filepath)
                if not rows:
                    print(f"警告：文件 {json_filepath} 没有可处理的数据。")
                    continue
                output_filepath = os.path.join(dirpath, os.path.splitext(json_file)[0] + ".csv")
                generate_csv(rows, output_filepath)
    except OSError as e:
        print(f"错误：无法遍历目录 {root_folder}，错误信息：{e}")


def main():
    root_folder = r"C:\Users\彭富\Desktop\data"
    if not os.path.exists(root_folder):
        print(f"错误：目录 {root_folder} 不存在！")
        return
    process_all_json_in_folder(root_folder)


if __name__ == "__main__":
    main()
