#include "CDataServer.h"



CDataServer::CDataServer(CServerConfig config, RemoteServerType type, std::weak_ptr<CLoginHall> pGame):
	CRemoteServer(config, type)
{
	m_pGame = pGame;
}


CDataServer::~CDataServer()
{
}
