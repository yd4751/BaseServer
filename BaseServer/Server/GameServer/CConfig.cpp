#include "CConfig.h"


CConfig::CConfig()
{
	//≤‚ ‘
	bindPort = 8300;
	registerIP = "127.0.0.1";
	registerPort = 8800;
	cacheServer.ip = "127.0.0.1";
	cacheServer.port = 8888;
}


CConfig::~CConfig()
{
}

bool CConfig::Init()
{
	return true;
}
