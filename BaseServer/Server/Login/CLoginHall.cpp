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
	//��ͨ��Ϣע��
	//
	//RegisterMessage(NetCore::ClientMangerCmd::DISCONNECT, std::bind(&CLoginHall::OnDisconnect, this, std::placeholders::_1, std::placeholders::_2));
	//
	RegisterMessage(ClientRequest::LOGIN_SERVER, std::bind(&CLoginHall::OnLogin, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(ClientRequest::LOGOUT_SERVER, std::bind(&CLoginHall::OnLogout, this, std::placeholders::_1, std::placeholders::_2));

	//Redis�������ظ���Ϣ
	RegisterMessage(RedisServerReply::REDIS_REPLY_LOGIN, std::bind(&CLoginHall::OnRedisReplyLogin, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(RedisServerReply::REDIS_REPLY_LOGOUT, std::bind(&CLoginHall::OnRedisReplyLogout, this, std::placeholders::_1, std::placeholders::_2));

	//���ӵ�¼������  //����redis��Ϣ��  95.179.238.125   8000  bighat
	std::shared_ptr<CRedisServer> pRedisServer = std::make_shared<CRedisServer>(
		CServerConfig("127.0.0.1", 8888,"bighat"),
		RemoteServerType::SERVER_TYPE_REDIS,
		shared_from_this()
		);
	m_RemoteServers[RemoteServerType::SERVER_TYPE_REDIS] = std::dynamic_pointer_cast<CRemoteServer>(pRedisServer);
	if (m_RemoteServers[RemoteServerType::SERVER_TYPE_REDIS].get()) m_RemoteServers[RemoteServerType::SERVER_TYPE_REDIS]->Start();
	/*//����RemoteServer
	{
		//�������ݿ������
		std::shared_ptr<CDataServer> pDataServer = std::make_shared<CDataServer>(
			CServerConfig("127.0.0.1", 3001),
			SERVER_TYPE_DATABASE,
			shared_from_this()
			);
		m_RemoteServers[RemoteServerType::SERVER_TYPE_LOGIN] = std::dynamic_pointer_cast<CRemoteServer>( pDataServer );
		if(m_RemoteServers[RemoteServerType::SERVER_TYPE_DATABASE].get()) m_RemoteServers[RemoteServerType::SERVER_TYPE_DATABASE]->Start();
	}*/

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
		//����
		switch (msg->nCmd)
		{
		//case NetCore::CONNECT:
			//���ݵ�¼����ж�
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
///
ReturnType CLoginHall::OnDisconnect(int nClientID, std::shared_ptr<CMessage> msg)
{
	std::cout << __FUNCTION__ << std::endl;

	return OnLogout(nClientID,nullptr);
};

//RemoteServer Reply
ReturnType CLoginHall::OnRedisReplyLogin(int nClientID, std::shared_ptr<CMessage> msg)
{
	std::cout << "[CLoginHall] " << __FUNCTION__ << std::endl;

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
		//��Ч���	(�����ѵ���)
		return ReturnType::Return_true;
	}

	int32_t nConnectID = itPreLoginInfo->second;
	m_PreLoginUser.erase(itPreLoginInfo);
	std::cout << "cursize:" << m_PreLoginUser.size() << " ::" << nConnectID <<std::endl;

	if (reply.code != ReplyErrorCode::REPLY_CODE_SUCCESS)
	{
		//��¼ʧ��
		std::string str = ProtoParse::MakePacket(NetCore::ProtocolType::PROTO_TYPE_JSON, reply);
		CBaseUser::Send(nConnectID, ServerRepley::REPLY_RESULT_LOGIN_SERVER, str);
		return ReturnType::Return_true;
	}

	std::shared_ptr<CBaseUser> pLoginUser = m_UserManager.AddUser(reply.id);
	if (!pLoginUser)
	{
		//δ֪�쳣
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

//
ReturnType CLoginHall::OnLogin(int nClientID, std::shared_ptr<CMessage> msg)
{
	std::cout << __FUNCTION__ << std::endl;
	Json::Reader reader;
	Json::Value packet;
	
	RequestAccountLogin req = ProtoParse::Parse<RequestAccountLogin>(msg);
	if (m_PreLoginUser.find(req.account) != m_PreLoginUser.end())
	{
		//���ύ����
		return ReturnType::Return_true;
	}
	//�ѵ�¼���
	std::shared_ptr<CBaseUser> pLoginUser = m_UserManager.GetUserByIdentify(UserIdentifyType::IDENTIFY_TYPE_ACCOUNT,req.account);
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
		pLoginUser->Send(ServerRepley::REPLY_RESULT_LOGIN_SERVER, str);
		//
		CScriptInterface::Reconnect(pLoginUser->GetID());
		return ReturnType::Return_true;
	}

	m_PreLoginUser[req.account] = nClientID;
	//Ͷ������
	GetRemoteServer(RemoteServerType::SERVER_TYPE_REDIS)->Call((uint32_t)RedisServerRequest::REDIS_REQUEST_LOGIN, req.account, req.password);
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
		pLoginUser->Send(ServerRepley::REPLY_RESULT_LOGIN_SERVER, str);
		//�Ƴ����
		m_UserManager.RemoveUser(pLoginUser->GetID());
		return ReturnType::Return_true;
	}

	return ReturnType::Return_false;
};
