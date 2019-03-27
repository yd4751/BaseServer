#include "CConfig.h"



CConfig::CConfig()
{
	//≤‚ ‘
	bindPort = 8400;
	registerIP = "127.0.0.1";
	registerPort = 8800;
	cacheServer.ip = "127.0.0.1";
	cacheServer.port = 8888;
	dbServer.ip = "127.0.0.1";
	dbServer.port = 3306;
}


CConfig::~CConfig()
{
}

bool CConfig::Init()
{
	return true;
}
