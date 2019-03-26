#include "CNodeServerMgr.h"

CNodeServer::CNodeServer()
{
	m_fd = -1;
	ip = "";
	port = 0;
	type = ServerType::SERVER_INVALID;
}

CNodeServer::CNodeServer(ServerType type, std::string ip, uint32_t port)
{
	m_fd = -1;
	this->ip = ip;
	this->port = port;
	this->type = type;

	Reconnect();
}

CNodeServer::~CNodeServer()
{
	Disconnect();
}

void CNodeServer::Reconnect()
{
	if (m_fd > 0)
		return;
	if (ip.empty())
		XWARN("CNodeServer Invalid ip !");

	m_fd = NetCore::Connect(ip.c_str(), port);
	if(m_fd > 0)
		XDEBUG("NodeServer:[",ip," | ",port,"] connect success!");
	else
		XDEBUG("NodeServer:[", ip, " | ", port, "] connect fail!");
}

void CNodeServer::Disconnect()
{
	if (m_fd > 0)
		NetCore::Disconnect(m_fd);

	m_fd = -1;
}

void CNodeServer::Send(std::shared_ptr<CMessage> msg)
{
	Reconnect();

	NetCore::SendData(m_fd, msg->nCmd, msg->GetDataBuf(), msg->nMsgLength, msg->nProtoType);
}

void CNodeServer::Send(int cmd, std::string & data)
{
	Reconnect();

	NetCore::SendData(m_fd, cmd, data.c_str(), data.length(), NetCore::ProtocolType::PROTO_TYPE_JSON);
}

void CNodeServer::Send(int cmd)
{
	Reconnect();

	NetCore::SendData(m_fd, cmd, nullptr, 0, NetCore::ProtocolType::PROTO_TYPE_JSON);
}

CNodeServerMgr::CNodeServerMgr()
{
}

bool CNodeServerMgr::Exists(ServerType type)
{
	auto pConn = m_servers.GetMatch([&type](std::shared_ptr<CNodeServer> param) {
		return param->type == type ? true : false;
	});
	return pConn == nullptr ? false : true;
}

bool CNodeServerMgr::Exists(int fd)
{
	auto pConn = m_servers.GetMatch([&fd](std::shared_ptr<CNodeServer> param) {
		return param->GetID() == fd ? true : false;
	});
	return pConn == nullptr ? false : true;
}

void CNodeServerMgr::Add(ServerType type, std::string ip, uint32_t port)
{
	if (m_servers.Exist(type))
	{
		XWARN("NodeServer exist!");
		return;
	}

	std::shared_ptr<CNodeServer> conn = std::make_shared<CNodeServer>(type, ip, port);
	m_servers.Add(type, conn);
}

void CNodeServerMgr::Disconnect(int fd)
{
	auto pConn = m_servers.GetMatch([&fd](std::shared_ptr<CNodeServer> param) {
		if (param->GetID() == fd)
		{
			param->Disconnect();
			return true;
		}
		return false;
	});
}

void CNodeServerMgr::Send(ServerType type, std::shared_ptr<CMessage> msg)
{
	auto pConn = m_servers.Get(type);
	if (pConn)
	{
		pConn->Send(msg);
	}
	else
		XWARN("Can not find type server,cmd:",msg->nCmd);
}

void CNodeServerMgr::Send(ServerType type,int cmd, std::string data)
{
	auto pConn = m_servers.Get(type);
	if (pConn)
	{
		pConn->Send(cmd, data);
	}
	else
		XWARN("Can not find type server,cmd:", cmd);
}

void CNodeServerMgr::Send(ServerType type, int cmd)
{
	auto pConn = m_servers.Get(type);
	if (pConn)
	{
		pConn->Send(cmd);
	}
	else
		XWARN("Can not find type server,cmd:", cmd);
}
