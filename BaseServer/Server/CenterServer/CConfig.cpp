#include "CConfig.h"



CConfig::CConfig()
{
	//≤‚ ‘
	bindPort = 6666;
	registerIP = "";
	registerPort = 0;
}


CConfig::~CConfig()
{
}

bool CConfig::Init()
{
	return true;
}
