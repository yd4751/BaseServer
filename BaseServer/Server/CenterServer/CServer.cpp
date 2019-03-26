#include "CServer.h"


CServer::CServer()
{
	m_bRunning = false;
	m_type = ServerType::SERVER_CENTER;
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
	return ReturnType::Return_true;
}
ReturnType CServer::OnDisconnect(int fd, std::shared_ptr<CMessage> msg)
{
	m_serverList.Remove(fd);
	SendList();
	return ReturnType::Return_true;
}
void CServer::SendList()
{
	NS_Center::ReplyServerList list;
	m_serverList.GetMatch([&list](CServerInfo param) {
		list.servers.emplace_back();
		
		list.servers[list.servers.size()-1].ip = param.ip;
		list.servers[list.servers.size() - 1].port = param.port;
		list.servers[list.servers.size() - 1].type = (int)param.type;
		list.servers[list.servers.size() - 1].cmdStart = param.cmdStart;
		list.servers[list.servers.size() - 1].cmdEnd = param.cmdEnd;
		return false;
	});
	m_serverList.GetMatch([this,&list](CServerInfo param) {
		if(param.IsSend())
			Send(param.fd, NS_Center::Reply::OnlineList, ProtoParseJson::MakePacket<NS_Center::ReplyServerList>(list));
		return false;
	});
}
ReturnType CServer::OnRegister(int fd, std::shared_ptr<CMessage> msg)
{
	//协议解析
	NS_Center::ReqRegister req = ProtoParseJson::Parse<NS_Center::ReqRegister>(msg);
	
	CServerInfo info;
	info.fd = fd;
	info.ip = req.ip;
	info.port = req.port;
	info.type = (ServerType)req.type;
	info.cmdStart = req.cmdStart;
	info.cmdEnd = req.cmdEnd;
	m_serverList.Add(fd, info);
	//reply
	Send(fd, NS_Center::Reply::Register);
	//
	SendList();
	return ReturnType::Return_true;
}
ReturnType CServer::OnUpdate(int fd, std::shared_ptr<CMessage> msg)
{
	if (m_serverList.Exist(fd))
	{

	}
	return ReturnType::Return_true;
}
ReturnType CServer::OnGetOnlineList(int fd, std::shared_ptr<CMessage> msg)
{
	NS_Center::ReplyServerList list;
	m_serverList.GetMatch([&list](CServerInfo param) {
		list.servers.emplace_back();

		list.servers[list.servers.size() - 1].ip = param.ip;
		list.servers[list.servers.size() - 1].port = param.port;
		list.servers[list.servers.size() - 1].type = (int)param.type;
		list.servers[list.servers.size() - 1].cmdStart = param.cmdStart;
		list.servers[list.servers.size() - 1].cmdEnd = param.cmdEnd;
		return false;
	});
	Send(fd, NS_Center::Reply::OnlineList, ProtoParseJson::MakePacket<NS_Center::ReplyServerList>(list));
	return ReturnType::Return_true;
}
;
void CServer::Init()
{
	RegisterMessage(NS_Center::Request::Register, std::bind(&CServer::OnRegister, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(NS_Center::Request::Update, std::bind(&CServer::OnUpdate, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(NS_Center::Request::OnlineList, std::bind(&CServer::OnGetOnlineList, this, std::placeholders::_1, std::placeholders::_2));
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
	m_bRunning = true;	
	XINFO("Server is running...");
}

void CServer::Stop()
{
	if (!m_bRunning) return;

	NetCore::Stop();
	m_bRunning = false;
}
