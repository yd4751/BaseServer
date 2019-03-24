#ifndef __H_JSON_PARSE_H__
#define __H_JSON_PARSE_H__
#include <memory>
#include <string>

class CMessage;

namespace ProtoParseJson
{
	template<typename T>
	T Parse(std::shared_ptr<CMessage>);

	template<typename T>
	std::string MakePacket(T);
}
#endif