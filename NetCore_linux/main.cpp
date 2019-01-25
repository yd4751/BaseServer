
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
namespace NetCore
{
#include "CNetCoreInterface.h"
}
#include	<json/json.h>
using namespace std;

bool EventHandler(int nClientID, int nCmd, int nMsgLength, char* msgBuf, NetCore::ProtocolType type)
{
	cout << "[Recv] Cmd:" << nCmd << " MsgLen:" << nMsgLength << endl;
	return true;
};
int main()
{
	NetCore::Config(0);
	NetCore::RegisterEvnetHandler(EventHandler);
	NetCore::Start();

	int nServerID = NetCore::Connect("192.168.1.50", 9999);
	cout << "==" << nServerID << endl;

	Json::Value val;
	val["accont"] = "bighat";
	val["password"] = "123";
	std::string str = val.toStyledString();
	NetCore::SendData(nServerID,5004, str.c_str(), str.length(), NetCore::ProtocolType::PROTO_TYPE_JSON);

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return 0;
}