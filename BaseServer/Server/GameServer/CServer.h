#ifndef __H_CSERVER_H__
#define __H_CSERVER_H__
#include "CSingleton.h"
#include "CServerBase.h"
#include "CConfig.h"
#include "CRedis.h"
#include "CUser.h"

class CServer: public CServerBase, public CSingleton<CServer>
{
	bool			m_bRunning;
	CConfig			m_config;
	//
	CRedis			m_redis;
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

	//reids
public:
	bool RDLogin(std::shared_ptr<CUser>);

	//
public:
	std::shared_ptr<CUser> GetUserByUserID(int uid);
	std::shared_ptr<CUser> GetUserByConnectID(int fd);

	//½Å±¾Ïà¹Ø
public:
	void UpdatePlaying(uint32_t uid);
	void UpdateExited(uint32_t uid);
	void UpdateUserInfo(uint32_t uid,std::string info);
	void SendData(uint32_t uid,uint32_t cmd,std::string message);
};

#endif