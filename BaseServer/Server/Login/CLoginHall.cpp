#include "CLoginHall.h"
#include "CDataServer.h"
#include "CRedisServer.h"
#include "global_include.h"
#include "CProtocolParse.h"
#include "message_define.h"
#include "CRpc.h"
#include "CBaseUser.h"
#include "CScriptInterface.h"

using namespace MessageDefine;

void CLoginHall::Start()
{
	//普通消息注册
	//
	//RegisterMessage(NetCore::ClientMangerCmd::DISCONNECT, std::bind(&CLoginHall::OnDisconnect, this, std::placeholders::_1, std::placeholders::_2));
	//
	RegisterMessage(ClientRequest::LOGIN, std::bind(&CLoginHall::OnLogin, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(ClientRequest::LOGOUT, std::bind(&CLoginHall::OnLogout, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(ClientRequest::ACCOUNT_CREATE, std::bind(&CLoginHall::OnAccountCreate, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(ClientRequest::ACCOUNT_UPDATE, std::bind(&CLoginHall::OnAccountUpdate, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(ClientRequest::ACCOUNT_DEL, std::bind(&CLoginHall::OnAccountDelete, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(ClientRequest::GUEST_LOGIN, std::bind(&CLoginHall::OnGuestLogin, this, std::placeholders::_1, std::placeholders::_2));

	//Redis服务器回复
	RegisterMessage(RedisServerReply::REDIS_REPLY_LOGIN, std::bind(&CLoginHall::OnRedisReplyLogin, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(RedisServerReply::REDIS_REPLY_LOGOUT, std::bind(&CLoginHall::OnRedisReplyLogout, this, std::placeholders::_1, std::placeholders::_2));

	//数据库服务回复
	RegisterMessage(DatabaseServerReply::DATABASE_REPLY_LOGIN, std::bind(&CLoginHall::OnDataBaseReplyLogin, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(DatabaseServerReply::DATABASE_REPLY_LOGOUT, std::bind(&CLoginHall::OnDataBaseReplyLogout, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(DatabaseServerReply::DATABASE_REPLY_ACCOUNT_CREATE, std::bind(&CLoginHall::OnDataBaseReplyAccountCreate, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(DatabaseServerReply::DATABASE_REPLY_ACCOUNT_UPDATE, std::bind(&CLoginHall::OnDataBaseReplyAccountUpdate, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(DatabaseServerReply::DATABASE_REPLY_ACCOUNT_DEL, std::bind(&CLoginHall::OnDataBaseReplyAccountDelete, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(DatabaseServerReply::DATABASE_REPLY_GUEST_LOGIN, std::bind(&CLoginHall::OnDataBaseReplyGuestLogin, this, std::placeholders::_1, std::placeholders::_2));
	
	//连接登录服务器  //外网redis信息：  95.179.238.125   8000  bighat
	std::shared_ptr<CRedisServer> pRedisServer = std::make_shared<CRedisServer>(
		CServerConfig("127.0.0.1", 8888,"bighat"),
		RemoteServerType::SERVER_TYPE_REDIS,
		shared_from_this()
		);
	m_RemoteServers[RemoteServerType::SERVER_TYPE_REDIS] = std::dynamic_pointer_cast<CRemoteServer>(pRedisServer);
	if (m_RemoteServers[RemoteServerType::SERVER_TYPE_REDIS].get()) m_RemoteServers[RemoteServerType::SERVER_TYPE_REDIS]->Start();
	//连接RemoteServer
	{
		//连接数据库服务器
		std::shared_ptr<CDataServer> pDataServer = std::make_shared<CDataServer>(
			CServerConfig("127.0.0.1", 3306,"root","123456"),
			RemoteServerType::SERVER_TYPE_DATABASE,
			shared_from_this()
			);
		m_RemoteServers[RemoteServerType::SERVER_TYPE_DATABASE] = std::dynamic_pointer_cast<CRemoteServer>( pDataServer );
		if(m_RemoteServers[RemoteServerType::SERVER_TYPE_DATABASE].get()) m_RemoteServers[RemoteServerType::SERVER_TYPE_DATABASE]->Start();
	}

	//Script
	CScriptInterface::GetInstance()->InitScript();
};

std::shared_ptr<CRemoteServer> CLoginHall::GetRemoteServer(RemoteServerType type)
{
	auto it = m_RemoteServers.find(type);
	if (it != m_RemoteServers.end()) return it->second;

	return std::shared_ptr<CRemoteServer>();
};

//
ReturnType CLoginHall::Dispatch(int nClientID, std::shared_ptr<CMessage> msg)
{
	ReturnType ret = CMessageHandler::Dispatch(nClientID, msg);
	if(ReturnType::Return_error != ret)
		return ret;
	std::shared_ptr<CBaseUser> pUser = m_UserManager.GetUserByConnectID(nClientID);
	if (pUser)
	{
		//连接
		switch (msg->nCmd)
		{
		//case NetCore::CONNECT:
			//根据登录结果判断
			//CScriptInterface::Reconnect();
		//	return Return_true;
		case NetCore::DISCONNECT:
			CScriptInterface::Disconnect(pUser->GetID());
			return Return_true;
		default:
			if(CScriptInterface::Dispatch(pUser->GetID(), msg->nCmd, msg->GetDataBuf()))
				return Return_true;
		}
	}

	CEasylog::GetInstance()->warn("can not handler messge!",msg->nCmd,nClientID);
	return Return_error;
};


