#include "CConfig.h"



CConfig::CConfig()
{
	//≤‚ ‘
	bindPort = 4444;
	registerIP = "127.0.0.1";
	registerPort = 6666;
}


CConfig::~CConfig()
{
}

bool CConfig::Init()
{
	return true;
}
