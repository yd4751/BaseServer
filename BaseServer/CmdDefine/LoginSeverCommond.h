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
		int32_t		id;			//�û�id
		int32_t		sex;		//�Ա�
		int32_t		money;		//Я��
		std::string nickName;	//�ǳ�
		std::string avatar;		//ͷ���ַ
	};
};

#endif