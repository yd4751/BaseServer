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

/////////////////////////////////
//script
/////////////////////////////////
void CLoginHall::UpdatePlaying(int32_t nUid)
{
	std::shared_ptr<CBaseUser> pUser = m_UserManager.GetUser(nUid);
	if (!pUser)
	{
		CEasylog::GetInstance()->warn("can not find user:", nUid);
		return;
	}
	pUser->UpdateStatus(UserStatus::USER_STATUS_PLAYING);
};
void CLoginHall::UpdateExited(int32_t nUid)
{
	std::shared_ptr<CBaseUser> pUser = m_UserManager.GetUser(nUid);
	if (!pUser)
	{
		CEasylog::GetInstance()->warn("can not find user:", nUid);
		return;
	}
	pUser->UpdateStatus(UserStatus::USER_STATUS_EXITED);
};
void CLoginHall::UpdateUserInfo(int32_t nUid, const std::map<std::string, CValueInfo>& info)
{
	std::shared_ptr<CBaseUser> pUser = m_UserManager.GetUser(nUid);
	if (!pUser)
	{
		CEasylog::GetInstance()->warn("can not find user:", nUid);
		return;
	}
	std::map<std::string, CValueInfo> newInfo = pUser->UpdateInfo(info);
	//同步玩家信息给lua
	CScriptInterface::UpdateUserInfo(nUid, newInfo);
};
void CLoginHall::SendData(int32_t nUid, int32_t nCmd, std::string msg)
{
	std::shared_ptr<CBaseUser> pUser = m_UserManager.GetUser(nUid);
	if (!pUser)
	{
		CEasylog::GetInstance()->warn("can not find user:", nUid);
		return;
	}

	pUser->Send(nCmd, msg);
};