#ifndef __H_NETWORK_SERILIZE_H__
#define __H_NETWORK_SERILIZE_H__

#include <string>
#include "CBuffer.h"
class CBuffer;

namespace NetWorkSerilize
{
	//ģ������ �� ���Ի�
	template<typename T>
	std::string SerilizeUint(T value, bool littleend = true);

	template<typename T>
	T DeSerilizeUint(CBuffer& in, bool littleend = true);
};

#endif
