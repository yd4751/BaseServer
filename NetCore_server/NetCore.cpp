// NetCore.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//#include "pch.h"
#include <iostream>
namespace NetCore
{
#include "CNetCoreInterface.h"
}
#include <thread>
#include <chrono>
#include <json/json.h>

bool NetWorkEvent(int nClientID, int nCmd, int nMsgLength, char* msgBuf, NetCore::ProtocolType type)
{
	std::cout << __FUNCTION__ << ": " << nClientID << " Cmd:" << nCmd << "  Length:" << nMsgLength << std::endl;

	//std::string reply("1234567890");
	//NetCore::SendData(nClientID, 1234, reply.c_str(), reply.length(), NetCore::ProtocolType::PROTO_TYPE_JSON);

	Json::Value sendStr;
	sendStr["code"] = 1;
	std::string reply = sendStr.toStyledString();
	NetCore::SendData(nClientID, 6004, reply.c_str(), reply.length(), NetCore::ProtocolType::PROTO_TYPE_JSON);
	return true;
};

int main()
{
#if 0
	NetCore::Config(0);
	NetCore::RegisterEvnetHandler(NetWorkEvent);
	NetCore::Start();

	Json::Value sendStr;
	sendStr["account"] = "12321";
	sendStr["password"] = "12321";
	std::string str = sendStr.toStyledString();
	int nServer = NetCore::Connect("127.0.0.1", 9999);
	NetCore::SendData(nServer, 5004, str.c_str(), str.length(), NetCore::ProtocolType::PROTO_TYPE_JSON);
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
#else
	NetCore::Config(9999);
	NetCore::RegisterEvnetHandler(NetWorkEvent);
	NetCore::Start();

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
#endif



	return 0;
}

