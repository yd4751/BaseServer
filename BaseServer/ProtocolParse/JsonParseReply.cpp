#include "JsonParse.h"
#include "global_include.h"
using namespace MessageDefine;

namespace ProtoParseJson
{
	//排列格式： 客户端请求包(解包、压包) - 服务端回复包(解包、压包)

	//Reply
	template <>
	ReplyAccountLogin Parse(std::shared_ptr<CMessage> msg)
	{
		Json::Reader reader;
		Json::Value value;
		ReplyAccountLogin reply;

		if (!reader.parse(msg->GetDataBuf(), (msg->GetDataBuf() + msg->nMsgLength), value))
		{
			CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
			return reply;
		}

		reply.code = static_cast<ReplyErrorCode>(value["code"].asInt());
		reply.account = value["account"].asString();
		if (reply.code == ReplyErrorCode::REPLY_CODE_SUCCESS)
		{
			reply.id = value["id"].asInt();
			reply.nick_name = value["nick_name"].asString();
		}
		return reply;
	};
	template<>
	std::string MakePacket(ReplyAccountLogin reply)
	{
		Json::Value value;
		value["code"] = reply.code;
		value["account"] = reply.account;
		if (reply.code == ReplyErrorCode::REPLY_CODE_SUCCESS)
		{
			value["id"] = reply.id;
			value["nick_name"] = reply.nick_name;
		}

		return value.toStyledString();
	};
}