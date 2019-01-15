#include "CScriptInterface.h"

lua_State* L;

void CScriptInterface::InitScript()
{
	L = luaL_newstate();
	luaL_openlibs(L);
	//当前脚本路径加入搜索路径
	std::string strPath = "Scripts";
	std::string strScript = "package.path = './" + strPath + "/?.lua;./?.lua'";
	luaL_dostring(L, strScript.c_str());

	//
	lua_register(L, "UpdatePlaying", CScriptInterface::UpdatePlaying);
	lua_register(L, "UpdateExited", CScriptInterface::UpdateExited);
	lua_register(L, "UpdateUserInfo", CScriptInterface::UpdateUserInfo);
	lua_register(L, "SendData", CScriptInterface::SendData);
	lua_register(L, "Log", CScriptInterface::Log);

	strPath = strPath + "/main.lua";
	if (luaL_dofile(L, strPath.c_str()) != 0)
	{
		CEasylog::GetInstance()->error("start script fail!");
		HandlerError(L);
		return;
	}

	lua_getglobal(L, "Init");
	if(lua_pcall(L, 0, 0, 0) != 0)
		HandlerError(L);
}