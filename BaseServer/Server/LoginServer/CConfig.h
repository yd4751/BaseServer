#ifndef __H_CONFIG_H__
#define _H_CONFIG_H__
#include "CConfigBase.h"

class CConfig:public CConfigBase
{
public:
	ServerConfig	cacheServer;
	ServerConfig	dbServer;

public:
	CConfig();
	virtual ~CConfig();
public:
	virtual bool Init();
};

#endif