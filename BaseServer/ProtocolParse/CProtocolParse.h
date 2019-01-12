#ifndef __H_PROTOCOL_PARSE_H__
#define __H_PROTOCOL_PARSE_H__
#include "global_include.h"
#include "JsonParse.h"
#include "ProtoBufParse.h"

/*
namespace ProtoParse
{
	//
	template<typename T>
	T Parse(std::shared_ptr<CMessage>);

	template<typename T>
	std::string MakePacket(ProtocolType, T);
};
*/
//还可以使用实例化声明，来做实现分离
namespace ProtoParse
{
	template<typename T>
	T Parse(std::shared_ptr<CMessage> msg)
	{
		NetCore::ProtocolType type = NetCore::ProtocolType::PROTO_TYPE_JSON;//static_cast<ProtocolType>(msg->GetProtoType());
		switch (type)
		{
		case NetCore::ProtocolType::PROTO_TYPE_JSON:
		{
			//return typename ProtoParseJson::Parse< T>(msg);
			return ProtoParseJson::Parse< T>(msg);
		}break;
		case NetCore::ProtocolType::PROTO_TYPE_PB:
		{
			//return ProtoParsePB::Parse<typename T>(msg);
		}break;
		default:
			break;
		}

		CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
		
		return T();
	};

	template<typename T>
	std::string MakePacket(NetCore::ProtocolType type, T input)
	{
		switch (type)
		{
		case NetCore::ProtocolType::PROTO_TYPE_JSON:
		{
			return ProtoParseJson::MakePacket(input);
		}break;
		case NetCore::ProtocolType::PROTO_TYPE_PB:
		{
			//return ProtoParsePB::MakePacket(input);
		}break;
		default:
			break;
		}

		CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
		return std::string();
	};
};
#endif