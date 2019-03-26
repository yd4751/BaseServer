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
	XINFO(__FUNCTION__ , "  ClientID:" , nClientID , "  nCmd:" , nCmd );

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

	return CServer::GetInstance()->OnTransmitMessage(nClientID,nCmd,pCurMessage);
}
//消息转发
ReturnType CServer::OnTransmitMessage(int fd, int nCmd, std::shared_ptr<CMessage> msg)
{
	//处理请求还是回复
	if (m_connects.Exist(fd))
	{
		auto pConn = m_connects.Get(fd);
		if (!pConn->IsAuth())
		{
			XINFO("connect is need authorization!");
			return ReturnType::Return_true;
		}
		XDEBUG(__FUNCTION__,"Request ", fd);
		//若节点不存在，则新增节点
		if (!pConn->TransmitExist(fd))
		{
			ServerType type = GetServerType(nCmd);
			if (type == ServerType::SERVER_INVALID)
			{
				XWARN("Can not find server node, cmd:",nCmd);
				return ReturnType::Return_true;
			}

			ServerInfo server = GetTargetServerInfo(type);
			pConn->TransmitAdd(type, server.ip, server.port);
		}
		
		//转发请求
		pConn->TransmitSend(msg);
		return ReturnType::Return_true;
	}

	XDEBUG(__FUNCTION__, "	Reply ", fd);
	//这样遍历效率比较低，以后看有没有什么好办法
	auto pConn = m_connects.GetMatch([&fd](std::shared_ptr<CConnection> param) {
		return param->TransmitExist(fd);
	});

	if (pConn)
	{
		pConn->Send(msg);
	}
	else
		XWARN("Can not handler cmd:", nCmd," from :",fd);
	

	return ReturnType::Return_true;
}
;
ReturnType CServer::OnConnect(int fd, std::shared_ptr<CMessage>)
{
	if (!m_connects.Exist(fd))
	{
		XDEBUG(__FUNCTION__,fd);
		std::shared_ptr<CConnection> conn = std::make_shared<CConnection>(fd,m_config.nAuthTime);
		m_connects.Add(fd, conn);
	}
	
	return ReturnType::Return_true;
}
ReturnType CServer::OnDisconnect(int fd, std::shared_ptr<CMessage> msg)
{
	if (m_connects.Exist(fd))
	{
		XDEBUG(__FUNCTION__, fd);
		m_connects.Remove(fd);
	}
	else
	{
		//转发节点断开
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
		pConn->Auth();
		pConn->Send(NS_Gate::Reply::CMD_AUTH);
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
		m_serverList[type].emplace_back(type,it.ip,it.port,it.cmdStart,it.cmdEnd);
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
ServerType CServer::GetServerType(int cmd)
{
	for (auto& it : m_serverList)
	{
		if (it.second.begin()->cmdStart < cmd && cmd < it.second.begin()->cmdEnd)
			return it.first;
	}
	return ServerType::SERVER_INVALID;
}
ServerInfo CServer::GetTargetServerInfo(ServerType type)
{
	//获取最优节点
	auto it = m_serverList.find(type);
	if (it != m_serverList.end())
	{
		//(暂时取第一个)
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
	if (!RegisterToServer(ServerType::SERVER_CENTER,m_config.registerIP, m_config.registerPort, 
		m_config.bindPort, NS_Center::Request::Register,(int)NS_Gate::Request::CMD_NULL, (int)NS_Gate::Request::CMD_MAX)
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
