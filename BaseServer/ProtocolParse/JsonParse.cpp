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
	//##### AccountLogin  -  �˻��ǳ�
	//Reply
	template <>
	ReplyAccountLogout Parse(std::shared_ptr<CMessage> msg)
	{
		Json::Reader reader;
		Json::Value value;
		ReplyAccountLogout reply;

		if (!reader.parse(msg->GetDataBuf(), (msg->GetDataBuf() + msg->nMsgLength), value))
		{
			CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
			return reply;
		}

		reply.code = static_cast<ReplyErrorCode>(value["code"].asInt());
		reply.account = value["account"].asString();
		return reply;
	};
	template<>
	std::string MakePacket(ReplyAccountLogout reply)
	{
		Json::Value value;
		value["code"] = reply.code;
		value["account"] = reply.account;
		return value.toStyledString();
	};
	//##### AccountCreate  -  �˻�����
	//Request
	template <>
	RequestAccountCreate Parse(std::shared_ptr<CMessage> msg)
	{
		Json::Reader reader;
		Json::Value value;
		RequestAccountCreate req;

		if (!reader.parse(msg->GetDataBuf(), (msg->GetDataBuf() + msg->nMsgLength), value))
		{
			CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
			return req;
		}

		req.account = value["account"].asString();
		req.password = value["password"].asString();
		return req;
	};
	template<>
	std::string MakePacket(RequestAccountCreate req)
	{
		Json::Value value;
		value["account"] = req.account;
		value["password"] = req.password;

		return value.toStyledString();
	};
	//Reply
	template <>
	ReplyAccountCreate Parse(std::shared_ptr<CMessage> msg)
	{
		Json::Reader reader;
		Json::Value value;
		ReplyAccountCreate reply;

		if (!reader.parse(msg->GetDataBuf(), (msg->GetDataBuf() + msg->nMsgLength), value))
		{
			CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
			return reply;
		}

		reply.code = static_cast<ReplyErrorCode>(value["code"].asInt());
		reply.account;
		if (reply.code == ReplyErrorCode::REPLY_CODE_SUCCESS)
		{
			reply.id = value["id"].asInt();
		}
		
		return reply;
	};
	template<>
	std::string MakePacket(ReplyAccountCreate reply)
	{
		Json::Value value;
		value["code"] = reply.code;
		value["account"] = reply.account;
		if (reply.code == ReplyErrorCode::REPLY_CODE_SUCCESS)
		{
			value["id"] = reply.id;
		}

		return value.toStyledString();
	};
}