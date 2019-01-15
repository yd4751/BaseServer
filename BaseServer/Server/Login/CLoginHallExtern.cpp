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

//
bool CLoginHall::CheckIsLoginUser(const std::string account)
{

	if (m_UserManager.GetUserByIdentify(UserIdentifyType::IDENTIFY_TYPE_ACCOUNT, account))
	{
		return true;
	}

	return false;
};