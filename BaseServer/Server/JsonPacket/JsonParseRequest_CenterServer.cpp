#include "JsonParse.h"
#include "global_include.h"
using namespace MessageDefine;
using namespace NS_Center;

namespace ProtoParseJson
{
	//排列格式： 客户端请求包(解包、压包) - 服务端回复包(解包、压包)

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

	//Register
	template <>
	ReqRegister Parse(std::shared_ptr<CMessage> msg)
	{
		Json::Reader reader;
		Json::Value value;
		ReqRegister info;

		if (!reader.parse(msg->GetDataBuf(), (msg->GetDataBuf() + msg->nMsgLength), value))
		{
			CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
			return info;
		}

		info.ip = value["ip"].asString();
		info.port = value["port"].asInt();
		info.type = value["type"].asInt();
		return info;
	};
	template<>
	std::string MakePacket(ReqRegister req)
	{
		Json::Value value;
		value["ip"] = req.ip;
		value["port"] = req.port;
		value["type"] = req.type;

		return value.toStyledString();
	};
	//OnlineList
	template <>
	ReqServerList Parse(std::shared_ptr<CMessage> msg)
	{
		Json::Reader reader;
		Json::Value value;
		ReqServerList info;

		return info;
	};
	template <>
	std::string MakePacket(ReqServerList msg)
	{
		Json::Value value;

		return value.toStyledString();
	}
}