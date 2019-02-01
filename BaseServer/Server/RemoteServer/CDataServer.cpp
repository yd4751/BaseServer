#include "CDataServer.h"
#include "CRpc.h"
#include "message_define.h"
using namespace MessageDefine;

CDataServer::CDataServer(CServerConfig config, RemoteServerType type, std::weak_ptr<CLoginHall> pGame):
	CRemoteServer(config, type)
{
	m_pGame = pGame;
}


CDataServer::~CDataServer()
{
}
void CDataServer::Reconnect()
{
	if (m_sql.Connect(m_config.ip, m_config.account, m_config.password, "MYSQL", m_config.port))
	{
		CEasylog::GetInstance()->info("[DataServer]","connect mysql success!");

		if (m_sql.SelectDataBase("bighat"))
		{
			CEasylog::GetInstance()->info("[DataServer]","choose db success!");
		}
	}
	else
	{
		CEasylog::GetInstance()->info("[DataServer]", "connect mysql fail!", m_config.ip, m_config.port, m_config.account, m_config.password);
	}
};
void CDataServer::Start()
{
	RegisterMessage(DatabaseServerRequest::DATABASE_REQUEST_LOGIN, std::bind(&CDataServer::OnLogin, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(DatabaseServerRequest::DATABASE_REQUEST_LOGOUT, std::bind(&CDataServer::OnLogout, this, std::placeholders::_1, std::placeholders::_2));
	//账户管理
	RegisterMessage(DatabaseServerRequest::DATABASE_REQUEST_ACCOUNT_CREATE, std::bind(&CDataServer::OnAccountCreate, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(DatabaseServerRequest::DATABASE_REQUEST_ACCOUNT_UPDATE, std::bind(&CDataServer::OnAccountUpdate, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(DatabaseServerRequest::DATABASE_REQUEST_ACCOUNT_DEL, std::bind(&CDataServer::OnAccountDelete, this, std::placeholders::_1, std::placeholders::_2));

	Reconnect();
};

bool CDataServer::Send(uint32_t nCmd)
{
	return false;
};
bool CDataServer::Send(uint32_t nCmd, std::string str)
{
	return false;
};
void CDataServer::SendCall(uint32_t nLength)
{
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

ReturnType CDataServer::OnLogin(int nClientID, std::shared_ptr<CMessage> msg)
{
	CRpc p(msg->GetDataBuf(), msg->nMsgLength);
	uint32_t nCmd;
	p >> nCmd;

	uint32_t nLoginType;
	p >> nLoginType;
	std::string strAccount;
	p >> strAccount;
	std::string strPassword;
	p >> strPassword;
	CEasylog::GetInstance()->info("[DataServer] ", __FUNCTION__, "Account:", strAccount, "Password:", strPassword);

	std::ostringstream strSql;
	strSql << "call UserLogin(" << nLoginType<<",";
	strSql << "\"" << strAccount << "\"" << ",";
	strSql << "\"" << strPassword << "\"" << ",";
	strSql << "@ret,@retUserID)";

	if (m_sql.DoSql(strSql.str(), -1))
	{
		m_sql.DoSql("select @ret as \"Code\",@retUserID \"NewID\"", 1);
	}
	auto itRlt = m_sql.GetData();

	uint32_t nReplyCmd = DatabaseServerReply::DATABASE_REPLY_LOGIN;
	uint32_t nReplySize = 0;
	uint32_t nCode = ReplyErrorCode::REPLY_CODE_SUCCESS;
	std::map<std::string, std::string> pUserInfo;
	int32_t nUserID;

	//fail
	if (itRlt.empty() || itRlt[0].find("Code") == itRlt[0].end())
	{
		nCode = ReplyErrorCode::REPLY_CODE_ACCOUNT_NOT_EXIST;
	}
	//fail, database output formate is error
	else if (itRlt[0].find("NewID") == itRlt[0].end())
	{
		nCode = ReplyErrorCode::REPLY_CODE_ACCOUNT_NOT_EXIST;
	}
	else
	{
		std::string strUserID = itRlt[0].find("NewID")->second;
		nUserID = ::atoi(strUserID.c_str());
		CEasylog::GetInstance()->info("[DataServer]", "UserLogin:", strUserID);

		//get user info
		GetUserInfo(nUserID, pUserInfo);
		if (pUserInfo.empty())
		{
			CEasylog::GetInstance()->warn("[DataServer] user:", strUserID,"get empty user info!");
			nCode = ReplyErrorCode::REPLY_CODE_ACCOUNT_ERROR_INFO;
		}
	}

	p.ResetBuf();
	p.SerializeToBufNoReset(nCode, strAccount);
	if (nCode == ReplyErrorCode::REPLY_CODE_SUCCESS)
	{
		p.SerializeToBufNoReset(nUserID, pUserInfo.size());
		//填充用户信息
		for (auto it : pUserInfo)
		{
			p.SerializeToBufNoReset(it.first, it.second);
		}
	}

	nReplySize = p.ReadSerializeData(m_pReplyMessage->GetDataBuf());
	m_pReplyMessage->nCmd = nReplyCmd;
	m_pReplyMessage->nMsgLength = nReplySize;
	return ReturnType::Return_true;
};
ReturnType CDataServer::OnLogout(int nClientID, std::shared_ptr<CMessage> msg)
{
	CRpc p(msg->GetDataBuf(), msg->nMsgLength);

	return ReturnType::Return_true;
};
void CDataServer::GetUserInfo(int32_t nUserID, std::map<std::string, std::string>& info)
{
	info.clear();

	std::ostringstream strSql;
	strSql << "select * from UserInfo where user_id = " << nUserID;
	m_sql.DoSql(strSql.str(), 100);
	auto itRlt = m_sql.GetData();
	if (!itRlt.empty())
	{
		for (auto it : itRlt[0])
		{
			info.emplace(it.first,it.second);
		}
	}
};

int32_t CDataServer::CreateUser(int32_t nAccountType, const std::string& strAccount, const std::string& strPassword)
{
	int32_t nGetUserID = 0;

	std::ostringstream strSql;
	strSql << "call CreateUser(" << nAccountType << ",";
	strSql << "\"" << strAccount << "\"" << ",";
	strSql << "\"" << strPassword << "\"" << ",";
	strSql << "\"" << "HelloWorld!" << "\"" << ",";
	strSql << "@ret,@retUserID)";

	if (m_sql.DoSql(strSql.str(), -1))
	{
		m_sql.DoSql("select @ret as \"Code\",@retUserID \"NewID\"", 1);
	}

	auto itRlt = m_sql.GetData();
	if (!itRlt.empty() && itRlt[0].find("Code") != itRlt[0].end() && itRlt[0].find("NewID") != itRlt[0].end())
	{
		if (itRlt[0].find("Code")->second == "0")
		{
			nGetUserID = ::atoi(itRlt[0].find("NewID")->second.c_str());
		}
	}

	return nGetUserID;
};
ReturnType CDataServer::OnAccountCreate(int nClientID, std::shared_ptr<CMessage> msg)
{
	CEasylog::GetInstance()->info("[DataServer] ", __FUNCTION__);
	CRpc p(msg->GetDataBuf(), msg->nMsgLength);
	uint32_t nCmd;
	p >> nCmd;

	uint32_t nLoginType;
	p >> nLoginType;
	std::string strAccount;
	p >> strAccount;
	std::string strPassword;
	p >> strPassword;


	uint32_t nReplyCmd = DatabaseServerReply::DATABASE_REPLY_ACCOUNT_CREATE;
	uint32_t nReplySize = 0;

	ReplyAccountCreate reply;
	reply.code = ReplyErrorCode::REPLY_CODE_ERROR_UNDEFINED;
	reply.account = strAccount;
	reply.id = CreateUser(nLoginType, strAccount, strPassword);
	if (reply.id > 0)
		reply.code = ReplyErrorCode::REPLY_CODE_SUCCESS;
	nReplySize = p.SerializeToBuf(m_pReplyMessage->GetDataBuf(), reply.code, reply.account, reply.id);

	m_pReplyMessage->nCmd = nReplyCmd;
	m_pReplyMessage->nMsgLength = nReplySize;
	return ReturnType::Return_true;
};
ReturnType CDataServer::OnAccountUpdate(int nClientID, std::shared_ptr<CMessage>)
{
	CEasylog::GetInstance()->info("[DataServer] ", __FUNCTION__);

	uint32_t nReplyCmd = DatabaseServerReply::DATABASE_REPLY_ACCOUNT_UPDATE;
	uint32_t nReplySize = 0;
	m_pReplyMessage->nCmd = nReplyCmd;
	m_pReplyMessage->nMsgLength = nReplySize;
	return ReturnType::Return_true;
};
ReturnType CDataServer::OnAccountDelete(int nClientID, std::shared_ptr<CMessage>)
{
	CEasylog::GetInstance()->info("[DataServer] ", __FUNCTION__);

	uint32_t nReplyCmd = DatabaseServerReply::DATABASE_REPLY_ACCOUNT_DEL;
	uint32_t nReplySize = 0;
	m_pReplyMessage->nCmd = nReplyCmd;
	m_pReplyMessage->nMsgLength = nReplySize;
	return ReturnType::Return_true;
};