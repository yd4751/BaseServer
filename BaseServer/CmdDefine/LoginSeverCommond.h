#ifndef __H_LOGINSERVERCOMMOND_H__
#define __H_LOGINSERVERCOMMOND_H__
namespace NS_Login	//300000
{
	namespace Request//300000
	{
		enum commond
		{
			CMD_NULL = 300000,
			Login,
			GuestLogin,
			CMD_MAX = 350000,
		};
	};
	namespace Reply//350000
	{
		enum commond
		{
			CMD_NULL = 350000,
			Login,
			CMD_MAX = 400000,
		};
	};

	struct ReqGuestLogin
	{

	};
	struct ReplyUserLogin
	{
		int32_t		id;			//用户id
		int32_t		sex;		//性别
		int32_t		money;		//携带
		std::string nickName;	//昵称
		std::string avatar;		//头像地址
	};
};

#endif