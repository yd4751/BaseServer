#include "CRemoteServer.h"
#include "CLoginHall.h"
#include "CRpc.h"

void CRemoteServer::Reconnect()
{
	{
		m_ClientID = NetCore::Connect(m_config.ip.c_str(), m_config.port);
		std::cout << "Connect server " << m_config.ip << " : " << m_config.port << "  ";
		if (m_ClientID < 0)
		{
			std::cout << "fail!" << std::endl;
			return;
		}
		{
			//std::shared_ptr<Client> p(m_pClient);
			//p->BindServerID(m_emType);
		}
		std::cout << "success!" << std::endl;
	}
};
void CRemoteServer::Start()
{
	Reconnect();
};

bool CRemoteServer::Send(uint32_t nCmd)
{
	if (m_ClientID < 0)
	{
		return false;
	}

	{
		//NetCore::SendData(m_ClientID, nCmd, nullptr, 0);
	}
	return true;
};
bool CRemoteServer::Send(uint32_t nCmd, std::string str)
{
	if (m_ClientID < 0)
	{
		return false;
	}

	{
		//NetCore::SendData(m_ClientID, nCmd, str.c_str(), str.length());
	}
	return true;
};

ReturnType CRemoteServer::Dispatch(int nClientID, std::shared_ptr<CMessage> msg)
{
	//这个是为了本地不走socket的服务器使用的
	CMessageHandler::Dispatch(nClientID, msg);
	{
		std::shared_ptr<CLoginHall> p(m_pGame);
		return p->Dispatch(nClientID, m_pReplyMessage);
	}
};