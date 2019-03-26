#ifndef __H_CSERVER_H__
#define __H_CSERVER_H__
#include "CSingleton.h"
#include "CServerBase.h"
#include "CConfig.h"

class CConnection;
struct ServerInfo
{
	std::string ip;
	uint32_t	port;
	int32_t		cmdStart;
	int32_t		cmdEnd;
	ServerType type;

	ServerInfo()
	{
		ip = "";
		port = 0;
		type = ServerType::SERVER_INVALID;
		cmdStart = 0;
		cmdEnd = 0;
	}
	ServerInfo(ServerType type,std::string ip, uint32_t port,int32_t cmdStart,int32_t cmdEnd)
	{
		this->ip = ip;
		this->port = port;
		this->type = type;
		this->cmdStart = cmdStart;
		this->cmdEnd = cmdEnd;
	}
};
class CServer: public CServerBase, public CSingleton<CServer>
{
	bool			m_bRunning;
	CConfig			m_config;
	CNodeManager<int, std::shared_ptr<CConnection>>		m_connects;
	std::map<ServerType, std::vector<ServerInfo>>		m_serverList;

public:
	CServer();
	virtual ~CServer();

public:
	virtual void Init();
	virtual void Start();
	virtual void Stop();
public:
	static bool OnNetEventHandler(int nClientID, int nCmd, int nMsgLength, char* msgBuf, NetCore::ProtocolType type);
	

public:
	virtual ReturnType OnConnect(int, std::shared_ptr<CMessage>);
	virtual ReturnType OnDisconnect(int, std::shared_ptr<CMessage>);
	bool OnAuthTimeOut();

public:
	//RegisterMessage   int, std::shared_ptr<CMessage>
	//连接认证
	ReturnType OnConnectAuth(int, std::shared_ptr<CMessage>);
	//列表更新通知
	ReturnType OnServerList(int, std::shared_ptr<CMessage>);

protected:
	//获取目标服务器类型
	ServerType GetServerType(int cmd);
	ServerInfo GetTargetServerInfo(ServerType type);
	ReturnType OnTransmitMessage(int fd,int nCmd, std::shared_ptr<CMessage>);
};

#endif