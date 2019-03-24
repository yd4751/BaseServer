#ifndef __H_CNODESERVERMGR_H__
#define __H_CNODESERVERMGR_H__
#include "global_include.h"

class CNodeServer
{
	int				m_fd;//socket

public:
	std::string		ip;
	uint32_t		port;
	ServerType		type;

public:
	CNodeServer();
	CNodeServer(ServerType type,std::string ip, uint32_t port);
	~CNodeServer();

public:
	int GetID() { return m_fd; }
	void Reconnect();
	void Disconnect();
	void Send(std::shared_ptr<CMessage>);
	void Send(int cmd,std::string& data);

};
class CNodeServerMgr
{
	CNodeManager<ServerType, std::shared_ptr<CNodeServer>>  m_servers;
public:
	CNodeServerMgr();

public:
	void Add(ServerType type,std::string ip, uint32_t port);
	void Disconnect(int fd);

public:
	void Send(ServerType type, std::shared_ptr<CMessage>);
	void Send(ServerType type, int cmd, std::string data);
};
#endif