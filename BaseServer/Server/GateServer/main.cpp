#include <iostream>
#include "CServer.h"

#include "CTimerTask.h"
#include "global_include.h"
#include <thread>
#include <chrono>
using namespace std;

int main()
{
	CServer::GetInstance()->Start();

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	CServer::GetInstance()->Stop();
	
	return 0;
}