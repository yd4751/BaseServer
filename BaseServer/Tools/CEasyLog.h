#ifndef __H_EASY_LOG_H__
#define __H_EASY_LOG_H__
//#include <assert.h>
#include <sstream>
#include <memory>
#include <fstream>
#include <string>
#include <mutex>
#include "CTools.h"
#include "CSingleton.h"
#include <iostream>

enum Loglvel
{
	LOGLEVEL_ERROR,
	LOGLEVEL_WARN,
	LOGLEVEL_INFO,
	LOGLEVEL_DEBUG,
};
class CEasylog:
	public CSingleton<CEasylog>
{
	std::fstream		m_file;
	std::ostringstream	m_inputString;
	std::mutex			m_lock;
	Loglvel				m_level;

public:

	CEasylog(){}
	~CEasylog()
	{
		m_file.close();
	}
	void Init(const std::string logFile = "netcore.log", Loglvel level = Loglvel::LOGLEVEL_DEBUG)
	{
		std::lock_guard<std::mutex>	gurd(m_lock);

		if (m_file.is_open()) return;
		m_level = level;
		m_file.open(logFile, std::ios::app);
		if (!m_file.is_open())
		{
			//abort();
		}
	}

protected:
	void Write()
	{
		std::cout << m_inputString.str();
		m_file.write(m_inputString.str().c_str(), m_inputString.str().length());
		//及时同步到文件
		m_file.sync();
	}
	template<typename T>
	void FormateLog(T param)
	{
		m_inputString << param;
	};
	void FormateHead()
	{
		m_inputString.str("");
		m_inputString << "[";
		m_inputString << Tools::GetCurDateTime() << " ." << Tools::GetCurMillisecond();
		m_inputString << "]";
	}
	void FormateTail()
	{
		m_inputString <<"\n";
		Write();
	}
	template<typename T>
	void Log(T first)
	{
		m_inputString <<" "<< first;
	}
	template<typename T, typename... Reset>
	void Log(T first, Reset... rest)
	{
		Log(first);
		Log(rest...);
	}
	template<typename T, typename... Reset>
	void log(T first, Reset... rest)
	{
		std::lock_guard<std::mutex>	gurd(m_lock);

		FormateHead();
		Log(first, rest...);
		FormateTail();
	}
public:
	template<typename... Reset>
	void error(Reset... rest)
	{
		if (m_level < Loglvel::LOGLEVEL_ERROR) return;
		log("[ERROR]",rest...);
	};
	template<typename... Reset>
	void warn(Reset... rest)
	{
		if (m_level < Loglvel::LOGLEVEL_WARN) return;
		log("[WARN]", rest...);
	};
	template<typename... Reset>
	void info(Reset... rest)
	{
		if (m_level < Loglvel::LOGLEVEL_INFO) return;
		log("[INFO]", rest...);
	};
	template<typename... Reset>
	void debug(Reset... rest)
	{
		if (m_level < Loglvel::LOGLEVEL_DEBUG) return;
		log("[DEBUG]", rest...);
	};
};
#endif