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
	int				fd;
};
class CServerList:public CNodeManager<int, CServerInfo>
{
public:
	CServerList();
	~CServerList();
};

#endif