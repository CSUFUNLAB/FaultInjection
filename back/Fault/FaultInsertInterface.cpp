#include "FaultInsertInterface.h"

HandlerInfo FaultInsertInterface::handlerData(http_request message)
{
	json::value recvDatas = json::value::object();
	recvDatas = message.extract_json().get();
	// ��ȡ��message��web::json::value��ʽ
	ucout << "recvDatas:" << recvDatas  << endl; // recvDatas: {"faultParameter":"", "faultType" : "congest", "nodes" : [{"value":"1"}] }
	// ��ȡmessage��Json::Value��ʽ
	Json::Value recvJsonDatas = HandleJsonData(recvDatas);
	/*
	* ����ʾ��
	recvJsonDatas: {
		"faultParameter" : "",// ���ϲ���
		"faultType" : "congest",// ��������
		"nodes" :// ���Ͻڵ�
			[
				{
					"value" : "1"
				}
			]
	}*/
	cout << "recvJsonDatas: " << recvJsonDatas << endl;

	// �Ի�ȡ�Ĺ���ע�����ݽ��в���



	// ��ע��������ǰ��
	HandlerInfo handlerInfo;
	// ע��ɹ���code=200��ע��ʧ��codeΪ������msgΪ������Ϣ
	handlerInfo.code = 200;
	handlerInfo.msg = "ע��ɹ�";
	return handlerInfo;
}
