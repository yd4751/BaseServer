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
	uint32_t nCmd;
	p >> nCmd;

	ReplyAccountLogin reply;
	p >> reply.code;
	p >> reply.account;
	if (reply.code == ReplyErrorCode::REPLY_CODE_SUCCESS)
	{
		p >> reply.id;
		p >> reply.nick_name;
	}

	auto itPreLoginInfo = m_PreLoginUser.find(reply.account);
	if (itPreLoginInfo == m_PreLoginUser.end())
	{
		//无效玩家	(可能已掉线)
		return ReturnType::Return_true;
	}

	int32_t nConnectID = itPreLoginInfo->second;
	m_PreLoginUser.erase(itPreLoginInfo);
	std::cout << "cursize:" << m_PreLoginUser.size() << " ::" << nConnectID << std::endl;

	if (reply.code != ReplyErrorCode::REPLY_CODE_SUCCESS)
	{
		//登录失败
		std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
		CBaseUser::Send(nConnectID, ServerRepley::REPLY_RESULT_LOGIN_SERVER, str);
		return ReturnType::Return_true;
	}

	std::shared_ptr<CBaseUser> pLoginUser = m_UserManager.AddUser(reply.id);
	if (!pLoginUser)
	{
		//未知异常
		reply.code = ReplyErrorCode::REPLY_CODE_ERROR_UNDEFINED;
		std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
		CBaseUser::Send(nConnectID, ServerRepley::REPLY_RESULT_LOGIN_SERVER, str);
		return ReturnType::Return_true;
	}

	pLoginUser->Init(reply.id);
	//
	pLoginUser->GetUserIdentify().account = reply.account;
	pLoginUser->BindConnectID(nConnectID);

	std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
	pLoginUser->Send(ServerRepley::REPLY_RESULT_LOGIN_SERVER, str);

	return ReturnType::Return_true;
};
ReturnType CLoginHall::OnRedisReplyLogout(int nClientID, std::shared_ptr<CMessage>)
{
	return ReturnType::Return_true;
};

////////////////////////////////////////////////////////////////
//DataBaseServer   Reply
////////////////////////////////////////////////////////////////
ReturnType CLoginHall::OnDataBaseReplyLogin(int nClientID, std::shared_ptr<CMessage> msg)
{
	CEasylog::GetInstance()->info("[CLoginHall]", __FUNCTION__);

	CRpc p(msg->GetDataBuf(), msg->nMsgLength);
	uint32_t nCode;
	p >> nCode;

	ReplyAccountLogin reply;
	p >> reply.account;
	auto itPreLoginInfo = m_PreLoginUser.find(reply.account);
	if (itPreLoginInfo == m_PreLoginUser.end())
	{
		//无效玩家	(可能已掉线)
		return ReturnType::Return_true;
	}

	int32_t nConnectID = itPreLoginInfo->second;
	m_PreLoginUser.erase(itPreLoginInfo);
	std::cout << "cursize:" << m_PreLoginUser.size() << " ::" << nConnectID << std::endl;

	if (reply.code != ReplyErrorCode::REPLY_CODE_SUCCESS)
	{
		//登录失败
		std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
		CBaseUser::Send(nConnectID, ServerRepley::REPLY_RESULT_LOGIN_SERVER, str);
		return ReturnType::Return_true;
	}

	if (nCode != ReplyErrorCode::REPLY_CODE_SUCCESS)
	{
		//登录失败
		std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
		CBaseUser::Send(nConnectID, ServerRepley::REPLY_RESULT_LOGIN_SERVER, str);
		return ReturnType::Return_true;
	}

	//success
	uint32_t nUserID;
	p >> nUserID;
	uint32_t nUserInfoCount;
	p >> nUserInfoCount;

	std::map < std::string, std::string > pUserInfo;
	std::string infoKey, infoValue;
	for (uint32_t i = 0; i < nUserInfoCount; ++i)
	{
		p >> infoKey;
		p >> infoValue;
		//save
		pUserInfo.emplace(infoKey,infoValue);
	}

	reply.id = nUserID;
	std::shared_ptr<CBaseUser> pLoginUser = m_UserManager.AddUser(reply.id);
	if (!pLoginUser)
	{
		//未知异常
		reply.code = ReplyErrorCode::REPLY_CODE_ERROR_UNDEFINED;
		std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
		CBaseUser::Send(nConnectID, ServerRepley::REPLY_RESULT_LOGIN_SERVER, str);
		return ReturnType::Return_true;
	}

	pLoginUser->Init(reply.id, pUserInfo);
	//
	pLoginUser->GetUserIdentify().account = reply.account;
	pLoginUser->BindConnectID(nConnectID);

	std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
	pLoginUser->Send(ServerRepley::REPLY_RESULT_LOGIN_SERVER, str);

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
	CEasylog::GetInstance()->info("[CLoginHall]",__FUNCTION__);
	CRpc p(msg->GetDataBuf(), msg->nMsgLength);

	ReplyAccountCreate reply;
	p >> reply.code;
	p >> reply.account;
	p >> reply.id;

	auto itPreLoginInfo = m_PreLoginUser.find(reply.account);
	if (itPreLoginInfo == m_PreLoginUser.end())
	{
		//无效玩家	(可能已掉线)
		return ReturnType::Return_true;
	}

	int32_t nConnectID = itPreLoginInfo->second;
	m_PreLoginUser.erase(itPreLoginInfo);
	if (reply.code != ReplyErrorCode::REPLY_CODE_SUCCESS)
	{
		std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
		CBaseUser::Send(nConnectID, ServerRepley::REPLY_RESULT_LOGIN_SERVER, str);
		return ReturnType::Return_true;
	}

	std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
	CBaseUser::Send(nConnectID, ServerRepley::REPLY_RESULT_LOGIN_SERVER, str);
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