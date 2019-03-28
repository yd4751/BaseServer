#include "CServer.h"


CServer::CServer()
{
	m_bRunning = false;
	m_type = ServerType::SERVER_LOGIN;
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
	//初始化尾部
	pCurMessage->GetDataBuf()[nMsgLength] = 0;
	memcpy(pCurMessage->GetDataBuf(), msgBuf, nMsgLength * sizeof(char));

	if (Return_true == CServer::GetInstance()->Dispatch(nClientID, pCurMessage))
	{
		return true;
	}

	return false;
}
ReturnType CServer::OnConnect(int, std::shared_ptr<CMessage>)
{
	return ReturnType();
}
ReturnType CServer::OnDisconnect(int, std::shared_ptr<CMessage>)
{
	return ReturnType();
}
;
void CServer::Init()
{
	RegisterMessage(NS_Gate::Reply::Auth, std::bind(&CServer::OnAuthSucess, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(NS_Login::Reply::Login, std::bind(&CServer::OnLoginSucess, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(NS_Game::Reply::Login, std::bind(&CServer::OnGameLogin, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(451001, std::bind(&CServer::OnRoomLogin, this, std::placeholders::_1, std::placeholders::_2));
}
void CServer::Start()
{
	if (m_bRunning)  return;

	//配置文件
	if (!m_config.Init())
	{
		std::cout << "init config fail!" << std::endl;
		assert(false);
		return;
	}
	//日志
	CEasylog::GetInstance()->Init("server.log", LOGLEVEL_DEBUG);
	//消息注册
	Init();
	//网络模块
	NetCore::Config(m_config.bindPort);
	NetCore::RegisterEvnetHandler(&CServer::OnNetEventHandler);
	NetCore::Start();

	//连接网关服务器
	m_nodeServers.Add(ServerType::SERVER_GATE, "192.168.0.99", 8500);
	SendAuth();

	m_bRunning = true;
}

void CServer::Stop()
{
	if (!m_bRunning) return;

	NetCore::Stop();
	m_bRunning = false;
}
