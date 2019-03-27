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
	XINFO(__FUNCTION__, "  ClientID:", nClientID, "  nCmd:", nCmd);

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
ReturnType CServer::OnLogin(int fd, std::shared_ptr<CMessage> msg)
{
	Send(fd, NS_Login::Reply::Login);
	return ReturnType();
}
ReturnType CServer::OnGuestLogin(int fd, std::shared_ptr<CMessage> msg)
{
	//
	std::shared_ptr<CUser>  pUser = std::make_shared<CUser>();
	if (!DBLogin(pUser))
	{
		XINFO(__FUNCTION__,"fail!");
		return ReturnType::Return_true;
	}
	if (!RDSaveUser(pUser))
	{
		XINFO(__FUNCTION__, "fail!");
		return ReturnType::Return_true;
	}
	//
	m_users.Add(pUser->id, pUser);
	
	NS_Login::ReplyUserLogin reply;
	reply.id = pUser->id;
	reply.money = pUser->money;
	reply.sex = pUser->sex;
	reply.nickName = pUser->nickName;
	reply.avatar = pUser->avatar;

	Send(fd,NS_Login::Reply::Login,ProtoParseJson::MakePacket< NS_Login::ReplyUserLogin>(reply));
	return ReturnType::Return_true;
}
;
void CServer::Init()
{
	RegisterMessage(NS_Login::Request::Login, std::bind(&CServer::OnLogin, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(NS_Login::Request::GuestLogin, std::bind(&CServer::OnGuestLogin, this, std::placeholders::_1, std::placeholders::_2));
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
		m_config.bindPort, NS_Center::Request::Register, (int)NS_Login::Request::CMD_NULL, (int)NS_Login::Request::CMD_MAX)
		)
	{
		XWARN("Reigster server failed!");
		assert(false);
	}

#if 0
	//连接缓存服务器, 
	m_nodeServers.Add(ServerType::SERVER_CACHE, m_config.cacheServer.ip, m_config.cacheServer.port);
	//连接DB服务器
	m_nodeServers.Add(ServerType::SERVER_DB, m_config.dbServer.ip, m_config.dbServer.port);
#else
	//直连redis
	if (m_Redis.Connect(m_config.cacheServer.ip, m_config.cacheServer.port, "bighat"))
	{
		XINFO("Connect redis success!");
	}
	//直连mysql
	if (m_sql.Connect(m_config.dbServer.ip, "root", "123456", "MYSQL", m_config.dbServer.port))
	{
		XINFO("Connect mysql success!");
	}
#endif

	m_bRunning = true;
}

void CServer::Stop()
{
	if (!m_bRunning) return;

	NetCore::Stop();
	m_bRunning = false;
}
