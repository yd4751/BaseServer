#ifndef __H_SCRIPT_INTERFACE_H__
#define __H_SCRIPT_INTERFACE_H__
#include "global_include.h"
#include "CEasyLog.h"
#include "CLoginHall.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

/*
//与业务逻辑层交互接口
logic -> loginhall : UpdatePlaying()
logic -> loginhall : UpdateExited()
logic -> loginhall : UpdateUserInfo()   //json或类似map数据结构
logic -> loginhall : SendData()

loginhall -> logic : Disconnect()
loginhall -> logic : Reconnect()
loginhall -> logic : UpdateUserInfo()   //json或类似map数据结构
loginhall -> logic : OnMessage()
*/
extern lua_State* L;

class CScriptInterface:
	public CSingleton<CScriptInterface>
{

public:
	
	void InitScript();

	
public:
	CScriptInterface()
	{
	}
	~CScriptInterface()
	{
		lua_close(L);
	}

	//注册到lua
public:
	static int UpdatePlaying(lua_State* L)
	{
		int uid = (int)lua_tonumber(L, -1);
		CLoginHall::GetInstance()->UpdatePlaying(uid);
		return 0;
	}
	static int	UpdateExited(lua_State* L)
	{
		int uid = (int)lua_tonumber(L, -1);
		CLoginHall::GetInstance()->UpdateExited(uid);
		return 0;
	}
	static int UpdateUserInfo(lua_State* L)
	{
		int uid = (int)lua_tonumber(L, -1);
		std::string info = lua_tostring(L, -2);
		CLoginHall::GetInstance()->UpdateUserInfo(uid,info);
		return 0;
	}
	static int SendData(lua_State* L)
	{
		int uid = (int)lua_tonumber(L, 1);
		int cmd = (int)lua_tonumber(L, 2);
		std::string message = lua_tostring(L, 3);
		CLoginHall::GetInstance()->SendData(uid, cmd, message);
		return 1;
	}
	static int Log(lua_State* L)
	{
		std::string str = lua_tostring(L, 1);
		CEasylog::GetInstance()->info("[Script]", str);
		return 0;
	}

	//c++调用lua
public:
	static void HandlerError(lua_State* L)
	{
		CEasylog::GetInstance()->error(lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	static bool Dispatch(int uid,int cmd, std::string message)
	{
		lua_getglobal(L, "OnMessage");
		lua_pushnumber(L, uid);
		lua_pushnumber(L, cmd);
		bool bRet = false;
		if (!message.empty())
		{
			lua_pushstring(L, message.c_str());
			bRet = lua_pcall(L, 3, 1, 0);
		}
		else
		{
			bRet = lua_pcall(L, 2, 1, 0);
		}
		if (bRet != 0) 
			HandlerError(L);

		bRet = lua_toboolean(L, -1);
		return bRet;
	}
	static void Disconnect(int uid)
	{
		lua_getglobal(L, "OnDisconnect");
		lua_pushnumber(L, uid);
		if(lua_pcall(L, 1, 0, 0) != 0)
			HandlerError(L);
	}
	static void Reconnect(int uid)
	{
		lua_getglobal(L, "OnReconnect");
		lua_pushnumber(L, uid);
		if(lua_pcall(L, 1, 0, 0) != 0)
			HandlerError(L);
	}
	static void UpdateUserInfo(int uid, std::string info)
	{
		lua_getglobal(L, "OnUpdateUserInfo");
		lua_pushnumber(L, uid);
		lua_pushstring(L, info.data());
		if(lua_pcall(L, 1, 0, 0) != 0)
			HandlerError(L);
	}
};
#endif