#ifndef __H_BASE_USER_H__
#define __H_BASE_USER_H__
#include "global_include.h"

typedef enum UserStatus
{
	USER_STATUS_NULL = 0,
	USER_STATUS_LOGIN,
	USER_STATUS_DISCONNECT,
	USER_STATUS_MAX,
}UserStatus;

//��ʶΨһ��¼�����ֶΣ�
class CUesrIdentify
{
public:
	std::string			email;
	std::int32_t		phoneNumber;
	std::int32_t		userID;

public:
	CUesrIdentify()
	{
		email = "";
		phoneNumber = 0;
		userID = 0;
	}
	CUesrIdentify(std::string email, std::int32_t phoneNumber, std::int32_t userID)
	{
		this->email = email;
		this->phoneNumber = phoneNumber;
		this->userID = userID;
	}

public:
	bool operator<(const CUesrIdentify& right) const
	{
		//ֻƥ����һ��������ͬ(���ã����䡢id���ֻ��ŵȶ෽ʽ��¼)
		if (this->userID > 0 && this->userID == right.userID) return false;
		if (this->phoneNumber > 0 && this->phoneNumber == right.phoneNumber) return false;
		if (!this->email.empty() && this->email == right.email) return false;
		return this->userID < right.userID;
	}
};

class CBaseUser
{
	CUesrIdentify			m_identify;
	UserStatus				m_curStatus;
	int32_t					m_connectID;

public:
	CBaseUser();
	virtual ~CBaseUser();

protected:
	void SetStatus(UserStatus status)
	{
		m_curStatus = status;
	};

	void Reset()
	{
		m_curStatus = USER_STATUS_NULL;
		m_connectID = -1;
	}
public:
	static bool Send(int32_t nConnectID, int32_t nSendCmd, std::string& strSendData, NetCore::ProtocolType type = NetCore::ProtocolType::PROTO_TYPE_JSON)
	{
		return NetCore::SendData(nConnectID, nSendCmd, strSendData.c_str(), strSendData.length(), type);
	}
	bool Send(int32_t nSendCmd, std::string& strSendData, NetCore::ProtocolType type = NetCore::ProtocolType::PROTO_TYPE_JSON)
	{
		if (m_curStatus == USER_STATUS_NULL || m_curStatus == USER_STATUS_DISCONNECT)
			return false;

		return NetCore::SendData(m_connectID, nSendCmd, strSendData.c_str(), strSendData.length(), type);
	}

public:
	void Init(int32_t id)
	{
		SetStatus(USER_STATUS_LOGIN);
	};
	void BindConnectID(int32_t clientID)
	{
		m_connectID = clientID;
	}
	void DisBindConnectID()
	{
		m_connectID = -1;
	}
	void Offline()
	{
		DisBindConnectID();
		SetStatus(USER_STATUS_DISCONNECT);
	}

public:
	UserStatus GetStatus() { return m_curStatus; }
	int32_t	GetID() { return m_identify.userID; }
	int32_t GetConnectID() { return m_connectID; }
};

#endif