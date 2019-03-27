#include "CServer.h"
#include "CScriptInterface.h"

CServer::CServer()
{
	m_bRunning = false;
	m_type = ServerType::SERVER_GAME;
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

	auto pUser = CServer::GetInstance()->GetUserByConnectID(nClientID);
	if (pUser)
	{
		return CScriptInterface::Dispatch(pUser->id, nCmd, pCurMessage->GetDataBuf());
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
ReturnType CServer::OnLogin(int fd, std::shared_ptr<CMessage> msg)
{
	NS_Game::ReqLogin req = ProtoParseJson::Parse<NS_Game::ReqLogin>(msg);

	std::shared_ptr<CUser> pUser = std::make_shared<CUser>();
	pUser->id = req.id;

	if (!RDLogin(pUser))
	{
		XINFO(__FUNCTION__,"fail!");
		return ReturnType::Return_true;
	}
	pUser->fd = fd;
	m_users.Add(pUser->id,pUser);

	NS_Game::ReplyLogin reply;
	reply.id = pUser->id;
	reply.sex = pUser->sex;
	reply.money = pUser->money;
	reply.nickName = pUser->nickName;
	reply.avatar = pUser->avatar;
	Send(fd, NS_Game::Reply::Login, ProtoParseJson::MakePacket<NS_Game::ReplyLogin>(reply));
	return ReturnType::Return_true;
}
;
void CServer::Init()
{
	RegisterMessage(NS_Game::Request::Login, std::bind(&CServer::OnLogin, this, std::placeholders::_1, std::placeholders::_2));
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

	//注册到中心服务器
	if (!RegisterToServer(ServerType::SERVER_CENTER, m_config.registerIP, m_config.registerPort,
		m_config.bindPort, NS_Center::Request::Register, (int)NS_Game::Request::CMD_NULL, (int)NS_Game::Request::CMD_MAX)
		)
	{
		XWARN("Reigster server failed!");
		assert(false);
	}

	//直连redis
	if (m_redis.Connect(m_config.cacheServer.ip, m_config.cacheServer.port, "bighat"))
	{
		XINFO("Connect redis success!");
	}

	//初始化脚本
	CScriptInterface::GetInstance()->InitScript();

	m_bRunning = true;
}

void CServer::Stop()
{
	if (!m_bRunning) return;

	NetCore::Stop();
	m_bRunning = false;
}
