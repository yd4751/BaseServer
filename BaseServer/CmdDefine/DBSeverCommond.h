#ifndef __H_DBSERVERCOMMOND_H__
#define __H_DBSERVERCOMMOND_H__

namespace NS_DB	//600000
{
	namespace Request//600000
	{
		enum commond
		{
			CMD_NULL = 600000,
			CMD_MAX = 650000,
		};
	};
	namespace Reply//650000
	{
		enum commond
		{
			CMD_NULL = 650000,
			CMD_MAX = 700000,
		};
	};
};

#endif