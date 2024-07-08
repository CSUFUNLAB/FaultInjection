const fs = require('fs');
const http = require('http');

let readPosition = 0; // 记录读取位置
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
                // 创建一个可以写入的流，追加方式写入
                const writerStream = fs.createWriteStream('faultData.csv', {flags: 'a'});
                // 写入数据
                writerStream.write(`${data.self_node},${data.pair_node},${data.is_client},${data.port},${data.trans_type},${data.sec},${data.transfer},${data.err},${data.rtry},${data.rtt},${data.lost}\n`);
                // 标记文件末尾
                writerStream.end();
                response.end("SUCCESS");
            } catch (err) {
                // 如果解析出现错误，返回错误信息
                response.statusCode = 400;
                response.end(err);
            }
        });
    } else if (request.url === '/front/get_fault_result') {
        try {
            if (request.method === 'GET') {
                let fileData = '';
                const readStream = fs.createReadStream('faultData.csv', {start: 0, encoding:'utf8'});
                readStream.on('data', chunk => {
                    fileData += chunk
                });
                readStream.on('end', () => {
                    readPosition = readStream.bytesRead;
                    response.statusCode = 200;
                    response.end(JSON.stringify({data: fileData, position: readPosition}));
                });
            }
        } catch (err) {
            // 如果解析出现错误，返回错误信息
            response.statusCode = 400;
            response.end(JSON.stringify(err));
        }
    } else {
        // 发送默认的响应
        response.end('接口未定义。\n');
    }
}).listen(8888);

// 终端打印如下信息
console.log('Server running at http://127.0.0.1:8888/');

