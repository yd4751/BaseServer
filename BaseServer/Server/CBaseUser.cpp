#include "CBaseUser.h"
#include "CScriptInterface.h"

CBaseUser::CBaseUser()
{
	Reset();
}

CBaseUser::~CBaseUser()
{
}

bool CBaseUser::IsUser(UserIdentifyType type, int32_t identify)
{
	switch (type)
	{
	case IDENTIFY_TYPE_PHONE:
		return false;
	case IDENTIFY_TYPE_UID:
		return m_identify.userID == identify ? true : false;
	case IDENTIFY_TYPE_NULL:
	default:
		return false;
	}
}
bool CBaseUser::IsUser(UserIdentifyType type, const std::string identify)
{
	switch (type)
	{
	case IDENTIFY_TYPE_ACCOUNT:
		return m_identify.account == identify ? true : false;
	case IDENTIFY_TYPE_EMAIL:
		return m_identify.email == identify ? true : false;

	case IDENTIFY_TYPE_WCHAT:
		return false;
	case IDENTIFY_TYPE_QQ:
		return false;
	case IDENTIFY_TYPE_NULL:
	default:
		return false;
	}
}

std::map<std::string, CValueInfo> CBaseUser::UpdateInfo(const std::map<std::string, CValueInfo>& info)
{
	if (info.empty())
		return m_info;

	std::map<std::string, CValueInfo> curInfo;
	//同步修改到数据库，获取最新值
	//....

	return curInfo;
}

void CBaseUser::Init(int32_t id)
{
	SetStatus(USER_STATUS_LOGIN);
};
void CBaseUser::Init(int32_t userID, const std::map<std::string, std::string>& info)
{
	Init(userID);

	for (auto it : info)
	{
		m_info[it.first].Set(it.second);
	}

	CScriptInterface::UpdateUserInfo(userID, m_info);
}