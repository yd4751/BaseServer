#include "CConfig.h"



CConfig::CConfig()
{
	//≤‚ ‘
	bindPort = 8800;
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
