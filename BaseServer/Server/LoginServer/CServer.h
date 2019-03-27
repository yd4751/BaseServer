#ifndef __H_CSERVER_H__
#define __H_CSERVER_H__
#include "CSingleton.h"
#include "CServerBase.h"
#include "CConfig.h"
#include "CRedis.h"
#include "CSQLInterface.h"
#include "CUser.h"

class CServer: public CServerBase, public CSingleton<CServer>
{
	bool			m_bRunning;
	CConfig			m_config;
	//
	CSQLInterface					m_sql;
	CRedis							m_Redis;
	//
	CNodeManager<uint32_t, std::shared_ptr<CUser>>		m_users;

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

public:
	//RegisterMessage
	ReturnType OnLogin(int, std::shared_ptr<CMessage>);
	ReturnType OnGuestLogin(int, std::shared_ptr<CMessage>);

	//DB相关
protected:
	bool DBLogin(std::shared_ptr<CUser>);
	//Redis相关
protected:
	bool RDSaveUser(std::shared_ptr<CUser>);
};

#endif
