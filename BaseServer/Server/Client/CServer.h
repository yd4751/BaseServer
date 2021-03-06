#ifndef __H_CSERVER_H__
#define __H_CSERVER_H__
#include "CSingleton.h"
#include "CServerBase.h"
#include "CConfig.h"

class CServer: public CServerBase, public CSingleton<CServer>
{
	bool			m_bRunning;
	CConfig			m_config;

public:
	uint32_t		m_nUserID;

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
	ReturnType OnAuthSucess(int, std::shared_ptr<CMessage>);
	ReturnType OnLoginSucess(int, std::shared_ptr<CMessage>);
	ReturnType OnGuestLoginSucess(int, std::shared_ptr<CMessage>);

public:
	void SendAuth();
	void SendLogin();
	void SendGuestLogin();
	void SendGameLogin(int id);

	//��Ϸ���
public:
	void SendRoomLogin();
public:
	ReturnType OnGameLogin(int, std::shared_ptr<CMessage>);
	ReturnType OnRoomLogin(int, std::shared_ptr<CMessage>);
};

#endif