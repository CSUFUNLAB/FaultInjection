import os
import json
import csv

# 把程序放到数据的根目录（data），根目录名称在main()里面更改。直接运行就可以

def parse_multiple_json(file_content):
    """
    解析 JSON 文件中的多个 JSON 对象，返回 JSON 对象列表。
    """
    decoder = json.JSONDecoder()
    pos = 0
    objs = []
    while pos < len(file_content):
        try:
            obj, idx = decoder.raw_decode(file_content, pos)
            if isinstance(obj, dict):  # 确保解析出的对象是字典
                objs.append(obj)
            else:
                print(f"警告：解析到非字典对象 {obj}，跳过")
            pos = idx  # 更新解析位置
        except json.JSONDecodeError:
            pos = file_content.find("{", pos + 1)  # 跳到下一个可能的 JSON 开始位置
            if pos == -1:  # 若找不到 "{"，跳出循环
                break
    return objs

def determine_file_type(start_info):
    """
    根据start部分判断文件类型：
    - 存在 'accepted_connection' 认为是 server
    - 存在 'connected' (且无 'accepted_connection') 认为是 client
    - 否则返回error并打印日志
    """
    if not isinstance(start_info, dict):
        print("警告：start格式错误，无法判断文件类型")
        return "error"
    
    if "accepted_connection" in start_info:
        return "server"
    elif "connected" in start_info:
        return "client"
    return "error"

def process_json_file(json_filepath):
    """
    处理单个 JSON 文件，提取所有数据流的信息，并返回数据列表。
    """
    with open(json_filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    json_objs = parse_multiple_json(content)
    rows = []

    for obj in json_objs:
        if not isinstance(obj, dict):
            print(f"警告：文件 {json_filepath} 包含无效 JSON 对象 {obj}，跳过")
            continue

        try:
            start_info = obj.get("start", {})
            file_type = determine_file_type(start_info)
            
            timestamp = start_info.get("timestamp", {})
            timesecs = timestamp.get("timesecs", 0)
            test_start = start_info.get("test_start", {})
            protocol = test_start.get("protocol", "")
            blksize = test_start.get("blksize", 0)

            intervals = obj.get("intervals", [])
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
    
    return rows

def generate_csv(rows, output_filepath):
    """
    生成 CSV 文件，确保所有出现的字段都有值，不存在的字段填0占位。
    """
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
    """
    root_folder的名称在下面main里面更改
    遍历 root_folder 及所有子目录，处理每个目录中所有的json文件
    并在每个目录中生成对应的 CSV 文件，与同名json文件在同一目录。
    只处理包含 .json 文件的目录
    """
    try: #try-except捕获OSError，防止无法访问导致的程序终止
        for dirpath, _, filenames in os.walk(root_folder):
            json_files = [f for f in filenames if f.lower().endswith(".json")]
            if not json_files:
                continue  # 如果当前目录没有 JSON 文件，跳过
            
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
    root_folder = "../data"  # 设定数据存储的根目录
    if not os.path.exists(root_folder):
        print(f"错误：目录 {root_folder} 不存在！")
        return
    process_all_json_in_folder(root_folder)

if __name__ == "__main__":
    main()
