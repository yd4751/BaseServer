#ifndef __H_CCONNECTION_H__
#define __H_CCONNECTION_H__
#include "global_include.h"
#include "CNodeServerMgr.h"

enum ConnectStatus
{
	CONNEECT_STATUS_NULL = 0,
	CONNEECT_AUTH_TIMEOUT,
	CONNEECT_AUTH,
};
class CTransmitNode
{
	int32_t		m_id;

public:
	CTransmitNode()
	{
		m_id = -1;
		ip = "";
		port = 0;
	}
	CTransmitNode(const std::string ip, int32_t port);
	~CTransmitNode();

public:
	uint32_t	port;
	std::string ip;

public:
	int32_t GetID() { return m_id; }
	void Reconnect();
	void Disconnect();
	void Send(std::shared_ptr<CMessage> msg);

};
class CConnection
{
	int				m_id;
	ConnectStatus	m_status;

	CNodeServerMgr 	m_transmitNodes;

public:
	CConnection(int fd,int nAuthTime = 0);
	virtual ~CConnection();

public:
	int GetID() { return m_id; }
	bool OnAuthTimeOut();
	void Auth() { if(CONNEECT_STATUS_NULL == m_status) m_status = CONNEECT_AUTH;};
	bool IsAuth() { return m_status == CONNEECT_STATUS_NULL ? false : true; }
	ConnectStatus GetStatus() { return m_status; }

public:
	ServerType GetTranmitType(int nCmd);
	void TransmitAdd(ServerType type,std::string ip, int32_t port);
	void TransmitDisconnect(int fd);
	void TransmitSend(std::shared_ptr<CMessage> msg);
};

#endif