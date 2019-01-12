#ifndef __H_DATA_SERVER_H__
#define __H_DATA_SERVER_H__

#include "CRemoteServer.h"
class CDataServer :
	public CRemoteServer
{
public:
	CDataServer(CServerConfig, RemoteServerType type, std::weak_ptr<CLoginHall> pGame);
	virtual ~CDataServer();

public:
	virtual void Start() {};

	//消息处理
public:
	void SendLogin() {};
	ReturnType OnLogin(int nClientID, int nCmd, int nMsgLength, char* msgBuf) { return ReturnType::Return_true; };

	void SendLogout() {};
	ReturnType OnLoginout(int nClientID, int nCmd, int nMsgLength, char* msgBuf) { return ReturnType::Return_true; };
};

#endif