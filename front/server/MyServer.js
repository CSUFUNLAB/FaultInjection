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
        // 后端传递“故障显示界面”的数据接口，传递数据形式为String
        // 获取请求体中的String数据
        let body = '';
        request.on('data', chunk => {
            body += chunk.toString();
        });
        request.on('end', () => {
            try {
                console.log(body);
                // 以追加的方式将数据存入文件中，文件不存在会自动创建
                const writerStream = fs.createWriteStream('faultData.csv', { flags: 'a' });
                // 写入数据
                writerStream.write( body)
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
    } else if (request.url === '/api/send_data_stream') {
        // 后端传递“数据流注入”界面输出表单数据接口，传输数据形式为JSON数据，
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
                console.log(err);
                response.statusCode = 400;
                response.end(JSON.stringify(err));
            }
        });
    } else if (request.url === '/front/get_data_stream') {
        try {
            if (request.method === 'GET') {
                // 检查文件是否存在，如果不存在则创建一个空的dataStream.csv文件
                fs.access('dataStream.csv', fs.constants.F_OK, (err) => {
                    if (err) {
                        // 文件不存在，创建一个空的dataStream.csv文件
                        fs.writeFile('dataStream.csv', '', (err) => {
                            if (err) {
                                response.statusCode = 500;
                                response.end(JSON.stringify({ error: '无法创建文件' }));
                                return;
                            }
                            processData();
                        });
                    } else {
                        // 文件存在，直接处理数据
                        processData();
                    }
                });

                function processData() {
                    let fileData = '';
                    const readStream = fs.createReadStream('dataStream.csv', { start: 0, encoding: 'utf8' });
                    readStream.on('data', chunk => {
                        fileData += chunk;
                    });
                    readStream.on('end', () => {
                        // 将文件数据按行分割，并解析每行
                        const lines = fileData.split('\\n');
                        const currentTime = Date.now();
                        let updatedData = [];
                        let newData = '';

                        // 过滤过期数据，并构建新的数据字符串
                        lines.forEach(line => {
                            if (line) {
                                const parts = line.split(',');
                                if (parts.length === 6) {
                                    const sendTime = parseInt(parts[4]);
                                    if (sendTime >= currentTime) {
                                        updatedData.push(line);
                                        newData += line + '\\n';
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
            }
        } catch (err) {
            // 如果解析出现错误，返回错误信息
            response.statusCode = 400;
            response.end(JSON.stringify(err));
        }
    } else if (request.url === '/front/get_fault_result') {
        try {
            if (request.method === 'GET') {
                // 检查文件是否存在
                fs.access('faultData.csv', fs.constants.F_OK, (err) => {
                    if (err) {
                        // 文件不存在，创建一个空的 faultData.csv 文件
                        fs.writeFile('faultData.csv', '', (err) => {
                            if (err) {
                                // 如果创建文件失败，返回错误信息
                                response.statusCode = 500;
                                response.end(JSON.stringify({ error: '无法创建 faultData.csv 文件' }));
                            } else {
                                // 文件创建成功，继续读取文件
                                readFileAndRespond();
                            }
                        });
                    } else {
                        // 文件存在，继续读取文件
                        readFileAndRespond();
                    }
                });
                function readFileAndRespond() {
                    let fileData = '';
                    const readStream = fs.createReadStream('faultData.csv', { start: 0, encoding: 'utf8' });
                    readStream.on('data', chunk => {
                        fileData += chunk;
                    });
                    readStream.on('end', () => {
                        const readPosition = readStream.bytesRead;
                        response.statusCode = 200;
                        response.end(JSON.stringify({ data: fileData, position: readPosition }));
                    });
                }
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

