#include "CConfig.h"



CConfig::CConfig()
{
	//����
	bindPort = 8600;
	registerIP = "127.0.0.1";
	registerPort = 0;
}


CConfig::~CConfig()
{
}

bool CConfig::Init()
{
	return true;
}
