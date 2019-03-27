#ifndef __H_CUSER_H__
#define __H_CUSER_H__
#include <string>
#include <cstdint>

class CUser
{
public:
	uint32_t fd = 0;
	uint32_t id = 0;
	uint32_t sex = 0;
	uint32_t money = 0;
	std::string nickName = "";
	std::string avatar = "";

public:
	CUser();
	virtual ~CUser();
};

#endif