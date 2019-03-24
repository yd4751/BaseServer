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
	ReplyRegister Parse(std::shared_ptr<CMessage> msg)
	{
		ReplyRegister reply;

		return reply;
	};
	template<>
	std::string MakePacket(ReplyRegister reply)
	{
		Json::Value value;
		return value.toStyledString();
	};
	//OnlineList
	template <>
	ReplyServerList Parse(std::shared_ptr<CMessage> msg)
	{
		Json::Reader reader;
		Json::Value value;
		ReplyServerList info;

		if (!reader.parse(msg->GetDataBuf(), (msg->GetDataBuf() + msg->nMsgLength), value))
		{
			CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
			return info;
		}
		for (unsigned int i = 0; i < value["servers"].size(); i++)
		{
			info.servers.emplace_back();
			info.servers[info.servers.size() - 1].ip = value["servers"][i]["ip"].asString();
			info.servers[info.servers.size() - 1].port = value["servers"][i]["port"].asInt();
			info.servers[info.servers.size() - 1].type = value["servers"][i]["type"].asInt();
		}
		return info;
	};
	template <>
	std::string MakePacket(ReplyServerList msg)
	{
		Json::Value value;
		for (auto& it : msg.servers)
		{
			Json::Value info;
			info["ip"] = it.ip;
			info["port"] = it.port;
			info["type"] = it.type;
			value["servers"].append(info);
		}
		return value.toStyledString();
	}
}