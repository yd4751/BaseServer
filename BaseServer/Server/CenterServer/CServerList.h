#ifndef __H_CSERVERLIST_H__
#define __H_CSERVERLIST_H__
#include "global_include.h"

class CServerInfo
{
public:
	CServerInfo() {
		ip = "";
		port = 0;
		type = ServerType::SERVER_INVALID;
		cmdStart = 0;
		cmdEnd = 0;
		fd = -1;
	}
public:
	bool IsSend()
	{
		return type == ServerType::SERVER_GATE;
	}
public:
	ServerType		type;
	std::string		ip;
	std::uint32_t	port;
	std::uint32_t   cmdStart;
	std::uint32_t   cmdEnd;
	int				fd;
};
class CServerList:public CNodeManager<int, CServerInfo>
{
public:
	CServerList();
	~CServerList();
};

#endif