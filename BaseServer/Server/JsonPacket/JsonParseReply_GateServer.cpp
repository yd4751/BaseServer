#include "JsonParse.h"
#include "global_include.h"
using namespace MessageDefine;
using namespace NS_Center;

namespace ProtoParseJson
{
	//���и�ʽ�� �ͻ��������(�����ѹ��) - ����˻ظ���(�����ѹ��)
	/*
	template <>
	ReqRegister Parse(std::shared_ptr<CMessage> msg)
	{
		Json::Reader reader;
		Json::Value value;
		if (!reader.parse(msg->GetDataBuf(), (msg->GetDataBuf() + msg->nMsgLength), value))
		{
			CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
			return ;
		}
	};
	template <>
	std::string MakePacket(ReqRegister msg)
	{
		Json::Value value;
		return value.toStyledString();
	}
	*/

	
}