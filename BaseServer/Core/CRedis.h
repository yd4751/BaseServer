#ifndef __H_REDIS_INTERFACE_H__
#define __H_REDIS_INTERFACE_H__
#include <string>
#include <hiredis.h>
#define NO_QFORKIMPL //这一行必须加才能正常使用
//#include <Win32InterOp/win32fixes.h>
#include <tuple>
#include <sstream>
#include <map>
#include <vector>
#include <memory>

using std::ostringstream;

#define REDIS_CMD_BUF_LEN_MAX			2048
//
#define REDIS_SET							"set"
#define REDIS_MSET							"mset"
#define REDIS_GET							"get"
#define REIDS_DEL							"del"
#define REDIS_INCRBY						"incrby"
//
#define REDIS_TYPE							"type"
#define REDIS_RENAME						"rename"
#define REDIS_EXISTS						"exists"
#define REDIS_EXPIRE						"expire"
#define REDIS_PEXPIRE						"pexpire"
#define REDIS_PERSIST						"persist"
#define REDIS_TTL							"ttl"
#define REDIS_KEYS							"keys"
//HASH
#define REDIS_HASH_SET						"hset"
#define REDIS_HASH_MSET						"hmset"
#define REDIS_HASH_GET						"hget"
#define REDIS_HASH_GETALL					"hgetall"
#define REDIS_HASH_HINCRBY					"hincrby"
#define REDIS_HASH_DEL						"hdel"
//LIST
#define REDIS_LIST_LPUSH					"lpush"
#define REDIS_LIST_LPOP						"lpop"
#define REDIS_LIST_RPUSH					"rpush"
#define REDIS_LIST_RPOP						"rpop"
#define REDIS_LIST_RPOPLPUSH				"rpoplpush"
#define REDIS_LIST_LLEN						"llen"
#define REDIS_LIST_LREM						"lrem"
//SET
#define REDIS_SET_SCARD						"scard"
#define REDIS_SET_SADD						"sadd"
#define REDIS_SET_SREM						"srem"
#define REDIS_SET_SISMEMBER					"sismember"
#define REDIS_SET_SMEMBERS					"smembers"



typedef std::map<std::string, std::string>			StringPairs;
//typedef std::map<std::string, int64_t>				IntPairs;
typedef std::shared_ptr<StringPairs>				RedisStringPairsPtr;		//string  -  string
//typedef std::shared_ptr<IntPairs>					RedisIntPairsPtr;			//string  -  int

typedef std::vector<std::string>					StringKey;
typedef std::shared_ptr<StringKey>					RedisStringKeysPtr;

enum RedisStatus
{
	REDIS_STATUS_CLOSED = 0,
	REDIS_STATUS_CONECTED,
};

class CRedis
{
	redisContext*					m_pRedis;
	redisReply*						m_pReply;
	char							m_bufCmd[REDIS_CMD_BUF_LEN_MAX];
	ostringstream					m_strFormatCmd;

	RedisStatus						m_status;

public:
	CRedis();
	~CRedis();

public:
	bool IsOnline() { return m_status == RedisStatus::REDIS_STATUS_CONECTED ? true : false; }
	bool Connect(std::string ip, uint32_t port, std::string password);

protected:
	bool Auth(const std::string password);
	bool IsError(redisReply*);
	bool IsArray(redisReply*);

	template<typename T>
	void FormatInputString(const T);

	template<typename T>
	bool ExecCommon(const T input)
	{
		FormatInputString(input);
		return true;
	};
	template<typename T, typename... Rest>
	bool ExecCommon(const T first, const Rest... rest)
	{
		if (sizeof...(rest) == 1)
		{
			ExecCommon(first);
			ExecCommon(rest...);
			return ExecCommonString(m_strFormatCmd.str());
		}
		ExecCommon(first);

		return ExecCommon(rest...);
	};

	template<typename T>
	T GetReturn(redisReply* reply);
	template<typename T>
	void GetReturn(redisReply* reply,T);

public:
	std::string GetLastError();
	//bool
	bool ExecCommonString(const std::string str);

	//field  key
	template<typename T>
	bool Set(const std::string key, const T value);

	template<typename T , typename... Reset>
	bool MSet(const T first, const Reset... rest)
	{
		m_strFormatCmd.str("");
		FormatInputString(REDIS_MSET);
		if (sizeof...(rest) == 0)
		{
			FormatInputString(first);
			return ExecCommonString(m_strFormatCmd.str());
		}
		else
		{
			return ExecCommon(first, rest...);
		}
	};
	template<typename T>
	std::tuple<bool, T> Get(const std::string key);
	
	bool Del(const std::string key);
	
	//hash table
	std::tuple<bool,int64_t> Hincrby(const std::string table, const std::string key, const int64_t value);
	template<typename T>
	bool HSet(const std::string table, const std::string key,const T value);
	template<typename T, typename... Reset>
	bool HMSet(const std::string table,const T first, const Reset... rest)
	{
		m_strFormatCmd.str("");
		FormatInputString(REDIS_HASH_MSET);
		FormatInputString(table);
		if (sizeof...(rest) == 0)
		{
			FormatInputString(first);
			return ExecCommonString(m_strFormatCmd.str());
		}
		else
		{
			return ExecCommon(first, rest...);
		}
	};

	template<typename T>
	std::tuple<bool, T> HGet(const std::string table, const std::string key);
	template<typename T>
	std::tuple<bool, T> HGetAll(const std::string table);
	//template<typename T1,typename T2>
	//std::tuple<bool, T1, T2> HGetAll(const std::string table);
	bool HDel(const std::string table, const std::string key);


	//list
	int ListLLen(const std::string list);
	int ListLRem(const std::string list, const std::string value, int nCount = -1); //按redis风格，-1表示删除一个元素
	std::tuple<bool, std::string> ListRPopLPush(const std::string srcList, const std::string dstList);
	std::tuple<bool, std::string> ListLPop(const std::string list);
	std::tuple<bool, std::string> ListRPop(const std::string list);
	template<typename T,typename... Rest>
	bool ListLPush(const std::string list, T first, Rest... rest)
	{
		m_strFormatCmd.str("");
		FormatInputString(REDIS_LIST_LPUSH);
		FormatInputString(list);
		if (sizeof...(rest) == 0)
		{
			FormatInputString(first);
			return ExecCommonString(m_strFormatCmd.str());
		}
		else
		{
			return ExecCommon(first, rest...);
		}
	};
	template<typename T, typename... Rest>
	bool ListRPush(const std::string list, T first, Rest... rest)
	{
		m_strFormatCmd.str("");
		FormatInputString(REDIS_LIST_RPUSH);
		FormatInputString(list);
		if (sizeof...(rest) == 0)
		{
			FormatInputString(first);
			return ExecCommonString(m_strFormatCmd.str());
		}
		else
		{
			return ExecCommon(first, rest...);
		}
	};

	//set
	int SetSCard(const std::string set);	//获取成员数
	bool SetIsMemeber(const std::string set, const std::string value);
	std::tuple<bool, RedisStringKeysPtr> SetSMembers(const std::string set);
	template <typename T,typename... Rest>
	int SetSRem(const std::string set, T first, Rest... rest)
	{
		bool bRet = false;
		m_strFormatCmd.str("");
		FormatInputString(REDIS_SET_SREM);
		FormatInputString(set);
		if (sizeof...(rest) == 0)
		{
			FormatInputString(first);
			bRet = ExecCommonString(m_strFormatCmd.str());
		}
		else
		{
			bRet = ExecCommon(first, rest...);
		}

		if (!bRet) return 0;

		return GetReturn<int>(m_pReply);
	};
	template <typename T, typename... Rest>
	int SetSAdd(const std::string set, T first, Rest... rest)
	{
		bool bRet = false;
		m_strFormatCmd.str("");
		FormatInputString(REDIS_SET_SADD);
		FormatInputString(set);
		if (sizeof...(rest) == 0)
		{
			FormatInputString(first);
			bRet = ExecCommonString(m_strFormatCmd.str());
		}
		else
		{
			bRet = ExecCommon(first, rest...);
		}

		if (!bRet) return 0;

		return GetReturn<int>(m_pReply);
	};

	//暂不加入
	//sort set
	//...

	//事务  mutil  set  get sadd  smembers exec
	//...
	//杂项命令
	//TYPE
	std::string Type(const std::string key);
	//RENAME
	bool Rename(const std::string oldKey, const std::string newKey);
	//EXISTS
	bool Exists(const std::string key);
	//EXPIRE
	bool Expire(const std::string key, uint64_t time);
	//PEXPIRE
	bool PExpire(const std::string key, uint64_t time);
	//PERSIST
	bool Persist(const std::string key);
	//TTL	当 key 不存在时，返回 - 2 。 当 key 存在但没有设置剩余生存时间时，返回 - 1 。 否则，以秒为单位，返回 key 的剩余生存时间。
	//注意：在 Redis 2.8 以前，当 key 不存在，或者 key 没有设置剩余生存时间时，命令都返回 - 1 。
	int Ttl(const std::string key);
	//KEYS
	std::tuple<bool,RedisStringKeysPtr> Keys(const std::string key);
	//INCRBY
	std::tuple<bool, int64_t> Incrby( const std::string key, const int64_t value);
	

};
#endif