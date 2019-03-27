#ifndef __H_CUSER_H__
#define __H_CUSER_H__
#include <string>
#include <cstdint>

class CUser
{
public:
	uint32_t	id;
	uint32_t	sex;
	uint32_t	money;
	std::string nickName;
	std::string avatar;

public:
	CUser();
	virtual ~CUser();
};

#endif