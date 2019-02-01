#ifndef __H_DATA_SERVER_H__
#define __H_DATA_SERVER_H__

#include "CRemoteServer.h"
#include "CSQLInterface.h"

class CDataServer :
	public CRemoteServer
{
	CSQLInterface			m_sql;

public:
	CDataServer(CServerConfig, RemoteServerType type, std::weak_ptr<CLoginHall> pGame);
	virtual ~CDataServer();

public:
	virtual void Reconnect();
	virtual void Start();

public:
	virtual bool Send(uint32_t nCmd);
	virtual bool Send(uint32_t nCmd, std::string str);
	virtual void SendCall(uint32_t nLength);


protected:
	void GetUserInfo(int32_t nUserID, std::map<std::string, std::string>& info);
	int32_t CreateUser(int32_t nAccountType, const std::string& strAccount, const std::string& strPassword);

	//消息处理
public:
	ReturnType OnLogin(int nClientID, std::shared_ptr<CMessage>);
	ReturnType OnLogout(int nClientID, std::shared_ptr<CMessage>);

	//账户管理
public:
	ReturnType OnAccountCreate(int nClientID, std::shared_ptr<CMessage>);
	ReturnType OnAccountUpdate(int nClientID, std::shared_ptr<CMessage>);
	ReturnType OnAccountDelete(int nClientID, std::shared_ptr<CMessage>);
};

#endif