#include "ServerHandler.h"
#include "Log.h"
#include "SshSession.h"
#include "DataFlow.h"
#include "NodeManager.h"

#include <thread>
#include <chrono>

#include <comutil.h>
#pragma comment(lib, "comsuppw.lib")

#include "ServerHandler.h"
#include <comutil.h>
#include <locale>
#include <string>
#include <codecvt>
#include <boost/filesystem.hpp>
#include <thread>
#include "FaultInsertInterface.h"
#include "DataInsertInterface.h"

#pragma comment(lib, "comsuppw.lib")


CommandHandle::CommandHandle(string_t url) : myListener(url)
{
	// 添加对POST请求的支持
	myListener.support(methods::POST, std::bind(&CommandHandle::HandlePost, this, std::placeholders::_1));
	myListener.support(methods::GET, std::bind(&CommandHandle::HandleGet, this, std::placeholders::_1));
}


void CommandHandle::HandlePost(http_request message)
{
	ucout << "Method:" << message.method() << endl;
	ucout << "URI:" << http::uri::decode(message.relative_uri().path()) << endl;
	// 解码请求的URI
	utility::string_t recvURI = http::uri::decode(message.relative_uri().path());

	if (recvURI == U("/api/fault_insert"))
	{
		// 故障注入接口
		FaultInsertInterface faultInsertInterface;
		HandlerInfo resultInfo = faultInsertInterface.handlerData(message);
		message.reply(faultInsertInterface.HandleResponse(&resultInfo));
	}
	else if (recvURI == U("/api/data_insert")) {
		// 数据注入接口
		DataInsertInterface dataInsertInterface;
		HandlerInfo resultInfo = dataInsertInterface.handlerData(message);
		message.reply(dataInsertInterface.HandleResponse(&resultInfo));

	} 
	//else if (recvURI == U("/api/get_fault_data")) {
	//	// 数据注入后，结果获取接口，采用轮询的方式，前端每10s调用一次，若有数据，code为200，msg为数据内容，code为601，结果传输结束，为其他则报错
	//	Json::Value recvJson1;
	//	recvJson1["bandWidth"] = "8,3,5,12";
	//	recvJson1["retry"] = "6,15,5,12";

	//	DataInsertInterface dataInsertInterface;
	//	HandlerInfo  resultInfo  = dataInsertInterface.faultResult(recvJson1);
	//	message.reply(dataInsertInterface.HandleResponse(&resultInfo));
	//	
	//	/*
	//	HandlerInfo info;
	//	info.code = 200;
	//	info.msg = "11111";
	//	DataInsertInterface dataInsertInterface;
	//	message.reply(dataInsertInterface.HandleResponse(&info));
	//	*/
	//}
	else
	{
		ucout << "接口未定义" << endl;
	}
}

void CommandHandle::HandleGet(http_request message)
{
	/*
	* SSE实时传输，有bug，还没调通
	ucout << "Method:" << message.method() << endl;
	ucout << "URI:" << http::uri::decode(message.relative_uri().path()) << endl;
	// 解码请求的URI
	utility::string_t recvURI = http::uri::decode(message.relative_uri().path());

	if (recvURI == U("/api/get_fault_data")) {
		// 设定http_response以实现SSE
		http_response resp(status_codes::OK);

		resp.headers().add(U("Content-Type"), U("text/event-stream"));
		resp.headers().add(U("Cache-Control"), U("no-cache"));
		resp.headers().add(U("Connection"), U("keep-alive"));

		int count = 0;

		// 使用定时器每秒执行一次函数
		std::time_t start = std::time(nullptr);
		std::time_t current;


		while (true) 
		{
			Json::Value recvJson1;
			recvJson1["bandWidth"] = "8,3,5,12";
			recvJson1["retry"] = "6,15,5,12";

			current = std::time(nullptr);
			if (current - start >= 1) 
			{
				std::string sse_frame = "data: 这是第 " + std::to_string(count++) + " 条消息\n\n";
				resp.set_body(sse_frame);
				// 发送响应
				message.reply(resp);
				// 假设在发送 10 条消息后关闭连接
				if (count > 10) {
					break;
				}
				start = current;
			}
		}
	}
	else
	{
		ucout << "接口未定义" << endl;
	}
	*/

}

