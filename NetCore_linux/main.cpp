
#include <iostream>
namespace NetCore
{
#include "CNetCoreInterface.h"
}
int main()
{
	NetCore::Config(9999);
	NetCore::Start();
	return 0;
}