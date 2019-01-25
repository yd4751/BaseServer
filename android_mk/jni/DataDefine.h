#ifndef __H_NETCORE_DATA_DEFINE_H__
#define __H_NETCORE_DATA_DEFINE_H__
#include <stdint.h>

//#include <assert.h>
#include <memory>
#include <sstream>
#include <string>
#include <fstream>

#include <map>
#include <vector>
#include <list>
#include <tuple>
#include <mutex>

#include "CSingleton.h"
#include "CArray.h"
#include "CTools.h"

#include "CNetCoreInterface.h"
#include "CBaseConnection.h"
#include "CBaseWorker.h"
#include "CNodeManager.h"
#include "CSocketInterface.h"

/////////////////////////////////////////
//逻辑层维持单线程锁
extern std::mutex			m_netEventlock;
/////////////////////////////////////////
#endif