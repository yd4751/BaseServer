#ifndef __H_BASE_USERMANAGER_H__
#define __H_BASE_USERMANAGER_H__
#include "global_include.h"
#include "CBaseUser.h"

class CBaseUser;

template<typename T = CBaseUser>
class CBaseUserManager
{
	std::map<int32_t, std::shared_ptr<T>>		m_LoginUsers;

public:
	CBaseUserManager()
	{
	};
	virtual ~CBaseUserManager()
	{
	};

public:
	std::shared_ptr<T> GetUser(int32_t nUserID)
	{
		if (m_LoginUsers.find(nUserID) == m_LoginUsers.end())
			return nullptr;
		return m_LoginUsers.find(nUserID)->second;
	}
	std::shared_ptr<T> GetUserByConnectID(int32_t nConnectID)
	{
		for (auto itUser : m_LoginUsers)
		{
			if (itUser.second->GetConnectID() == nConnectID)
			{
				return itUser.second;
			}
		}
		
		return nullptr;
	}

	template<typename TIdentify>
	std::shared_ptr<T> GetUserByIdentify(UserIdentifyType type,const TIdentify identify)
	{
		for (auto itUser : m_LoginUsers)
		{
			if (itUser.second->IsUser(type,identify))
				return itUser.second;
		}

		return nullptr;
	}
	std::shared_ptr<T> AddUser(int32_t nUserID)
	{
		if (m_LoginUsers.find(nUserID) != m_LoginUsers.end())
			return m_LoginUsers[nUserID];
		return  m_LoginUsers[nUserID] = std::make_shared<T>(nUserID);
	}
	void RemoveUser(int32_t nUserID)
	{
		auto itUser = m_LoginUsers.find(nUserID);
		if (itUser == m_LoginUsers.end())
			return ;
		m_LoginUsers.erase(itUser);
	}
};

#endif