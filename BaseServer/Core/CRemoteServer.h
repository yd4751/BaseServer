#ifndef __H_CREMOTESERVER_H__
#define __H_CREMOTESERVER_H__
#include "global_include.h"
#include "CRpc.h"

class CLoginHall;
class CRpc;


#define DEFAULT_RECONECT_TIMES 10
#define DEFAULT_MAX_MESSAGE_LEN 2048
class CServerConfig
{
public:
	std::string		password;
	std::string		ip;
	uint32_t		port;
	uint32_t		maxReconnectTime;
	CServerConfig()
	{
		port = 0;
		maxReconnectTime = DEFAULT_RECONECT_TIMES;		
	}
	CServerConfig(std::string ip, uint32_t port)
	{
		this->ip = ip;
		this->port = port;
		maxReconnectTime = DEFAULT_RECONECT_TIMES;
	}
	CServerConfig(std::string ip, uint32_t port,std::string password)
	//	CServerConfig(ip, port)  这样写赋值也是对的，但是这个传参原理是什么？
	{
		this->ip = ip;
		this->port = port;
		this->password = password;
	}
};

class CRemoteServer:
	public CMessageHandler,
	public CRpc
{
public:
	
	int										m_ClientID;
	std::weak_ptr<CLoginHall>					m_pGame;
	
	CServerConfig							m_config;
	RemoteServerType						m_emType;
	std::shared_ptr<CMessage>				m_pReplyMessage;

public:
	CRemoteServer(CServerConfig config, RemoteServerType type):
		m_config(config),
		m_emType(type)
	{
		m_pReplyMessage = std::make_shared<CMessage>();
		m_pReplyMessage->data = std::make_shared<CArray<char>>(DEFAULT_MAX_MESSAGE_LEN);
	}
	~CRemoteServer() {}

public:
	virtual void Reconnect();
	virtual void Start();

public:
	RemoteServerType GetType() { return m_emType; }
	void SetType(RemoteServerType type) { m_emType = type; }

public:
	ReturnType Dispatch(int nClientID, std::shared_ptr<CMessage>);
	virtual bool Send(uint32_t nCmd);
	virtual bool Send(uint32_t nCmd,std::string str);
};
#endif