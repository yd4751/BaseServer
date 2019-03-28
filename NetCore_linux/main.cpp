
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
	Json::Value sendStr;
	int nReplyCmd = 0;
	//auth
	if (nCmd == 100001)
	{
		nReplyCmd = 150001;
	}
	//login
	else if (nCmd == 300002)
	{
		nReplyCmd = 350001;
		sendStr["id"] = 123456;			//用户id
		sendStr["sex"] = 2;		//性别
		sendStr["money"] = 100;		//携带
		sendStr["nickName"] = "哈哈";	//昵称
		sendStr["avatar"] = "www.google.com";		//头像地址
	}
	//game login
	else if (nCmd == 400001)
	{
		nReplyCmd = 450001;
	}
	//room login
	else if (nCmd == 401001)
	{
		nReplyCmd = 451001;
	}
	else return true;

	std::string reply = sendStr.toStyledString();
	NetCore::SendData(nClientID, nReplyCmd, reply.c_str(), reply.length(), NetCore::ProtocolType::PROTO_TYPE_JSON);
	return true;
};
int main()
{
	NetCore::Config(8500);
	NetCore::RegisterEvnetHandler(EventHandler);
	NetCore::Start();

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return 0;
}