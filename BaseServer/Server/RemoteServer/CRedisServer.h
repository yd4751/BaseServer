#ifndef __H_CREDIS_SERVER_H__
#define __H_CREDIS_SERVER_H__

#include "CRemoteServer.h"
#include "CRedis.h"

class CRedisServer :
	public CRemoteServer
{
	CRedis							m_Redis;

public:
	CRedisServer(CServerConfig, RemoteServerType, std::weak_ptr<CLoginHall>);
	virtual ~CRedisServer();

public:
	virtual void Reconnect();
	virtual void Start();

public:
	virtual bool Send(uint32_t nCmd);
	virtual bool Send(uint32_t nCmd, std::string str);
	virtual void SendCall(uint32_t nLength);

	//消息处理
public:
	ReturnType OnLogin(int nClientID, std::shared_ptr<CMessage>);
	ReturnType OnLogout(int nClientID, std::shared_ptr<CMessage>);
};


#endif