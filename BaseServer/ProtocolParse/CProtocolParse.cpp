#include "CProtocolParse.h"
#include "global_include.h"

#include "JsonParse.h"
#include "ProtoBufParse.h"
/*
namespace ProtoParse
{
	template<typename T>
	T Parse(std::shared_ptr<CMessage> msg)
	{
		ProtocolType type = static_cast<ProtocolType>(msg->GetProtoType());
		switch (type)
		{
			case ProtocolType::PROTO_TYPE_JSON:
			{
				return ProtoParseJson::Parse<T>(msg);
			}break;
			case ProtocolType::PROTO_TYPE_PB:
			{
				return ProtoParsePB::Parse<T>(msg);
			}break;
			default:
			break;
		}

		CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
		return T();
	};

	template<typename T>
	std::string MakePacket(ProtocolType type, T)
	{
		switch (type)
		{
		case ProtocolType::PROTO_TYPE_JSON:
		{
			return ProtoParseJson::MakePacket(T);
		}break;
		case ProtocolType::PROTO_TYPE_PB:
		{
			return ProtoParsePB::MakePacket(T);
		}break;
		default:
			break;
		}

		CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
		return std::string();
	};
};

*/