#include "CConnection.h"
#include "CTimerTask.h"
#include "CServer.h"
CTransmitNode::CTransmitNode(const std::string ip, int32_t port)
{
	this->ip = ip;
	this->port = port;
	m_id = NetCore::Connect(ip.c_str(), port);
};
CTransmitNode::~CTransmitNode()
{
	NetCore::Disconnect(m_id);
};
void CTransmitNode::Reconnect()
{
	if (m_id > 0) return;

	m_id = NetCore::Connect(ip.c_str(), port);
};
void CTransmitNode::Disconnect()
{
	m_id = -1;
};
void CTransmitNode::Send(std::shared_ptr<CMessage> msg)
{
	Reconnect();
	NetCore::SendData(m_id, msg->nCmd, msg->GetDataBuf(), msg->nMsgLength, msg->nProtoType);
};
CConnection::CConnection(int fd,int nAuthTime)
{
	m_status = CONNEECT_STATUS_NULL;

	m_id = fd;
	if(nAuthTime > 0)
		CTimerTask::GetInstance()->AddTask(std::bind(&CConnection::OnAuthTimeOut, this), nAuthTime);
}


CConnection::~CConnection()
{
	NetCore::Disconnect(m_id);
}

bool CConnection::OnAuthTimeOut()
{
	if (IsAuth())
		return false;

	m_status = CONNEECT_AUTH_TIMEOUT;
	CServer::GetInstance()->OnAuthTimeOut();
	return false;
}
void CConnection::TransmitAdd(ServerType type, std::string ip, int32_t port)
{
	m_transmitNodes.Add(type, ip, port);
}
void CConnection::TransmitDisconnect(int fd)
{
	m_transmitNodes.Disconnect(fd);
};
ServerType CConnection::GetTranmitType(int nCmd)
{
	//NS_Login
	if (NS_Login::Request::CMD_NULL < nCmd && nCmd < NS_Login::Request::CMD_MAX)
		return ServerType::SERVER_LOGIN;
	//NS_Game
	else if (NS_Game::Request::CMD_NULL < nCmd && nCmd < NS_Game::Request::CMD_MAX)
		return ServerType::SERVER_GAME;
	//NS_DB
	else if (NS_DB::Request::CMD_NULL < nCmd && nCmd < NS_DB::Request::CMD_MAX)
		return ServerType::SERVER_DB;
	//NS_Cache
	else if (NS_Cache::Request::CMD_NULL < nCmd && nCmd < NS_Cache::Request::CMD_MAX)
		return ServerType::SERVER_CACHE;

	return ServerType::SERVER_INVALID;
};
void CConnection::TransmitSend(std::shared_ptr<CMessage> msg)
{
	m_transmitNodes.Send(GetTranmitType(msg->nCmd), msg);
}
