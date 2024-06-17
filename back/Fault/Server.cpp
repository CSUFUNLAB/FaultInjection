#include "server.h"

void Server::ServerStart()
{
	try
	{
		// 定义本地地址 
		// const string host = domain + ":" + to_string(port);
		utility::string_t address = U("http://127.0.0.1:8082");

		// 创建URI构建器
		uri_builder uri(address);

		// 获取URI并将其转换为字符串
		auto addr = uri.to_uri().to_string();
		// ucout << addr << endl;

		// 创建命令处理程序，并将URI地址传递
		CommandHandle handler(addr);
		// 打开命令处理程序，并等待完成
		handler.open().wait();
		ucout << utility::string_t(U("listening for requests at:")) << addr << endl;
		ucout << U("Press enter key to quit....") << endl;

		string line;
		getline(cin, line);
		// 回车键关闭处理程序，并等待完成
		handler.close().wait();
	}
	catch (const std::exception& e)
	{
		cout << "Exception caught: " << e.what() << endl;
	}

}
