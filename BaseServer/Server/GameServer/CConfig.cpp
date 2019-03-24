#include "CConfig.h"



CConfig::CConfig()
{
	//≤‚ ‘
	bindPort = 3333;
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
