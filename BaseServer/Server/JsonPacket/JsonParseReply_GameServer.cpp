#include "JsonParse.h"
#include "global_include.h"
using namespace MessageDefine;
using namespace NS_Game;

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
			return ReqRegister();
		}
		ReqRegister req;
		return req;
	};
	template <>
	std::string MakePacket(ReqRegister msg)
	{
		Json::Value value;
		return value.toStyledString();
	}
	*/
	template <>
	ReplyLogin Parse(std::shared_ptr<CMessage> msg)
	{
		Json::Reader reader;
		Json::Value value;
		if (!reader.parse(msg->GetDataBuf(), (msg->GetDataBuf() + msg->nMsgLength), value))
		{
			CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
			return ReplyLogin();
		}

		ReplyLogin reply;
		reply.id = value["id"].asInt();
		reply.sex = value["sex"].asInt();
		reply.money = value["money"].asInt();
		reply.nickName = value["nickName"].asString();
		reply.avatar = value["avatar"].asString();
		return reply;
	};
	template <>
	std::string MakePacket(ReplyLogin msg)
	{
		Json::Value value;
		value["id"] = msg.id;
		value["sex"] = msg.sex;
		value["money"] = msg.money;
		value["nickName"] = msg.nickName;
		value["avatar"] = msg.avatar;
		return value.toStyledString();
	}
	
}