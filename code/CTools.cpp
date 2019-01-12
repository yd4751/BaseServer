#include "CTools.h"
#include <stdint.h>
#include <float.h>
#include <time.h>
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace Tools
{
	//�ŵ��������У����������õõ������������ͬ��
	std::default_random_engine random(static_cast<uint32_t>(time(NULL)) );

	template<>
	int GetRandom(int nMin, int nMax)//ȫ�պ�����
	{
		if (nMax <= nMin) return nMax;
		//std::default_random_engine random(time(NULL));
		std::uniform_int_distribution<int> dis(nMin, nMax);
		return dis(random);
	};

	//������ò�Ҫ�������Ƹ��ʣ������漰����������
	template<>
	double GetRandom(double nMin, double nMax)//ȫ�պ�����
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
};