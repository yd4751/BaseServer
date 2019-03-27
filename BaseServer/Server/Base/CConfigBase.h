#ifndef __H_CCONFIGBASE_H__
#define __H_CCONFIGBASE_H__
#include <string>
#include <cstdint>

struct ServerConfig
{
	std::string ip = "";
	uint32_t port = 0;
};
class CConfigBase
{
public:
	CConfigBase()
	{
	}
public:
	//
	std::string		bindIP;
	std::uint32_t	bindPort;
	//
	std::string		registerIP;
	std::uint32_t	registerPort;
	
public:
	virtual bool Init() = 0;
};
#endif