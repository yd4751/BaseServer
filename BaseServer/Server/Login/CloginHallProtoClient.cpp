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

//////////////////////////////////////
//
//////////////////////////////////////
///
ReturnType CLoginHall::OnDisconnect(int nClientID, std::shared_ptr<CMessage> msg)
{
	std::cout << __FUNCTION__ << std::endl;

	return OnLogout(nClientID, nullptr);
};

//
ReturnType CLoginHall::OnLogin(int nClientID, std::shared_ptr<CMessage> msg)
{
	std::cout << __FUNCTION__ << std::endl;

	RequestAccountLogin req = ProtoParse::Parse<RequestAccountLogin>(msg);
	if (m_ClientRequestCache.find(nClientID) != m_ClientRequestCache.end())
	{
		//���ύ����
		return ReturnType::Return_true;
	}
	//�ѵ�¼���
	std::shared_ptr<CBaseUser> pLoginUser = m_UserManager.GetUserByIdentify(UserIdentifyType::IDENTIFY_TYPE_ACCOUNT, req.account);
	if (pLoginUser)
	{
		//
		if (nClientID != pLoginUser->GetConnectID())
		{
			NetCore::Disconnect(pLoginUser->GetConnectID());
		}
		pLoginUser->BindConnectID(nClientID);
		//�ظ�
		ReplyAccountLogin reply;
		reply.code = ReplyErrorCode::REPLY_CODE_SUCCESS;
		reply.id = pLoginUser->GetID();
		reply.account = pLoginUser->GetUserIdentify().account;
		reply.nick_name = "test";
		std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
		pLoginUser->Send(ServerRepley::REPLY_RESULT_LOGIN, str);
		//
		CScriptInterface::Reconnect(pLoginUser->GetID());
		return ReturnType::Return_true;
	}

	m_ClientRequestCache[nClientID] = 0;
	//Ͷ������
	GetRemoteServer(RemoteServerType::SERVER_TYPE_REDIS)->Call((uint32_t)RedisServerRequest::REDIS_REQUEST_LOGIN, nClientID, req.account, req.password);
	return ReturnType::Return_true;
};
ReturnType CLoginHall::OnGuestLogin(int nClientID, std::shared_ptr<CMessage> msg)
{
	std::cout << __FUNCTION__ << std::endl;

	//RequestAccountLogin req = ProtoParse::Parse<RequestAccountLogin>(msg);
	if (m_ClientRequestCache.find(nClientID) != m_ClientRequestCache.end())
	{
		//���ύ����
		return ReturnType::Return_true;
	}

	m_ClientRequestCache[nClientID] = 0;
	//Ͷ������
	GetRemoteServer(RemoteServerType::SERVER_TYPE_DATABASE)->Call((uint32_t)DatabaseServerRequest::DATABASE_REQUEST_GUEST_LOGIN, nClientID);
	return ReturnType::Return_true;
};
ReturnType CLoginHall::OnLogout(int nClientID, std::shared_ptr<CMessage> msg)
{
	std::cout << __FUNCTION__ << std::endl;
	std::shared_ptr<CBaseUser> pLoginUser = m_UserManager.GetUserByConnectID(nClientID);
	if (pLoginUser)
	{
		//���߽���
		if (msg == nullptr)
		{
			pLoginUser->DisBindConnectID();
		}
		if (!CheckCanLogout(pLoginUser))
		{
			//�������˳�
			std::cout << "Forbid logout, user:" << pLoginUser->GetID() << std::endl;
			return ReturnType::Return_true;
		}

		ReplyAccountLogout reply;
		reply.code = ReplyErrorCode::REPLY_CODE_SUCCESS;
		reply.account = "test";// pLoginUser->GetAccount();
		std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
		pLoginUser->Send(ServerRepley::REPLY_RESULT_LOGIN, str);
		//�Ƴ����
		m_UserManager.RemoveUser(pLoginUser->GetID());
		return ReturnType::Return_true;
	}

	return ReturnType::Return_false;
};

ReturnType CLoginHall::OnAccountCreate(int nClientID, std::shared_ptr<CMessage> msg)
{
	CEasylog::GetInstance()->info("[CLoginHall]", __FUNCTION__);

	RequestAccountCreate request = ProtoParse::Parse<RequestAccountCreate>(msg);
	if (request.account.empty() || request.password.empty())
	{
		CEasylog::GetInstance()->warn("invalid param!");
		return ReturnType::Return_true;
	}

	if (m_ClientRequestCache.find(nClientID) != m_ClientRequestCache.end())
	{
		CEasylog::GetInstance()->warn("repeat request!");
		return ReturnType::Return_true;
	}

	m_ClientRequestCache[nClientID] = 0;
	//Ͷ������,��Ҫ�ṩ�˻�����
	GetRemoteServer(RemoteServerType::SERVER_TYPE_DATABASE)->Call((uint32_t)DatabaseServerRequest::DATABASE_REQUEST_ACCOUNT_CREATE, 1, request.account, request.password);
	return ReturnType::Return_true;
};
ReturnType CLoginHall::OnAccountUpdate(int nClientID, std::shared_ptr<CMessage> msg)
{
	CEasylog::GetInstance()->info("[CLoginHall]", __FUNCTION__);


	//Э�鴦��
	//...

	//Ͷ������
	GetRemoteServer(RemoteServerType::SERVER_TYPE_DATABASE)->Call((uint32_t)DatabaseServerRequest::DATABASE_REQUEST_ACCOUNT_UPDATE);
	return ReturnType::Return_true;
};
ReturnType CLoginHall::OnAccountDelete(int nClientID, std::shared_ptr<CMessage>)
{
	CEasylog::GetInstance()->info("[CLoginHall]", __FUNCTION__);
	//Э�鴦��
	//...

	//Ͷ������
	GetRemoteServer(RemoteServerType::SERVER_TYPE_DATABASE)->Call((uint32_t)DatabaseServerRequest::DATABASE_REQUEST_ACCOUNT_DEL);
	return ReturnType::Return_true;
};