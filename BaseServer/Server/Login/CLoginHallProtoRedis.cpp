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

////////////////////////////////////////////////////////////////
//RedisServer   Reply
////////////////////////////////////////////////////////////////
//RemoteServer Reply
ReturnType CLoginHall::OnRedisReplyLogin(int nClientID, std::shared_ptr<CMessage> msg)
{
	CEasylog::GetInstance()->info("[CLoginHall]", __FUNCTION__);

	CRpc p(msg->GetDataBuf(), msg->nMsgLength);
	uint32_t nReqClientID;
	p >> nReqClientID;

	ReplyAccountLogin reply;
	p >> reply.code;
	p >> reply.account;
	if (reply.code == ReplyErrorCode::REPLY_CODE_SUCCESS)
	{
		p >> reply.id;
		p >> reply.nick_name;
	}

	if (m_ClientRequestCache.find(nReqClientID) == m_ClientRequestCache.end())
	{
		//无效玩家	(可能已掉线)
		return ReturnType::Return_true;
	}

	int32_t nConnectID = nReqClientID;
	m_ClientRequestCache.erase(nConnectID);
	std::cout << "cursize:" << m_ClientRequestCache.size() << " ::" << nConnectID << std::endl;

	if (reply.code != ReplyErrorCode::REPLY_CODE_SUCCESS)
	{
		//登录失败
		std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
		CBaseUser::Send(nConnectID, ServerRepley::REPLY_RESULT_LOGIN, str);
		return ReturnType::Return_true;
	}

	std::shared_ptr<CBaseUser> pLoginUser = m_UserManager.AddUser(reply.id);
	if (!pLoginUser)
	{
		//未知异常
		reply.code = ReplyErrorCode::REPLY_CODE_ERROR_UNDEFINED;
		std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
		CBaseUser::Send(nConnectID, ServerRepley::REPLY_RESULT_LOGIN, str);
		return ReturnType::Return_true;
	}

	pLoginUser->Init(reply.id);
	//
	pLoginUser->GetUserIdentify().account = reply.account;
	pLoginUser->BindConnectID(nConnectID);

	std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
	pLoginUser->Send(ServerRepley::REPLY_RESULT_LOGIN, str);

	return ReturnType::Return_true;
};
ReturnType CLoginHall::OnRedisReplyLogout(int nClientID, std::shared_ptr<CMessage>)
{
	return ReturnType::Return_true;
};
