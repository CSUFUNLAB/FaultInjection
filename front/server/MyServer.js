const fs = require('fs');
const http = require('http');

// 创建服务器
http.createServer(function (request, response) {
    // 根据请求路径处理不同的接口
    if (request.url === '/api/fault_result') {
        // 获取请求体中的 JSON 数据
        let body = [];
        request.on('data', chunk => {
            body.push(chunk)
        });
        request.on('end', () => {
            try {
                // 将 JSON 数据解析为对象
                const data = JSON.parse(body);
                response.statusCode = 200;

                console.log(data)
                // 处理写入文件的逻辑
                // 创建一个可以写入的流，写入到文件 newJs.txt 中
                const writerStream = fs.createWriteStream('faultData.json');
                // 使用 utf8 编码写入数据
                writerStream.write(JSON.stringify(data),'UTF8');
                // 标记文件末尾
                writerStream.end();
                response.end("SUCCESS");
            } catch (err) {
                // 如果解析出现错误，返回错误信息
                response.statusCode = 400;
                response.end(err);
            }
        });
    } else {
        // 发送默认的响应
        response.end('接口未定义。\n');
    }
}).listen(8888);

// 终端打印如下信息
console.log('Server running at http://127.0.0.1:8888/');