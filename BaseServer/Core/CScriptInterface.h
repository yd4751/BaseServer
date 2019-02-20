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

/*
栈底   ->  栈顶
负数索引，不需要知道栈的大小，我们就能知道栈顶在哪，栈顶的索引永远是-1
正数索引，不需要知道栈的大小，我们就能知道栈底在哪，栈底的索引永远是1
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

public:
	static void DumpLuaState(lua_State* L)
	{
		int nTop = lua_gettop(L);
		std::cout << "Cur lua stack deep:" << nTop << std::endl;
		for (int i = nTop; i > 0; --i)
		{
			switch (lua_type(L, i))
			{
			case LUA_TSTRING:
				std::cout << lua_tostring(L, i) << std::endl;
				break;
			case LUA_TNUMBER:
				std::cout << (int)lua_tonumber(L, i) << std::endl;
				break;
			case LUA_TBOOLEAN:
				std::cout << lua_toboolean(L, i) << std::endl;
				break;
			default:
				std::cout << lua_typename(L, i) << std::endl;
				break;
			};
		};
	}

	static bool GetBoolean(lua_State* L)
	{
		int pos = -1;
		bool nValue = false;

		if (lua_isboolean(L, pos))
		{
			nValue = lua_toboolean(L, pos);
			lua_pop(L, 1);
			return nValue;
		}
		CEasylog::GetInstance()->info("[Script] Cur Stack Value Type:", lua_typename(L, lua_type(L, pos)));
		abort();
		return nValue;
	}
	static int GetInteger(lua_State* L)
	{
		//DumpLuaState(L);
		int pos = -1;
		int nValue = 0;

		if (lua_isnumber(L, pos))
		{
			nValue = lua_tonumber(L, pos);
			lua_pop(L, 1);
			return nValue;
		}
		CEasylog::GetInstance()->info("[Script] Cur Stack Value Type:", lua_typename(L,lua_type(L, pos)));
		CEasylog::GetInstance()->info(lua_tostring(L, pos));
		abort();
		return nValue;
	}
	static std::string GetString(lua_State* L)
	{
		//DumpLuaState(L);
		int pos = -1;
		std::string nValue = "";
		if (lua_isstring(L, pos))
		{
			nValue = lua_tostring(L, pos);
			lua_pop(L, 1);//实际调用规则:-n-1
			return nValue;
		}

		CEasylog::GetInstance()->info("[Script] Cur Stack Value Type:", lua_typename(L, lua_type(L, pos)));
		abort();
		return nValue;
	}
	//注册到lua
public:
	static int UpdatePlaying(lua_State* L)
	{
		int uid = GetInteger(L);
		CLoginHall::GetInstance()->UpdatePlaying(uid);
		return 0;
	}
	static int	UpdateExited(lua_State* L)
	{
		int uid = GetInteger(L);
		CLoginHall::GetInstance()->UpdateExited(uid);
		return 0;
	}
	static int UpdateUserInfo(lua_State* L)
	{
		std::map<std::string, CValueInfo> info;
		lua_pushnil(L);
		while (lua_next(L, -2))
		{
			//这时值在-1（栈顶）处，key在-2处。  
			std::string key = lua_tostring(L, -2);

			if (lua_isstring(L, -1))
				info[key].Set(lua_tostring(L, -1));
			else if (lua_isnumber(L, -1))
				info[key].Set(lua_tonumber(L, -1));
			else
				abort();

			//把栈底的值移出栈，让key成为栈顶以便继续遍历  
			lua_pop(L, 1);
		}
		lua_pop(L, 1);

		int uid = GetInteger(L);

		CLoginHall::GetInstance()->UpdateUserInfo(uid,info);
		return 0;
	}
	static int SendData(lua_State* L)
	{
		std::string message = GetString(L);
		int cmd = GetInteger(L);
		int uid = GetInteger(L);
		
		CLoginHall::GetInstance()->SendData(uid, cmd, message);
		return 1;
	}
	static int Log(lua_State* L)
	{
		std::string str = GetString(L);
		CEasylog::GetInstance()->info("[Script]", str);
		return 0;
	}

	//c++调用lua
public:
	static void HandlerError(lua_State* L)
	{
		CEasylog::GetInstance()->error(lua_tostring(L, -1));
		lua_pop(L, -1);
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

		bRet = GetBoolean(L);

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
	static void UpdateUserInfo(int uid, const std::map<std::string, CValueInfo>& info)
	{
		lua_getglobal(L, "OnUpdateUserInfo");
		lua_pushnumber(L, uid);
		lua_newtable(L);
		for (auto it : info)
		{
			lua_pushstring(L, it.first.data());
			switch (it.second.m_type)
			{
			case CValueInfo::ValueType::TYPE_INTEGER:
				lua_pushnumber(L,it.second.GetInteger());
				break;
			case CValueInfo::ValueType::TYPE_STRING:
				lua_pushstring(L, it.second.GetString().data());
				break;
			default:
				abort();
				break;
			}
			lua_settable(L, -3);
		}
		if(lua_pcall(L, 2, 0, 0) != 0)
			HandlerError(L);
	}
};
#endif