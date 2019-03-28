#include "CConfig.h"



CConfig::CConfig()
{
	//≤‚ ‘
	bindPort = 8500;
	registerIP = "127.0.0.1";
	registerPort = 8800;
	nAuthTime = 50000;
}


CConfig::~CConfig()
{
}

bool CConfig::Init()
{
	return true;
}
