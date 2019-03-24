#ifndef __H_CSERVERBASE_H__
#define __H_CSERVERBASE_H__
#include "global_include.h"
#include "CNodeServerMgr.h"

#define DEFAULT_MESSAGE_PACKET_LEN_MAX			0x40000

class CServerBase:public CMessageHandler
{	
public:
	CServerBase();
public:
	virtual void Init() = 0;
	virtual void Start() = 0;
	virtual void Stop() = 0;

public:
	ServerType								m_type;
	std::shared_ptr<CMessage>				m_curMessage;
	CNodeServerMgr							m_nodeServers;

public:
	virtual ReturnType OnConnect(int, std::shared_ptr<CMessage>) = 0;
	virtual ReturnType OnDisconnect(int, std::shared_ptr<CMessage>) = 0;
	virtual ReturnType OnRegisterSuccess(int, std::shared_ptr<CMessage>);

public:
	virtual void Send(int fd,int cmd, Json::Value& packet);
	virtual void Send(int fd, int cmd, std::string str);
	virtual void Send(int fd, int cmd);
	virtual bool RegisterToServer(ServerType type,const std::string& ip, uint32_t port,uint32_t bindPort,int cmd);
	
};
#endif
