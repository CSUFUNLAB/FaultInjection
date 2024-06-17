const http = require('http');

// 创建服务器
http.createServer(function (request, response) {
    // if (request.url === "/api/sse" && request.method === "GET") {
    //     response.writeHead(200, {
    //         'Content-Type': 'text/event-stream',
    //         'Connection': 'keep-alive',
    //         'Cache-Control': 'no-cache'
    //     });
    //     // 模拟发送数据
    //     let count = 0;
    //     const interval = setInterval(() => {
    //         const eventData = `data: 这是第 ${count++} 条消息\n\n`;
    //         response.write(eventData);
    //
    //         // 假设在发送10条消息后关闭连接
    //         if (count > 10) {
    //             clearInterval(interval);
    //             response.end();
    //         }
    //     }, 1000); // 每秒发送一次
    // }
    // response.writeHead(200, { 'Content-Type': 'text/plain; charset=UTF-8' });
    //
    // // 根据请求路径处理不同的接口
    // if (request.url === '/api/fault_result') {
    //     // 获取请求体中的 JSON 数据
    //     let body = '';
    //     request.on('data', chunk => {
    //         body += chunk;
    //     });
    //     request.on('end', () => {
    //         try {
    //             // 将 JSON 数据解析为对象
    //             const data = JSON.parse(body);
    //             localStorage.setItem('faultResult', JSON.stringify(data));
    //             console.log(data); // 在控制台打印获取到的 data 对象
    //             response.statusCode = 200;
    //             // 在此处处理获取到的数
    //
    //             response.end("SUCCESS");
    //         } catch (err) {
    //
    //             // 如果解析出现错误，返回错误信息
    //             response.statusCode = 400;
    //             response.end(err);
    //         }
    //     });
    // } else {
    //     // 发送默认的响应
    //     response.end('接口未定义。\n');
    // }
}).listen(8888);

// 终端打印如下信息
console.log('Server running at http://127.0.0.1:8888/');