#ifndef __H_COMMONDDEFINE_H__
#define __H_COMMONDDEFINE_H__

#include "LogSeverCommond.h"
#include "CacheSeverCommond.h"
#include "DBSeverCommond.h"
#include "CenterSeverCommond.h"
#include "GateSeverCommond.h"
#include "LoginSeverCommond.h"
#include "GameSeverCommond.h"

enum ServerType
{
	SERVER_INVALID = 0,
	SERVER_GATE,
	SERVER_CENTER,
	SERVER_CACHE,
	SERVER_DB,
	SERVER_LOG,
	SERVER_LOGIN,
	SERVER_GAME,
};
#endif