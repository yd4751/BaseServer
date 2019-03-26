#ifndef __H_GAMESERVERCOMMOND_H__
#define __H_GAMESERVERCOMMOND_H__

namespace NS_Game	//400000
{
	namespace Request//400000
	{
		enum commond
		{
			CMD_NULL = 400000,
			CMD_MAX = 450000,
		};
	};
	namespace Reply//450000
	{
		enum commond
		{
			CMD_NULL = 450000,
			CMD_MAX = 500000,
		};
	};
};

#endif