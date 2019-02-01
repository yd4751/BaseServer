#include "CRedisServer.h"
#include "CRpc.h"
#include "message_define.h"
using namespace MessageDefine;

CRedisServer::CRedisServer(CServerConfig config, RemoteServerType type, std::weak_ptr<CLoginHall> pGame):
	CRemoteServer(config, type)
{

	m_pGame = pGame;
}


CRedisServer::~CRedisServer()
{
	
}

void CRedisServer::Reconnect()
{
	if (!m_Redis.IsOnline())
	{

		if (!m_Redis.Connect(m_config.ip, m_config.port, m_config.password))
		{
			CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
			return;
		}
		
		std::cout << "Connect Redis " << m_config.ip << " : " << m_config.port << " success!" << std::endl;
	}
};
void CRedisServer::Start()
{
	RegisterMessage(RedisServerRequest::REDIS_REQUEST_LOGIN, std::bind(&CRedisServer::OnLogin, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(RedisServerRequest::REDIS_REQUEST_LOGOUT, std::bind(&CRedisServer::OnLogout, this, std::placeholders::_1, std::placeholders::_2));
	
	Reconnect();
};


bool CRedisServer::Send(uint32_t nCmd)
{
	if (!m_Redis.IsOnline())
	{
		return false;
	}

	return true;
};
bool CRedisServer::Send(uint32_t nCmd, std::string str)
{
	if (!m_Redis.IsOnline())
	{
		return false;
	}

	return true;
};

void CRedisServer::SendCall(uint32_t nLength)
{
	if (!m_Redis.IsOnline())
	{
		return ;
	}
	
	std::shared_ptr<CMessage> msg = std::make_shared<CMessage>();
	msg->data = std::make_shared<CArray<char>>(nLength);
	CRpc::GetSerializeData(msg->GetDataBuf(), nLength);
	CRpc p(msg->GetDataBuf(), nLength);
	uint32_t nCmd = 0;
	p >> nCmd;

	msg->nCmd = nCmd;
	msg->nMsgLength = nLength;
	Dispatch(-1, msg);
};
//
ReturnType CRedisServer::OnLogin(int nClientID, std::shared_ptr<CMessage> msg)
{
	CRpc p(msg->GetDataBuf(), msg->nMsgLength);
	uint32_t nCmd;
	p >> nCmd;
	std::string strAccount;
	p >> strAccount;
	std::string strPassword;
	p >> strPassword;
	CEasylog::GetInstance()->info("[RedisServer] ", __FUNCTION__,"Account:", strAccount,"Password:", strPassword);

	ReplyAccountLogin reply;

	reply.code = ReplyErrorCode::REPLY_CODE_SUCCESS;
	reply.account = strAccount;
	//获取玩家UID
	auto result = m_Redis.Get<std::string>(reply.account);
	if(!std::get<0>(result))
	{
		reply.code = ReplyErrorCode::REPLY_CODE_ACCOUNT_NOT_EXIST;
	}
	else
	{
		reply.id = std::stoi(std::get<1>(result));

		auto tUserTableName = TABLE_USERINFO_PRFIX + std::to_string(reply.id);
		//获取密码
		auto tuplePassword = m_Redis.HGet<std::string>(tUserTableName, "password");
		if (!std::get<0>(tuplePassword))
		{
			reply.code = ReplyErrorCode::REPLY_CODE_ACCOUNT_ERROR_INFO;
		}
		else
		{
			//校验密码
			if (std::get<1>(tuplePassword) != strPassword)
			{
				reply.code = ReplyErrorCode::REPLY_CODE_ACCOUNT_ERROR_PASSWORD;
			}
		}

		//获取玩家信息
		auto tupleNickName = m_Redis.HGet<std::string>(tUserTableName, "nick_name");
		if (!std::get<0>(tupleNickName))
		{
			reply.code = ReplyErrorCode::REPLY_CODE_ACCOUNT_ERROR_INFO;
		}
		else
		{
			reply.nick_name = std::get<1>(tupleNickName);
		}
	}
	
	uint32_t nReplyCmd = RedisServerReply::REDIS_REPLY_LOGIN;
	uint32_t nReplySize = 0;
	if (ReplyErrorCode::REPLY_CODE_SUCCESS == reply.code)
	{
		nReplySize = p.SerializeToBuf(m_pReplyMessage->GetDataBuf(), nReplyCmd,
			reply.code,
			reply.account,
			reply.id,
			reply.nick_name
		);
	}
	else
	{
		nReplySize = p.SerializeToBuf(m_pReplyMessage->GetDataBuf(), nReplyCmd,
			reply.code,
			reply.account
		);
	}
	
	m_pReplyMessage->nCmd = nReplyCmd;
	m_pReplyMessage->nMsgLength = nReplySize;
	
	return ReturnType::Return_true;
};

ReturnType CRedisServer::OnLogout(int nClientID, std::shared_ptr<CMessage> msg)
{
	//修改命令，进入CLoginHall处理
	//msg->SetCommond(RedisServerReply::REDIS_REPLY_LOGOUT);
	return ReturnType::Return_true;
};