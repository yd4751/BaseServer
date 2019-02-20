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
//DataBaseServer   Reply
////////////////////////////////////////////////////////////////
ReturnType CLoginHall::OnDataBaseReplyGuestLogin(int nClientID, std::shared_ptr<CMessage> msg)
{
	CEasylog::GetInstance()->info("[CLoginHall]", __FUNCTION__);

	CRpc p(msg->GetDataBuf(), msg->nMsgLength);
	uint32_t nReqClientID;
	p >> nReqClientID;

	ReplyAccountLogin reply;
	p >> reply.code;

	//p >> reply.account;

	if (m_ClientRequestCache.find(nReqClientID) == m_ClientRequestCache.end())
	{
		//无效玩家	(可能已掉线)
		return ReturnType::Return_true;
	}

	int32_t nConnectID = nReqClientID;
	m_ClientRequestCache.erase(nReqClientID);
	std::cout << "cursize:" << m_ClientRequestCache.size() << " ::" << nConnectID << std::endl;

	if (reply.code != ReplyErrorCode::REPLY_CODE_SUCCESS)
	{
		//登录失败
		std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
		CBaseUser::Send(nConnectID, ServerRepley::REPLY_RESULT_LOGIN, str);
		return ReturnType::Return_true;
	}

	//success
	uint32_t nUserID;
	p >> nUserID;
	uint32_t nUserInfoCount;
	p >> nUserInfoCount;

	std::map < std::string, std::string > pUserInfo;
	for (uint32_t i = 0; i < nUserInfoCount; ++i)
	{
		std::string infoKey, infoValue;
		p >> infoKey;
		p >> infoValue;
		//save
		pUserInfo.emplace(infoKey, infoValue);
	}

	reply.id = nUserID;
	std::shared_ptr<CBaseUser> pLoginUser = m_UserManager.AddUser(reply.id);
	if (!pLoginUser)
	{
		//未知异常
		reply.code = ReplyErrorCode::REPLY_CODE_ERROR_UNDEFINED;
		std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
		CBaseUser::Send(nConnectID, ServerRepley::REPLY_RESULT_LOGIN, str);
		return ReturnType::Return_true;
	}

	pLoginUser->Init(reply.id, pUserInfo);
	//
	pLoginUser->GetUserIdentify().account = reply.account;
	if (pUserInfo.find("nick_name") != pUserInfo.end())
		reply.nick_name = pUserInfo.find("nick_name")->second;

	pLoginUser->BindConnectID(nConnectID);

	std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
	pLoginUser->Send(ServerRepley::REPLY_RESULT_LOGIN, str);

	return ReturnType::Return_true;
};
ReturnType CLoginHall::OnDataBaseReplyLogin(int nClientID, std::shared_ptr<CMessage> msg)
{
	CEasylog::GetInstance()->info("[CLoginHall]", __FUNCTION__);

	CRpc p(msg->GetDataBuf(), msg->nMsgLength);
	uint32_t nReqClientID;
	p >> nReqClientID;
	uint32_t nCode;
	p >> nCode;

	ReplyAccountLogin reply;
	p >> reply.account;

	if (m_ClientRequestCache.find(nReqClientID) == m_ClientRequestCache.end())
	{
		//无效玩家	(可能已掉线)
		return ReturnType::Return_true;
	}

	int32_t nConnectID = nReqClientID;
	m_ClientRequestCache.erase(nReqClientID);
	std::cout << "cursize:" << m_ClientRequestCache.size() << " ::" << nConnectID << std::endl;

	if (reply.code != ReplyErrorCode::REPLY_CODE_SUCCESS)
	{
		//登录失败
		std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
		CBaseUser::Send(nConnectID, ServerRepley::REPLY_RESULT_LOGIN, str);
		return ReturnType::Return_true;
	}

	if (nCode != ReplyErrorCode::REPLY_CODE_SUCCESS)
	{
		//登录失败
		std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
		CBaseUser::Send(nConnectID, ServerRepley::REPLY_RESULT_LOGIN, str);
		return ReturnType::Return_true;
	}

	//success
	uint32_t nUserID;
	p >> nUserID;
	uint32_t nUserInfoCount;
	p >> nUserInfoCount;

	std::map < std::string, std::string > pUserInfo;
	for (uint32_t i = 0; i < nUserInfoCount; ++i)
	{
		std::string infoKey, infoValue;
		p >> infoKey;
		p >> infoValue;
		//save
		pUserInfo.emplace(infoKey, infoValue);
	}

	reply.id = nUserID;
	std::shared_ptr<CBaseUser> pLoginUser = m_UserManager.AddUser(reply.id);
	if (!pLoginUser)
	{
		//未知异常
		reply.code = ReplyErrorCode::REPLY_CODE_ERROR_UNDEFINED;
		std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
		CBaseUser::Send(nConnectID, ServerRepley::REPLY_RESULT_LOGIN, str);
		return ReturnType::Return_true;
	}

	pLoginUser->Init(reply.id, pUserInfo);
	//
	pLoginUser->GetUserIdentify().account = reply.account;
	pLoginUser->BindConnectID(nConnectID);

	std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
	pLoginUser->Send(ServerRepley::REPLY_RESULT_LOGIN, str);

	return ReturnType::Return_true;
};

ReturnType CLoginHall::OnDataBaseReplyLogout(int nClientID, std::shared_ptr<CMessage>)
{
	CEasylog::GetInstance()->info("[CLoginHall]", __FUNCTION__);
	return ReturnType::Return_true;
}
//Account: create update del
ReturnType CLoginHall::OnDataBaseReplyAccountCreate(int nClientID, std::shared_ptr<CMessage> msg)
{
	CEasylog::GetInstance()->info("[CLoginHall]", __FUNCTION__);
	CRpc p(msg->GetDataBuf(), msg->nMsgLength);

	ReplyAccountCreate reply;
	uint32_t nReqClientID;
	p >> nReqClientID;
	p >> reply.code;
	p >> reply.account;
	p >> reply.id;

	if (m_ClientRequestCache.find(nReqClientID) == m_ClientRequestCache.end())
	{
		//无效玩家	(可能已掉线)
		return ReturnType::Return_true;
	}

	int32_t nConnectID = nReqClientID;
	m_ClientRequestCache.erase(nReqClientID);
	if (reply.code != ReplyErrorCode::REPLY_CODE_SUCCESS)
	{
		std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
		CBaseUser::Send(nConnectID, ServerRepley::REPLY_RESULT_LOGIN, str);
		return ReturnType::Return_true;
	}

	std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
	CBaseUser::Send(nConnectID, ServerRepley::REPLY_RESULT_LOGIN, str);
	return ReturnType::Return_true;
};
ReturnType CLoginHall::OnDataBaseReplyAccountUpdate(int nClientID, std::shared_ptr<CMessage>)
{
	CEasylog::GetInstance()->info("[CLoginHall]", __FUNCTION__);
	return ReturnType::Return_true;
};
ReturnType CLoginHall::OnDataBaseReplyAccountDelete(int nClientID, std::shared_ptr<CMessage>)
{
	CEasylog::GetInstance()->info("[CLoginHall]", __FUNCTION__);
	return ReturnType::Return_true;
};