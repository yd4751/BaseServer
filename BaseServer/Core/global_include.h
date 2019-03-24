#ifndef __H_GLOBAL_INCLUDE_H__
#define __H_GLOBAL_INCLUDE_H__

//json
#include <json/json.h>

#include <vector>
#include <map>
#include <iostream>
#include <chrono>
#include <memory>
#include <functional>
#include <string>

namespace NetCore
{
#include "CNetCoreInterface.h"
};

#include "CBuffer.h"
#include "CommondDefine.h"
#include "CMessageHandler.h"
#include "message_define.h"
#include "CTools.h"
#include "CArray.h"
#include "CEasyLog.h"
#include "CNodeManager.h"
#include "JsonParse.h"

using std::cout;
using std::endl;


#endif
