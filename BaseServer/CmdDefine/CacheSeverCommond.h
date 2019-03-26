#ifndef __H_CACHESERVERCOMMOND_H__
#define __H_CACHESERVERCOMMOND_H__

namespace NS_Cache	//500000
{
	namespace Request//500000
	{
		enum commond
		{
			CMD_NULL = 500000,
			CMD_MAX = 550000,
		};
	};
	namespace Reply//550000
	{
		enum commond
		{
			CMD_NULL = 550000,
			CMD_MAX = 600000,
		};
	};
};

#endif