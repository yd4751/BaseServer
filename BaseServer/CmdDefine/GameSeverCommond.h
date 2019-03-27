#ifndef __H_GAMESERVERCOMMOND_H__
#define __H_GAMESERVERCOMMOND_H__

namespace NS_Game	//400000
{
	namespace Request//400000
	{
		enum commond
		{
			CMD_NULL = 400000,
			Login,
			Logout,
			//
			GameInfo = CMD_NULL + 1000,
			CMD_MAX = 450000,
		};
	};
	namespace Reply//450000
	{
		enum commond
		{
			CMD_NULL = 450000,
			Login,
			Logout,
			//
			GameInfo = CMD_NULL + 1000,
			CMD_MAX = 500000,
		};
	};

	struct ReqLogin
	{
		uint32_t id = 0;
	};
	struct ReplyLogin
	{
		uint32_t id = 0;
		uint32_t sex = 0;
		uint32_t money = 0;
		std::string nickName = "";
		std::string avatar = "";
	};
};

#endif