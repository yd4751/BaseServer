#include "JsonParse.h"
#include "global_include.h"
using namespace MessageDefine;

namespace ProtoParseJson
{
	//���и�ʽ�� �ͻ��������(�����ѹ��) - ����˻ظ���(�����ѹ��)

	//##### AccountLogin  -  �˻���¼
	//Request
	template <>
	RequestAccountLogin Parse(std::shared_ptr<CMessage> msg)
	{
		Json::Reader reader;
		Json::Value value;

		if (!reader.parse(msg->GetDataBuf(), (msg->GetDataBuf() + msg->nMsgLength), value))
		{
			CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
			return RequestAccountLogin();
		}

		return RequestAccountLogin(value["account"].asString(), value["password"].asString());
	};
	template<>
	std::string MakePacket(RequestAccountLogin req)
	{
		Json::Value value;
		value["account"] = req.account;
		value["password"] = req.password;

		return value.toStyledString();
	};
	
}