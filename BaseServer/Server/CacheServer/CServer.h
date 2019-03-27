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
};

#endif