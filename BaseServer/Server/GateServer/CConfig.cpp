#include "CConfig.h"



CConfig::CConfig()
{
	//≤‚ ‘
	bindPort = 5555;
	registerIP = "127.0.0.1";
	registerPort = 6666;
	nAuthTime = 5000;
}


CConfig::~CConfig()
{
}

bool CConfig::Init()
{
	return true;
}
