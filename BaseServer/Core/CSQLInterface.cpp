#include "CSQLInterface.h"



CSQLInterface::CSQLInterface()
{
	m_status = SQL_STATUS_CLOSE;
	m_returnMaxRow = 0;
	m_querryResult = nullptr;
	m_resultInfo.Rest();

	try
	{
		if (!mysql_init(&m_pSql))
			throw std::string("init mysql error");
	}
	catch (std::string &error)
	{
		std::cout << __FUNCTION__ << ":" << error << std::endl;
	}

	//设置自动重连
	int32_t nAutoConnect = 1;
	mysql_options(&m_pSql, MYSQL_OPT_RECONNECT, &nAutoConnect);
}


CSQLInterface::~CSQLInterface()
{
	Disconnect();
}

void CSQLInterface::ExecSQLComond(const std::string& sql)
{
	m_resultInfo.Rest();
	if (0 != mysql_query(&m_pSql, sql.c_str()))
	{
		//错误处理
		switch (mysql_errno(&m_pSql))
		{
		//case CR_SERVER_LOST:
		//	std::cout << "SQL is offline , auto reconnect!" << std::endl;
		//	break;
		default:
			std::cout << "SQL is offline , auto reconnect!" << "Error:"<< std::string(mysql_error(&m_pSql))<<std::endl;
			break;
		}
	}
};
bool CSQLInterface::CheckReturn()
{
	m_returnData.clear();
	//不判断结果
	if (m_returnMaxRow < 0)
	{
		return true;
	}
	//有影响行数的不需要取结果
	if(0 == m_returnMaxRow)
	{
		m_resultInfo.nAffectRows = mysql_affected_rows(&m_pSql);
		if (m_resultInfo.nAffectRows >= 0)
		{
			std::cout << "affect rows:" << m_resultInfo.nAffectRows << std::endl;
			return true;
		}
	}

	try
	{
		m_querryResult = mysql_store_result(&m_pSql);
		if (!m_querryResult)
		{
			throw std::string(mysql_error(&m_pSql));
		}
		//获取字段数量
		m_resultInfo.nClomun = mysql_num_fields(m_querryResult);
		if (0 == m_resultInfo.nClomun)
			throw std::string(mysql_error(&m_pSql));

		//获取字段名
		m_resultInfo.pClomunNames = mysql_fetch_fields(m_querryResult);
		if (!m_resultInfo.pClomunNames)
			throw std::string(mysql_error(&m_pSql));

		//结果行数
		m_resultInfo.nRowCount = mysql_num_rows(m_querryResult);
		if(0 == m_resultInfo.nRowCount)
			throw std::string(mysql_error(&m_pSql));

	}
	catch (std::string& error)
	{
		if (error.empty())	
			error = "no result!";

		std::cout << error << std::endl;
		return false;
	}

	SaveData();

	//DumpResult();
	//释放结果
	mysql_free_result(m_querryResult);
	return true;
};
void CSQLInterface::SaveData()
{
	int32_t nMax = m_returnMaxRow;
	while (--nMax >= 0 && (m_resultInfo.pRows = mysql_fetch_row(m_querryResult)) != nullptr)
	{
		m_returnData.emplace_back();
		for (int i = 0; i < m_resultInfo.nClomun; i++)
		{
			m_returnData[m_returnData.size() - 1].emplace(m_resultInfo.pClomunNames[i].name, m_resultInfo.pRows[i] == nullptr ? "" : m_resultInfo.pRows[i]);
		}
	}
};
bool CSQLInterface::DoSql(std::string sql, int32_t nMaxReturn /*= 0*/)
{
	SetMaxRowGet(nMaxReturn);
	ExecSQLComond(sql);
	return CheckReturn();
};

void CSQLInterface::DumpResult()
{
	//输出列名
	std::ostringstream info;
	
	for (int i = 0; i < m_resultInfo.nClomun; i++)
	{
		info << m_resultInfo.pClomunNames[i].name << "  ";
	}
	info << "\n";
	std::cout << info.str();

	int32_t nMax = m_returnMaxRow;
	while ( (m_resultInfo.pRows = mysql_fetch_row(m_querryResult)) != nullptr && --nMax >= 0)
	{
		info.str("");
		for (int i = 0; i < m_resultInfo.nClomun; i++)
		{
			info << m_resultInfo.pRows[i] << "  ";
		}
		info << "\n";
		std::cout << info.str();
	}
};