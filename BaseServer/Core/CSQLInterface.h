#ifndef __H_SQLINTERFACE_H__
#define __H_SQLINTERFACE_H__
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <iostream>
#include <mysql.h>

enum SQLConnectStatus
{
	SQL_STATUS_CLOSE = 0,
	SQL_STATUS_CONNECT,
};
typedef struct SQLResultInfo
{
	//
	int32_t			nRowCount;		//行
	MYSQL_ROW		pRows;			//结果集
	//字段信息
	int32_t			nClomun;		//列
	MYSQL_FIELD*	pClomunNames;	//列名
	//DELETE, INSERT , REPLACE , UPDATE  执行结果判断
	int64_t			nAffectRows;	//影响行数
	void Rest()
	{
		nRowCount = 0;
		//
		nClomun = 0;
		pClomunNames = nullptr;
		pRows = nullptr;
		//
		nAffectRows = -1;
	}
}SQLResultInfo;

typedef std::vector<std::map<std::string, std::string>>		ReturnData;

class CSQLInterface
{
	MYSQL						m_pSql;

	MYSQL_RES*					m_querryResult;
	SQLResultInfo				m_resultInfo;
	//
	int32_t						m_returnMaxRow;
	ReturnData					m_returnData;
	//
	SQLConnectStatus			m_status;

public:
	CSQLInterface();
	virtual ~CSQLInterface();


protected:
	void ExecSQLComond(const std::string& sql);
	void SetMaxRowGet(int32_t nMax) { m_returnMaxRow = nMax; }
	void SaveData();
	bool CheckReturn();
	void DumpResult();

public:
	bool Connect(std::string host, std::string user , std::string password, std::string dbname,int32_t port)
	{
		//判断重复连接
		if (m_status == SQL_STATUS_CONNECT)
			return true;

		try
		{
			if (!mysql_real_connect(&m_pSql, host.c_str(), user.c_str(), password.c_str(), dbname.c_str(), port, NULL, 0))
				throw std::string("connect db is error!");

		}
		catch (std::string &error)
		{
			std::cout << __FUNCTION__ << ":" << error <<":"<< mysql_errno(&m_pSql)<<std::endl;
			return false;
		}

		m_status = SQL_STATUS_CONNECT;
		
		return true;
	}
	bool SelectDataBase(std::string dbname)
	{
		if (0 != mysql_select_db(&m_pSql, dbname.c_str()))
		{
			std::cout << "select " << dbname << " fail!" << std::endl;
			return false;
		}
		return true;
	}
	void Disconnect()
	{

		if(m_status != SQL_STATUS_CONNECT)
			return;
		try
		{
			mysql_close(&m_pSql);
		}
		catch (std::string error)
		{
			std::cout << std::string(mysql_error(&m_pSql)) << std::endl;
		}

		m_status = SQL_STATUS_CLOSE;
	}

public:
	bool DoSql(std::string sql,int32_t nMaxReturn = 0);
	const ReturnData& GetData() { return m_returnData; };
};

#endif