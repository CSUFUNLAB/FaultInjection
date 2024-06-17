#include "server.h"

void Server::ServerStart()
{
	try
	{
		// ���屾�ص�ַ 
		// const string host = domain + ":" + to_string(port);
		utility::string_t address = U("http://127.0.0.1:8082");

		// ����URI������
		uri_builder uri(address);

		// ��ȡURI������ת��Ϊ�ַ���
		auto addr = uri.to_uri().to_string();
		// ucout << addr << endl;

		// �����������򣬲���URI��ַ����
		CommandHandle handler(addr);
		// ���������򣬲��ȴ����
		handler.open().wait();
		ucout << utility::string_t(U("listening for requests at:")) << addr << endl;
		ucout << U("Press enter key to quit....") << endl;

		string line;
		getline(cin, line);
		// �س����رմ�����򣬲��ȴ����
		handler.close().wait();
	}
	catch (const std::exception& e)
	{
		cout << "Exception caught: " << e.what() << endl;
	}

}
