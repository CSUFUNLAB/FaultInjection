#include "ServerHandler.h"
#include "Log.h"
#include "SshSession.h"
#include "DataFlow.h"
#include "NodeManager.h"
#include "FaultInjectInterface.h"
#include "DataInjectInterface.h"

#include <chrono>
#include <codecvt>
#include <boost/filesystem.hpp>
#include <locale>
#include <string>
#include <thread>

#include <comutil.h>
#pragma comment(lib, "comsuppw.lib")

CommandHandle::CommandHandle(string_t url) : myListener(url)
{
	// ��Ӷ�POST�����֧��
	myListener.support(methods::POST, std::bind(&CommandHandle::HandlePost, this, std::placeholders::_1));
	myListener.support(methods::GET, std::bind(&CommandHandle::HandleGet, this, std::placeholders::_1));
}

void CommandHandle::HandlePost(http_request message)
{
	// ���������URI
	utility::string_t recvURI = http::uri::decode(message.relative_uri().path());
	string uri = utility::conversions::to_utf8string(recvURI);
	ucout << "Method:" << message.method() << endl;
	ucout << "URI:" << http::uri::decode(message.relative_uri().path()) << endl;
	LOG_INFO("conversion URI: {}", uri);

	FrontInterface *fault_interface = FrontInterface::fault_interface_factory(uri);
	if (fault_interface == nullptr) {
		LOG_INFO("this should not happend, check fault_interface_factory code");
		return;
	}
	message.reply(fault_interface->handle(message));
	delete fault_interface;
}

void CommandHandle::HandleGet(http_request message)
{
	/*
	* SSEʵʱ���䣬��bug����û��ͨ
	ucout << "Method:" << message.method() << endl;
	ucout << "URI:" << http::uri::decode(message.relative_uri().path()) << endl;
	// ���������URI
	utility::string_t recvURI = http::uri::decode(message.relative_uri().path());

	if (recvURI == U("/api/get_fault_data")) {
		// �趨http_response��ʵ��SSE
		http_response resp(status_codes::OK);

		resp.headers().add(U("Content-Type"), U("text/event-stream"));
		resp.headers().add(U("Cache-Control"), U("no-cache"));
		resp.headers().add(U("Connection"), U("keep-alive"));

		int count = 0;

		// ʹ�ö�ʱ��ÿ��ִ��һ�κ���
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
				std::string sse_frame = "data: ���ǵ� " + std::to_string(count++) + " ����Ϣ\n\n";
				resp.set_body(sse_frame);
				// ������Ӧ
				message.reply(resp);
				// �����ڷ��� 10 ����Ϣ��ر�����
				if (count > 10) {
					break;
				}
				start = current;
			}
		}
	}
	else
	{
		ucout << "�ӿ�δ����" << endl;
	}
	*/
}

