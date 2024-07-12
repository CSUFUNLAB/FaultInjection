const fs = require('fs');
const http = require('http');

let readPosition = 0; // 记录读取位置
function handleWrite(data) {
    const writerStream = fs.createWriteStream('faultData.csv', { flags: 'a' });
    // 写入数据
    writerStream.write(`${data.self_node},${data.pair_node},${data.is_client},${data.port},${data.trans_type},${data.sec},${data.err},${data.band},${data.transfer},${data.rtry},${data.rtt},${data.lost},${data.flag}\n`);
    // 标记文件末尾
    writerStream.end();
}
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

                fs.readFile('faultData.csv', 'utf8', (err, fileContent) => {
                    if (err) {
                        // 文件不存在或读取错误，直接处理
                        // 创建一个可以写入的流，追加方式写入
                        handleWrite(data);
                        response.end("SUCCESS");
                    } else {
                        const lines  = fileContent.split('\n');
                        const secondLastLine = lines[lines.length - 2];
                        if (secondLastLine) {
                            const lastNumber = secondLastLine.split(',').pop();
                            if (data.flag === 1 && lastNumber === '0') {
                                handleWrite(data);
                                response.end("SUCCESS");
                                // 清空文件后写入
                                // fs.writeFile('faultData.csv', '', (clearErr) => {
                                //     if (clearErr) {
                                //         console.error('清空文件出错:', clearErr);
                                //     } else {
                                //         handleWrite(data);
                                //         response.end("SUCCESS");
                                //     }
                                // });
                            } else {
                                handleWrite(data);
                                response.end("SUCCESS");
                            }
                        } else {
                            handleWrite(data);
                            response.end("SUCCESS");
                        }
                    }
                });
            } catch (err) {
                // 如果解析出现错误，返回错误信息
                response.statusCode = 400;
                response.end(err);
            }
        });
    } else if (request.url === '/api/send_data_stream') {
    //     接收后端发送的数据流数据
        let body = [];
        request.on('data', chunk => {
            body.push(chunk)
        });
        request.on('end', () => {
            try {
                // 将 JSON 数据解析为对象
                const data = JSON.parse(body);
                console.log(data);
                const laterTime = new Date(new Date().getTime() + data.sendTime * 1000);
                data.laterTime = laterTime;
                data.sendTime = laterTime.getTime();
                const writerStream = fs.createWriteStream('dataStream.csv', { flags: 'a' });
                // 写入数据
                writerStream.write(`${data.nodeSrc},${data.nodeDst},${data.type},${data.bandWidth},${data.sendTime},${data.laterTime}\n`);
                // 标记文件末尾
                writerStream.end();
                response.statusCode = 200;
                response.end("SUCCESS");
            } catch (err) {
                // 如果解析出现错误，返回错误信息
                response.statusCode = 400;
                response.end(JSON.stringify(err));
            }
        });
    } else if (request.url === '/front/get_data_stream') {
        try {
            if (request.method === 'GET') {
                let fileData = '';
                const readStream = fs.createReadStream('dataStream.csv', {start: 0, encoding:'utf8'});
                readStream.on('data', chunk => {
                    fileData += chunk
                });
                readStream.on('end', () => {
                    // 将文件数据按行分割，并解析每行
                    const lines = fileData.split('\n');
                    const currentTime = Date.now();
                    let updatedData = [];
                    let newData = '';

                    // 过滤过期数据，并构建新的数据字符串
                    lines.forEach(line => {
                        if (line) {
                            const parts = line.split(',');
                            console.log('ll:', parts.length);
                            if (parts.length === 6) {
                                const sendTime = parseInt(parts[4]);
                                if (sendTime >= currentTime) {
                                    updatedData.push(line);
                                    newData += line + '\n';
                                }
                            }
                        }
                    });
                    // 将更新后的数据写回文件
                    const writeStream = fs.createWriteStream('dataStream.csv', { flags: 'w' });
                    writeStream.write(newData);
                    writeStream.end();

                    // 返回更新后的数据给前端
                    response.statusCode = 200;
                    response.end(JSON.stringify({ data: updatedData, position: writeStream.bytesWritten }));
                });
            }
        } catch (err) {
            // 如果解析出现错误，返回错误信息
            response.statusCode = 400;
            response.end(JSON.stringify(err));
        }
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

