#ifndef __H_GATESERVERCOMMOND_H__
#define __H_GATESERVERCOMMOND_H__

namespace NS_Gate	//100000
{
	namespace Request//100000
	{
		enum commond
		{
			CMD_NULL = 100000,
			CMD_AUTH,
			CMD_MAX = 150000,
		};
	};
	namespace Reply//150000
	{
		enum commond
		{
			CMD_NULL = 150000,
			CMD_AUTH,
			CMD_MAX = 200000,
		};
	};

	struct ReqAuth
	{

	};
	struct ReplyAuth
	{

	};
};

#endif