#include "CServer.h"
#include "CConnection.h"
#include "CTimerTask.h"

CServer::CServer()
{
	m_bRunning = false;
	m_type = ServerType::SERVER_GATE;
	m_connects.Clear();
	m_serverList.clear();
}


CServer::~CServer()
{
}
bool CServer::OnNetEventHandler(int nClientID, int nCmd, int nMsgLength, char* msgBuf, NetCore::ProtocolType type)
{
	std::cout << __FUNCTION__ << "  ClientID:" << nClientID << "  nCmd:" << nCmd << std::endl;

	std::shared_ptr<CMessage> pCurMessage = CServer::GetInstance()->m_curMessage;
	pCurMessage->nCmd = nCmd;
	pCurMessage->nMsgLength = nMsgLength;
	pCurMessage->nProtoType = type;
	//��ʼ��β��
	pCurMessage->GetDataBuf()[nMsgLength] = 0;
	memcpy(pCurMessage->GetDataBuf(), msgBuf, nMsgLength * sizeof(char));

	if (Return_true == CServer::GetInstance()->Dispatch(nClientID, pCurMessage))
	{
		return true;
	}

	return CServer::GetInstance()->OnTransmitRequest(nClientID,nCmd,pCurMessage);
}
//��Ϣת��
ReturnType CServer::OnTransmitRequest(int fd, int nCmd, std::shared_ptr<CMessage> msg)
{
	//
	if (!m_connects.Exist(fd))
	{
		XINFO("connect is invalid!");
		return ReturnType::Return_false;
	}
	//
	auto pConn = m_connects.Get(fd);
	if (!pConn->IsAuth())
	{
		XINFO("connect is need authorization!");
		return ReturnType::Return_true;
	}

	pConn->TransmitSend(msg);

	return ReturnType::Return_true;
};
ReturnType CServer::OnConnect(int fd, std::shared_ptr<CMessage>)
{
	if (!m_connects.Exist(fd))
	{
		std::shared_ptr<CConnection> conn = std::make_shared<CConnection>(fd,m_config.nAuthTime);
		m_connects.Add(fd, conn);
	}
	
	return ReturnType::Return_true;
}
ReturnType CServer::OnDisconnect(int fd, std::shared_ptr<CMessage> msg)
{
	if(m_connects.Exist(fd))
		m_connects.Remove(fd);
	else
	{
		//ת���ڵ�Ͽ�
		m_connects.GetMatch([&fd](std::shared_ptr<CConnection> param) {
			param->TransmitDisconnect(fd);
			return false;
		});
	}
	return ReturnType::Return_true;
}
ReturnType CServer::OnConnectAuth(int fd, std::shared_ptr<CMessage> msg)
{
	auto pConn = m_connects.Get(fd);
	if (pConn)
	{
		if (!pConn->IsAuth())
		{
			//��ʼ��ת���ڵ�
			//��¼�ڵ�
			ServerInfo login = GetTargetServerInfo(ServerType::SERVER_LOGIN);
			if(login.type != ServerType::SERVER_INVALID)
				pConn->TransmitAdd(ServerType::SERVER_LOGIN, login.ip, login.port);
			//��Ϸ�ڵ�
			ServerInfo game = GetTargetServerInfo(ServerType::SERVER_GAME);
			if (game.type != ServerType::SERVER_INVALID)
				pConn->TransmitAdd(ServerType::SERVER_GAME, game.ip, game.port);
		}
		pConn->Auth();
	}
	return ReturnType::Return_true;
}
ReturnType CServer::OnServerList(int, std::shared_ptr<CMessage> msg)
{
	NS_Center::ReplyServerList list = ProtoParseJson::Parse<NS_Center::ReplyServerList>(msg);
	m_serverList.clear();
	for (auto it : list.servers)
	{
		ServerType type = ServerType(it.type);
		m_serverList[type].emplace_back(type,it.ip,it.port);
	}
	return ReturnType::Return_true;
}
bool CServer::OnAuthTimeOut()
{
	auto pConn = m_connects.GetMatch([](std::shared_ptr<CConnection> param) {
		return param->GetStatus() == CONNEECT_AUTH_TIMEOUT ? true : false;
	});
	if (pConn)
	{
		m_connects.Remove(pConn->GetID());
	}
	return false;
};
ServerInfo CServer::GetTargetServerInfo(ServerType type)
{
	//��ȡ���Žڵ�
	auto it = m_serverList.find(type);
	if (it != m_serverList.end())
	{
		//(��ʱȡ��һ��)
		return it->second[0];
	}
	return ServerInfo();
}
;
void CServer::Init()
{
	RegisterMessage(NS_Gate::Request::CMD_AUTH,std::bind(&CServer::OnConnectAuth,this,std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(NS_Center::Reply::Register, std::bind(&CServer::OnRegisterSuccess, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(NS_Center::Reply::OnlineList, std::bind(&CServer::OnServerList, this, std::placeholders::_1, std::placeholders::_2));
}
void CServer::Start()
{
	if (m_bRunning)  return;

	//�����ļ�
	if (!m_config.Init())
	{
		std::cout << "init config fail!" << std::endl;
		assert(false);
		return;
	}
	//��־
	CEasylog::GetInstance()->Init("server.log", LOGLEVEL_DEBUG);
	//��Ϣע��
	Init();
	//����ģ��
	NetCore::Config(m_config.bindPort);
	NetCore::RegisterEvnetHandler(&CServer::OnNetEventHandler);
	NetCore::Start();

	//ע�ᵽ���ķ�����
	if (!RegisterToServer(ServerType::SERVER_CENTER,m_config.registerIP, m_config.registerPort, m_config.bindPort, NS_Center::Request::Register))
	{
		XWARN("Reigster server failed!");
		assert(false);
	}

	m_bRunning = true;
}

void CServer::Stop()
{
	if (!m_bRunning) return;

	NetCore::Stop();
	m_bRunning = false;
}
