#ifndef __H_C_TOOLS_H__
#define __H_C_TOOLS_H__
#include <string>

namespace Tools
{
	template<typename T>
	T GetRandom(T nMin, T nMax);

	int64_t GetCurSecond();
	int64_t GetCurMillisecond();

	std::string GetCurDate();
	std::string GetCurDateTime();
};
#endif