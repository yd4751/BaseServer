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
	Send(fd, NS_Login::Reply::CMD_LOGIN);
	return ReturnType();
}
ReturnType CServer::OnGuestLogin(int fd, std::shared_ptr<CMessage> msg)
{
	//测试
	NS_Login::ReplyUserLogin reply;
	reply.id = Tools::GetRandom(100000, 999999);//随机6位数id
	reply.money = Tools::GetRandom(100, 100000);//随机携带
	reply.sex = Tools::GetRandom(0, 2);//随机性别 0-未填写 1-男 2-女
	reply.nickName = "大帽子:" + std::to_string(reply.id);
	reply.avatar = "www.google.com";

	Send(fd,NS_Login::Reply::CMD_LOGIN,ProtoParseJson::MakePacket< NS_Login::ReplyUserLogin>(reply));
	return ReturnType();
}
;
void CServer::Init()
{
	RegisterMessage(NS_Login::Request::CMD_LOGIN, std::bind(&CServer::OnLogin, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(NS_Login::Request::CMD_GUEST_LOGIN, std::bind(&CServer::OnGuestLogin, this, std::placeholders::_1, std::placeholders::_2));
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

	m_bRunning = true;
}

void CServer::Stop()
{
	if (!m_bRunning) return;

	NetCore::Stop();
	m_bRunning = false;
}
