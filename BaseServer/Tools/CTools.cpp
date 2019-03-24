﻿#include "CTools.h"
#include <stdint.h>
#include <float.h>
#include <time.h>
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cassert>
#ifdef __WINDOWS__
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#elif __LINUX__
#include <netdb.h>
#endif
namespace Tools
{
	//放到函数体中，若持续调用得到的随机数是相同的
	std::default_random_engine random(static_cast<uint32_t>(time(NULL)) );

	template<>
	int GetRandom(int nMin, int nMax)//全闭合区间
	{
		if (nMax <= nMin) return nMax;
		//std::default_random_engine random(time(NULL));
		std::uniform_int_distribution<int> dis(nMin, nMax);
		return dis(random);
	};

	//这里最好不要用来控制概率，这里涉及到精度问题
	template<>
	double GetRandom(double nMin, double nMax)//全闭合区间
	{
		if (nMax <= nMin) return nMax;
		//std::default_random_engine random(time(NULL));
		std::uniform_real_distribution<double> dis(nMin, std::nextafter(nMax, DBL_MAX));
		return dis(random);
	};

	int64_t GetCurSecond()
	{
		std::chrono::seconds cur = std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()
			);
		return cur.count();
	}
	int64_t GetCurMillisecond()
	{
		std::chrono::milliseconds cur = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
			);
		return cur.count();
	}

	std::string GetCurDate()
	{
		auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::stringstream str;
		str << std::put_time(std::localtime(&t), "%Y-%m-%d");
		return str.str();
	};
	std::string GetCurDateTime()
	{
		auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::stringstream str;
		str << std::put_time(std::localtime(&t), "%Y-%m-%d %X");
		return str.str();
	};

	std::string	GetCurPath()
	{
		//待实现
		//...
		return "";
	};
	std::string GetLocalIP()
	{
		char hostname[256] = { 0 };
		gethostname(hostname, sizeof(hostname));
		struct hostent* hosts = gethostbyname(hostname);
		if (hosts == nullptr)
		{
			assert(false);
			return "";
		}

		int idx = 0;
		while (hosts->h_addr_list[idx] != nullptr)
		{
			//hosts->h_name
			return std::string(inet_ntoa(*(struct in_addr*)hosts->h_addr_list[idx]));
		}

		assert(false);
		return "";
	};
};