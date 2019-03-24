#ifndef __H_CENTERSERVERCOMMOND_H__
#define __H_CENTERSERVERCOMMOND_H__

namespace NS_Center	//200000
{
	namespace Request//200000
	{
		enum commond
		{
			CMD_NULL = 200000,
			Register,
			Update,
			OnlineList,
			CMD_MAX,
		};
	};
	namespace Reply//250000
	{
		enum commond
		{
			CMD_NULL = 250000,
			Register,
			Update,
			OnlineList,
			CMD_MAX,
		};
	};

	//������ע��
	struct ReqRegister
	{
		std::string ip = "";
		int	port = 0;
		int type = 0;
	};
	//
	struct ReplyRegister
	{
		
	};
	//��ȡ�������б�
	struct ServerInfo
	{
		std::string ip = "";
		int	port = 0;
		int type = 0;
	};
	struct ReqServerList
	{

	};
	struct ReplyServerList
	{
		std::vector<ServerInfo>	servers;
	};
};

#endif