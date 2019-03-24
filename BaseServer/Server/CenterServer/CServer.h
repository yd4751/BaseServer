#ifndef __H_CSERVER_H__
#define __H_CSERVER_H__
#include "CSingleton.h"
#include "CServerBase.h"
#include "CConfig.h"
#include "ServerDefine.h"
#include "CServerList.h"

class CServer: public CServerBase, public CSingleton<CServer>
{
	bool							m_bRunning;
	CConfig							m_config;
	CServerList						m_serverList;

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
	//推送列表信息
	void SendList();

public:
	//RegisterMessage   int, std::shared_ptr<CMessage>
	//服务器注册
	ReturnType OnRegister(int, std::shared_ptr<CMessage>);
	//服务器数据更新
	ReturnType OnUpdate(int, std::shared_ptr<CMessage>);
	//在线服务器列表获取
	ReturnType OnGetOnlineList(int, std::shared_ptr<CMessage>);
};

#endif