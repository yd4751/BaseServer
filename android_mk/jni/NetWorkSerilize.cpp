#include "NetWorkSerilize.h"


namespace NetWorkSerilize
{
	template<>
	std::string SerilizeUint(uint16_t value, bool littleend)
	{
		uint8_t* p = (uint8_t*)(&value);
		std::string str;
		if (littleend)
		{
			str += p[0];
			str += p[1];
		}
		else
		{
			str += p[1];
			str += p[0];
		}
		return str;
	};
	template<>
	std::string SerilizeUint(uint32_t value, bool littleend)
	{
		uint8_t* p = (uint8_t*)(&value);
		std::string str;
		if (littleend)
		{
			str += p[0];
			str += p[1];
			str += p[2];
			str += p[3];
		}
		else
		{
			str += p[3];
			str += p[2];
			str += p[1];
			str += p[0];
		}
		return str;
	};

	template<>
	uint16_t DeSerilizeUint(CBuffer& in, bool littleend)
	{
		uint16_t value = 0;
		char* p = (char*)(&value);
		if (littleend)
		{
			in.get(p[0]);
			in.get(p[1]);
		}
		else
		{
			in.get(p[1]);
			in.get(p[0]);
		}

		return value;
	};

	template<>
	uint32_t DeSerilizeUint(CBuffer& in, bool littleend)
	{
		uint32_t value = 0;
		char* p = (char*)(&value);
		if (littleend)
		{
			in.get(p[0]);
			in.get(p[1]);
			in.get(p[2]);
			in.get(p[3]);
		}
		else
		{
			in.get(p[3]);
			in.get(p[2]);
			in.get(p[1]);
			in.get(p[0]);
		}

		return value;
	};
}