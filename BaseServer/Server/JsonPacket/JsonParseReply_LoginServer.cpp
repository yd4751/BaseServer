#include "JsonParse.h"
#include "global_include.h"
using namespace MessageDefine;
using namespace NS_Login;

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
	//ReplyUserLogin
	template <>
	ReplyUserLogin Parse(std::shared_ptr<CMessage> msg)
	{
		ReplyUserLogin info;
		Json::Reader reader;
		Json::Value value;
		if (!reader.parse(msg->GetDataBuf(), (msg->GetDataBuf() + msg->nMsgLength), value))
		{
			CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
			return info;
		}
		
		info.id = value["id"].asInt();			//用户id
		info.sex = value["sex"].asInt();;		//性别
		info.money = value["money"].asInt();;		//携带
		info.nickName = value["nickName"].asString();;	//昵称
		info.avatar = value["avatar"].asString();;		//头像地址
		return info;
	};
	template <>
	std::string MakePacket(ReplyUserLogin msg)
	{
		Json::Value value;
		
		value["id"] = msg.id;			//用户id
		value["sex"] = msg.sex;		//性别
		value["money"] = msg.money;		//携带
		value["nickName"] = msg.nickName;	//昵称
		value["avatar"] = msg.avatar;		//头像地址
		return value.toStyledString();
	}
	
}