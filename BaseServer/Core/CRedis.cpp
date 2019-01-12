#include "CRedis.h"
#include <stdint.h>
#include <memory>
#include <string.h>
#include <iostream>
#include "CEasyLog.h"

CRedis::CRedis()
{
	m_pRedis = nullptr;
	m_pReply = nullptr;
	m_status = RedisStatus::REDIS_STATUS_CLOSED;
	memset(m_bufCmd, 0, sizeof(m_bufCmd));
	m_strFormatCmd.str("");
};
CRedis::~CRedis()
{
	//退出有异常，为什么？
	//if (m_pRedis) redisFree(m_pRedis);
};

bool CRedis::Connect(std::string ip, uint32_t port, std::string password)
{
	if (ip.empty()) return false;
	if (port == 0) return false;

	if (IsOnline()) return true;

#if defined(__LINUX__)
	struct timeval timeout = { 1, 500000 }; // 1.5 seconds
	m_pRedis = redisConnectWithTimeout(ip.c_str(), port, timeout);
#elif defined(__WINDOWS__)
	m_pRedis = redisConnect(ip.c_str(), port);
#endif

	if (m_pRedis->err)
	{
		CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
		return false;
	}
	if (!password.empty())
	{
		//授权
		if (!Auth(password))
		{
			CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
			return false;
		}
	}

	m_status = RedisStatus::REDIS_STATUS_CONECTED;
	return true;
};

bool CRedis::Auth(const std::string password)
{
	return ExecCommonString(std::string("auth " + password));
};
/*
命令执行结果的返回类型 int type; 
存储执行结果返回为整数 long long integer; 

字符串值的长度 size_t len; 
存储命令执行结果返回是字符串 char *str; 

返回结果是数组的大小 size_t elements; 
存储执行结果返回是数组 struct redisReply **element;

*/
/*
REDIS_REPLY_STRING == 1				返回值是字符串
REDIS_REPLY_ARRAY == 2				返回值是数组
REDIS_REPLY_INTEGER == 3			返回值为整数 long long
REDIS_REPLY_NIL==4					返回值为空表示执行结果为空。 
REDIS_REPLY_STATUS ==5				返回命令执行的状态
REDIS_REPLY_ERROR ==6				命令执行错误
*/
std::string CRedis::GetLastError()
{
	if (!m_pReply) return "";
	switch (m_pReply->type)
	{
	case REDIS_REPLY_NIL:
	case REDIS_REPLY_ERROR:
		if (m_pReply->len == 0)
		{
			std::string str("No describe error,errcode:");
			return  (str += (m_pReply->type + 0x30) );
		}
		else
			return std::string(m_pReply->str);
	}

	return "";
};
bool CRedis::IsError(redisReply* reply)
{
	if (!reply) return false;
	switch (reply->type)
	{
	case REDIS_REPLY_NIL:
	case REDIS_REPLY_ERROR:
		return false;
	}
	return true;
};
bool CRedis::IsArray(redisReply* reply)
{
	if (!reply) return false;
	return reply->type == REDIS_REPLY_ARRAY ? true : false;
};

template<>
void CRedis::FormatInputString(const int value)
{
	m_strFormatCmd << value <<" ";
};
template<>
void CRedis::FormatInputString(const int64_t value)
{
	m_strFormatCmd << value << " ";
};
template<>
void CRedis::FormatInputString(const uint64_t value)
{
	m_strFormatCmd << value << " ";
};
template<>
void CRedis::FormatInputString(const float value)
{
	m_strFormatCmd << value << " ";
};
template<>
void CRedis::FormatInputString(const double value)
{
	m_strFormatCmd << value << " ";
};
template<>
void CRedis::FormatInputString(const std::string str)
{
	m_strFormatCmd << str<<" ";
};
template<>
void CRedis::FormatInputString(const char* const str)
{
	m_strFormatCmd << str << " ";
};
bool CRedis::ExecCommonString(const std::string str)
{
	if (m_pReply) freeReplyObject(m_pReply);
	std::cout << "Execcmd:" << str << std::endl;
	sprintf(m_bufCmd, "%s\0", str.c_str());
	m_pReply = (redisReply *)redisCommand(m_pRedis, m_bufCmd);					//这样就OK
	//m_pReply = (redisReply *)redisCommand(m_pRedis, "%s\0", str.c_str());	//这种就返回错误，?
	if (!IsError(m_pReply))
	{
		m_strFormatCmd.str("");
		return false;
	}
	
	m_strFormatCmd.str("");
	return true;
};
template<>
int CRedis::GetReturn(redisReply* reply)
{
	if (reply->type == REDIS_REPLY_STRING)
		return ::atoi(m_pReply->str);
	else if (reply->type == REDIS_REPLY_INTEGER)
		return static_cast<int>(m_pReply->integer);

	CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
	return 0;
};
template<>
int64_t CRedis::GetReturn(redisReply* reply)
{
	if (reply->type == REDIS_REPLY_STRING)
		return ::atoll(reply->str);
	else if (reply->type == REDIS_REPLY_INTEGER)
		return static_cast<int64_t>(reply->integer);

	CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
	return 0;
};
template<>
std::string CRedis::GetReturn(redisReply* reply)
{
	if (reply->type == REDIS_REPLY_STRING)
		return std::string(reply->str);

	CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
	return "";
};
template<>
void CRedis::GetReturn(redisReply* reply, RedisStringPairsPtr pResult)
{
	if (reply->elements == 0) return;

	for (int i = 0; i < reply->elements - 1; i += 2)
	{
		if (reply->element[i]->type == REDIS_REPLY_STRING)
		{
			redisReply* pValue = reply->element[i + 1];
			if(pValue->type == REDIS_REPLY_STRING)
				pResult->emplace(reply->element[i]->str, pValue->str);
		}
	}
};
template<>
void CRedis::GetReturn(redisReply* reply, RedisStringKeysPtr pResult)
{
	for (size_t i = 0; i < reply->elements; i ++)
	{
		if (reply->element[i]->type == REDIS_REPLY_STRING)
		{
			pResult->emplace_back(reply->element[i]->str);
		}
	}
};

/*
template<>
void CRedis::GetReturn(redisReply* reply, RedisIntPairsPtr pResult)
{
	for (unsigned int i = 0; i < reply->elements - 1; i += 2)
	{
		if (reply->element[i]->type == REDIS_REPLY_STRING)
		{
			redisReply* pValue = reply->element[i + 1];
			if (pValue->type == REDIS_REPLY_INTEGER)
				pResult->emplace(reply->element[i]->str, pValue->integer);
		}
	}
};
*/
template<>
bool CRedis::Set(const std::string key, const int value)
{
	return ExecCommon(REDIS_SET,key, value);
};
template<>
bool CRedis::Set(const std::string key, const int64_t value)
{
	return ExecCommon(REDIS_SET, key, value);
};
template<>
bool CRedis::Set(const std::string key, const float value)
{
	return ExecCommon(REDIS_SET, key, value);
};
template<>
bool CRedis::Set(const std::string key, const std::string value)
{
	return ExecCommon(REDIS_SET, key, value);
};
template<>
std::tuple<bool, int> CRedis::Get(const std::string key)
{
	bool bRet = false;
	int result = 0;
	if (!ExecCommon(REDIS_GET, key))
	{
		return std::make_tuple<bool, int>((bool)bRet, (int)result);
	}
		
	bRet = true;
	result = GetReturn<decltype(result)>(m_pReply);
	return std::make_tuple<bool, int>((bool)bRet, (int)result);
};
template<>
std::tuple<bool, int64_t> CRedis::Get(const std::string key)
{
	bool bRet = false;
	int64_t result = 0;
	if (!ExecCommon(REDIS_GET, key))
	{
		return std::make_tuple<bool, int64_t>((bool)bRet, (int64_t)result);
	}

	bRet = true;
	result = GetReturn<decltype(result)>(m_pReply);
	return std::make_tuple<bool, int64_t>((bool)bRet, (int64_t)result);
};
template<>
std::tuple<bool, std::string> CRedis::Get(const std::string key)
{
	bool bRet = false;
	std::string result;
	if (!ExecCommon(REDIS_GET, key))
	{
		return std::make_tuple<bool, std::string>((bool)bRet, (std::string)result);
	}

	bRet = true;
	result = GetReturn<decltype(result)>(m_pReply);

	return std::make_tuple<bool, std::string>((bool)bRet, (std::string)result);
};

bool CRedis::Del(const std::string key)
{
	return ExecCommon(REIDS_DEL, key);
};

//hash table
template<>
bool CRedis::HSet(const std::string table, const std::string key, const int value)
{
	return ExecCommon(REDIS_HASH_SET, table, key, value);
};
template<>
bool CRedis::HSet(const std::string table, const std::string key, const int64_t value)
{
	return ExecCommon(REDIS_HASH_SET, table, key, value);
};
template<>
bool CRedis::HSet(const std::string table, const std::string key, const float value)
{
	return ExecCommon(REDIS_HASH_SET, table, key, value);
};
template<>
bool CRedis::HSet(const std::string table, const std::string key, const double value)
{
	return ExecCommon(REDIS_HASH_SET, table, key, value);
};
template<>
bool CRedis::HSet(const std::string table, const std::string key, const std::string value)
{
	return ExecCommon(REDIS_HASH_SET, table, key, value);
};
template<>
bool CRedis::HSet(const std::string table, const std::string key, const char* const value)
{
	return ExecCommon(REDIS_HASH_SET, table, key, value);
};

template<>
std::tuple<bool, int> CRedis::HGet(const std::string table, const std::string key)
{
	bool bRet = false;
	int result = 0;

	if (!ExecCommon(REDIS_HASH_GET, table, key))
	{
		return std::make_tuple<bool, int>((bool)bRet, (int)result);
	}

	bRet = true;
	result = GetReturn<decltype(result)>(m_pReply);
	return std::make_tuple<bool, int>((bool)bRet, (int)result);
};
template<>
std::tuple<bool, int64_t> CRedis::HGet(const std::string table, const std::string key)
{
	bool bRet = false;
	int64_t result = 0;

	if (!ExecCommon(REDIS_HASH_GET, table, key))
	{
		return std::make_tuple<bool, int64_t>((bool)bRet, (int64_t)result);
	}

	bRet = true;
	result = GetReturn<decltype(result)>(m_pReply);
	return std::make_tuple<bool, int64_t>((bool)bRet, (int64_t)result);
};
template<>
std::tuple<bool, std::string> CRedis::HGet(const std::string table, const std::string key)
{
	bool bRet = false;
	std::string result("");

	if (!ExecCommon(REDIS_HASH_GET, table, key))
	{
		return std::make_tuple<bool, std::string>((bool)bRet, (std::string)result);
	}

	bRet = true;
	result = GetReturn<decltype(result)>(m_pReply);
	return std::make_tuple<bool, std::string>((bool)bRet, (std::string)result);
};
template<>
std::tuple<bool, RedisStringPairsPtr> CRedis::HGetAll(const std::string table)
{
	bool bRet = false;
	RedisStringPairsPtr result = std::make_shared<StringPairs>();

	if (!ExecCommon(REDIS_HASH_GETALL, table) || !IsArray(m_pReply))
	{
		return std::make_tuple<bool, RedisStringPairsPtr>((bool)bRet, (RedisStringPairsPtr)result);
	}

	bRet = true;
	GetReturn<decltype(result)>(m_pReply, result);
	return std::make_tuple<bool, RedisStringPairsPtr>((bool)bRet, (RedisStringPairsPtr)result);
};
/*  不存在string - int 键值对,intenger类型是给命令结果使用的
template<>
std::tuple<bool, RedisIntPairsPtr> CRedis::HGetAll(const std::string table)
{
	bool bRet = false;
	RedisIntPairsPtr result = std::make_shared<IntPairs>();

	if (!ExecCommon(REDIS_HASH_GETALL, table) || !IsArray(m_pReply))
	{
		return std::make_tuple<bool, RedisIntPairsPtr>((bool)bRet, (RedisIntPairsPtr)result);
	}

	bRet = true;
	GetReturn<decltype(result)>(m_pReply, result);
	return std::make_tuple<bool, RedisIntPairsPtr>((bool)bRet, (RedisIntPairsPtr)result);
};
template<>
std::tuple<bool, RedisStringPairsPtr, RedisIntPairsPtr> CRedis::HGetAll(const std::string table)
{
	bool bRet = false;
	RedisStringPairsPtr result_string = std::make_shared<StringPairs>();
	RedisIntPairsPtr result_int = std::make_shared<IntPairs>();

	if (!ExecCommon(REDIS_HASH_GETALL, table) || !IsArray(m_pReply))
	{
		return std::make_tuple<bool, RedisStringPairsPtr, RedisIntPairsPtr>((bool)bRet, (RedisStringPairsPtr)result_string,(RedisIntPairsPtr)result_int);
	}

	bRet = true;
	GetReturn<decltype(result_string)>(m_pReply, result_string);
	GetReturn<decltype(result_int)>(m_pReply, result_int);
	return std::make_tuple<bool, RedisStringPairsPtr, RedisIntPairsPtr>((bool)bRet, (RedisStringPairsPtr)result_string,(RedisIntPairsPtr)result_int);
};
*/
bool CRedis::HDel(const std::string table,const std::string key)
{
	return ExecCommon(REDIS_HASH_DEL, table, key);
};
std::tuple<bool, int64_t> CRedis::Hincrby(const std::string table, const std::string key, const int64_t value)
{
	bool bRet = false;
	int64_t nNewValue = 0;
	if (!ExecCommon(REDIS_HASH_HINCRBY, table, key, value))
		return std::make_tuple<bool,int64_t>((bool)bRet,(int64_t)nNewValue);

	bRet = true;
	nNewValue = GetReturn<int64_t>(m_pReply);
	return std::make_tuple<bool, int64_t>((bool)bRet, (int64_t)nNewValue);
};
//list
int  CRedis::ListLLen(const std::string list)
{
	if (!ExecCommon(REDIS_LIST_LLEN, list))
		return 0;
	return GetReturn<int>(m_pReply);
};
int CRedis::ListLRem(const std::string list, const std::string value, int nCount)
{
	if (!ExecCommon(REDIS_LIST_LREM, list, nCount, value))
	{
		return 0;
	}

	return GetReturn<int>(m_pReply);
};
std::tuple<bool, std::string> CRedis::ListRPopLPush(const std::string srcList, const std::string dstList)
{
	bool bRet = false;
	std::string result("");

	if (!ExecCommon(REDIS_LIST_RPOPLPUSH, srcList, dstList))
		return std::make_tuple<bool, std::string >((bool)bRet,(std::string)result) ;

	bRet = true;
	result = GetReturn<std::string>(m_pReply);
	return std::make_tuple<bool, std::string >((bool)bRet, (std::string)result);
};
std::tuple<bool, std::string> CRedis::ListLPop(const std::string list)
{
	bool bRet = false;
	std::string result("");

	if (!ExecCommon(REDIS_LIST_LPOP, list))
		return std::make_tuple<bool, std::string >((bool)bRet, (std::string)result);

	bRet = true;
	result = GetReturn<std::string>(m_pReply);
	return std::make_tuple<bool, std::string >((bool)bRet, (std::string)result);
};
std::tuple<bool, std::string> CRedis::ListRPop(const std::string list)
{
	bool bRet = false;
	std::string result("");

	if (!ExecCommon(REDIS_LIST_RPOP, list))
		return std::make_tuple<bool, std::string >((bool)bRet, (std::string)result);

	bRet = true;
	result = GetReturn<std::string>(m_pReply);
	return std::make_tuple<bool, std::string >((bool)bRet, (std::string)result);
};

//set
int CRedis::SetSCard(const std::string set)
{
	if (!ExecCommon(REDIS_SET_SCARD, set))
		return 0;

	return GetReturn<int>(m_pReply);
};
bool CRedis::SetIsMemeber(const std::string set, const std::string value)
{
	if (!ExecCommon(REDIS_SET_SISMEMBER, set, value))
		return false;
	return GetReturn<int>(m_pReply) == 1 ? true : false;
};
std::tuple<bool, RedisStringKeysPtr> CRedis::SetSMembers(const std::string set)
{
	bool bRet = false;
	RedisStringKeysPtr result = std::make_shared<StringKey>();

	if (!ExecCommon(REDIS_SET_SMEMBERS, set))
		return std::make_tuple<bool, RedisStringKeysPtr>((bool)bRet,(RedisStringKeysPtr)result);

	bRet = true;
	GetReturn<RedisStringKeysPtr>(m_pReply, result);
	return std::make_tuple<bool, RedisStringKeysPtr>((bool)bRet, (RedisStringKeysPtr)result);
};


//杂项命令
//TYPE
std::string CRedis::Type(const std::string key)
{
	if (!ExecCommon(REDIS_TYPE, key))
		return "";

	return GetReturn < std::string >(m_pReply);
};
//RENAME
bool CRedis::Rename(const std::string oldKey, const std::string newKey)
{
	if (!ExecCommon(REDIS_RENAME, oldKey, newKey))
		return false;

	return true;
};
//EXISTS
bool CRedis::Exists(const std::string key) 
{
	if (!ExecCommon(REDIS_EXISTS, key))
		return false;

	return true;
};
//EXPIRE
bool CRedis::Expire(const std::string key, uint64_t time)
{
	if (!ExecCommon(REDIS_EXPIRE, key, time))
		return false;

	return GetReturn<int>(m_pReply) == 1 ? true : false;
};
//PEXPIRE
bool CRedis::PExpire(const std::string key, uint64_t time)
{
	if (!ExecCommon(REDIS_PEXPIRE, key, time))
		return false;

	return GetReturn<int>(m_pReply) == 1 ? true : false;
};
//PERSIST
bool CRedis::Persist(const std::string key)
{
	if (!ExecCommon(REDIS_PERSIST, key))
		return false;

	return GetReturn<int>(m_pReply) == 1 ? true : false;
};
//TTL
int CRedis::Ttl(const std::string key)
{
	if (!ExecCommon(REDIS_TTL, key))
		return -1;
	return GetReturn<int>(m_pReply);
};
//KEYS
std::tuple<bool, RedisStringKeysPtr> CRedis::Keys(const std::string key)
{
	bool bRet = false;
	RedisStringKeysPtr result = std::make_shared<StringKey>();
	if (!ExecCommon(REDIS_KEYS, key))
		return std::make_tuple<bool, RedisStringKeysPtr>((bool)bRet,(RedisStringKeysPtr)result);

	GetReturn<RedisStringKeysPtr>(m_pReply, result);
	return std::make_tuple<bool, RedisStringKeysPtr>((bool)bRet, (RedisStringKeysPtr)result);
};

//INCRBY
std::tuple<bool, int64_t> CRedis::Incrby(const std::string key, const int64_t value)
{
	bool bRet = false;
	int64_t nNewValue = 0;
	if (!ExecCommon(REDIS_INCRBY, key, value))
		return std::make_tuple<bool, int64_t>((bool)bRet, (int64_t)nNewValue);

	bRet = true;
	nNewValue = GetReturn<int64_t>(m_pReply);
	return std::make_tuple<bool, int64_t>((bool)bRet, (int64_t)nNewValue);
};