#ifndef __H_BASE_USER_H__
#define __H_BASE_USER_H__
#include "global_include.h"

typedef enum UserStatus
{
	USER_STATUS_NULL = 0,
	USER_STATUS_LOGIN,
	USER_STATUS_DISCONNECT,
	USER_STATUS_PLAYING,
	USER_STATUS_EXITED,
	USER_STATUS_MAX,
}UserStatus;

//具体是什么登录方式由客户端识别
enum UserIdentifyType
{
	IDENTIFY_TYPE_NULL = 0,
	IDENTIFY_TYPE_ACCOUNT,
	IDENTIFY_TYPE_EMAIL,		
	IDENTIFY_TYPE_PHONE,
	IDENTIFY_TYPE_UID,
	IDENTIFY_TYPE_WCHAT,
	IDENTIFY_TYPE_QQ,
};
//标识唯一登录（多字段）
class CUesrIdentify
{
public:
	std::string			account;
	std::string			email;
	std::int32_t		phoneNumber;
	std::int32_t		userID;

public:
	CUesrIdentify()
	{
		account = "";
		email = "";
		phoneNumber = 0;
		userID = 0;
	}
	CUesrIdentify(std::string account,std::string email, std::int32_t phoneNumber, std::int32_t userID)
	{
		this->account = account;
		this->email = email;
		this->phoneNumber = phoneNumber;
		this->userID = userID;
	}

public:
	bool operator<(const CUesrIdentify& right) const
	{
		//只匹配上一个就是相同(作用：邮箱、id、手机号等多方式登录)
		if (this->userID > 0 && this->userID == right.userID) return false;
		if (this->phoneNumber > 0 && this->phoneNumber == right.phoneNumber) return false;
		if (!this->email.empty() && this->email == right.email) return false;
		return this->userID < right.userID;
	}
};

class CValueInfo
{
public:
	CValueInfo() 
	{
		strValue = "";
		intValue = 0;
	}
	template<typename T>
	CValueInfo(T value)
	{
		Set(value);
	}
public:
	enum ValueType
	{
		TYPE_INTEGER,
		TYPE_STRING,
	}	m_type;

	std::string		strValue;
	int64_t			intValue;

	void Set(const char* value)
	{
		strValue.assign(value);
		m_type = ValueType::TYPE_STRING;
	}
	void Set(const std::string value)
	{
		strValue = value;
		m_type = ValueType::TYPE_STRING;
	}
	void Set(int64_t value)
	{
		intValue = value;
		m_type = ValueType::TYPE_INTEGER;
	}

	std::string GetString() { return strValue; }
	int64_t GetInteger() { return intValue; }
};

class CBaseUser
{
	CUesrIdentify			m_identify;
	UserStatus				m_curStatus;
	int32_t					m_connectID;
	std::map<std::string, CValueInfo>		m_info;

public:
	CBaseUser();
	CBaseUser(int nUid) { 
		m_identify.userID = nUid; 
		m_info.clear();
	};
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
	void Init(int32_t id);
	void Init(int32_t userID, const std::map<std::string, std::string>& info);
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
	CUesrIdentify& GetUserIdentify() { return m_identify; }
	UserStatus GetStatus() { return m_curStatus; }
	int32_t	GetID() { return m_identify.userID; }
	int32_t GetConnectID() { return m_connectID; }
	void UpdateStatus(UserStatus status){	m_curStatus = status;	}
	
	bool IsUser(UserIdentifyType type, int32_t identify);
	bool IsUser(UserIdentifyType type, const std::string identify);

	std::map<std::string, CValueInfo> UpdateInfo(const std::map<std::string, CValueInfo>& info);
};

#endif