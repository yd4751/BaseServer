﻿// NetCore.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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
#include "CTools.h"
static bool bInit = false;
int32_t nServerID = -1;

bool NetWorkEvent(int nClientID, int nCmd, int nMsgLength, char* msgBuf, NetCore::ProtocolType type)
{
	std::cout << __FUNCTION__ << ": " << nClientID << " Cmd:" << nCmd << "  Length:" << nMsgLength << std::endl;
	
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
void OnTimeOut(int timerID)
{
	std::cout << __FUNCTION__ << " : " << Tools::GetCurDateTime() << std::endl;
};

void Init()
{
	if (bInit)
	{
		return;
	}
	
	NetCore::Config(0);
	NetCore::RegisterEvnetHandler(NetWorkEvent);
	NetCore::Start();
	
	nServerID = NetCore::Connect("127.0.0.1", 9999);
	//LoginTest();
	bInit = true;
};

void LoginTest()
{
	if (nServerID < 0)
	{
		std::cout << "server is disconnect!" << std::endl;
		return;
	}

	Json::Value sendStr;
	sendStr["account"] = "bighat";
	sendStr["password"] = "bighat";
	std::string str = sendStr.toStyledString();
	NetCore::SendData(nServerID, 5004, str.c_str(), str.length(), NetCore::ProtocolType::PROTO_TYPE_JSON);
}
void AccountCreate()
{
	if (nServerID < 0)
	{
		std::cout << "server is disconnect!" << std::endl;
		return;
	}

	Json::Value sendStr;
	sendStr["account"] = "create_bighat";
	sendStr["password"] = "123456";
	std::string str = sendStr.toStyledString();
	NetCore::SendData(nServerID, 5001, str.c_str(), str.length(), NetCore::ProtocolType::PROTO_TYPE_JSON);
}
void AccountUpdate()
{
	if (nServerID < 0)
	{
		std::cout << "server is disconnect!" << std::endl;
		return;
	}
	NetCore::SendData(nServerID, 5002, nullptr, 0, NetCore::ProtocolType::PROTO_TYPE_JSON);
}
void AccountDelete()
{
	if (nServerID < 0)
	{
		std::cout << "server is disconnect!" << std::endl;
		return;
	}
	NetCore::SendData(nServerID, 5003, nullptr, 0, NetCore::ProtocolType::PROTO_TYPE_JSON);
}
void GuestLogin()
{
	if (nServerID < 0)
	{
		std::cout << "server is disconnect!" << std::endl;
		return;
	}
	NetCore::SendData(nServerID, 5006, nullptr, 0, NetCore::ProtocolType::PROTO_TYPE_JSON);
}
void LoginRoom()
{
	if (nServerID < 0)
	{
		std::cout << "server is disconnect!" << std::endl;
		return;
	}
	NetCore::SendData(nServerID, 1000, nullptr, 0, NetCore::ProtocolType::PROTO_TYPE_JSON);
}
int main()
{
#if 0

	while (true)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		std::string input;
		std::cin >> input;
		if (input == "start")
		{
			Init();
		}
		else if (input == "stop")
		{
			NetCore::Stop();
			nServerID = -1;
			bInit = false;
		}
		else if (input == "exit")
		{
			break;
		}
		else if (input == "create")
		{
			AccountCreate();
		}
		else if (input == "update")
		{
			AccountUpdate();
		}
		else if (input == "delete")
		{
			AccountDelete();
		}
		else if (input == "guest")
		{
			GuestLogin();
		}
		else if (input == "loginroom")
		{
			LoginRoom();
		}
	}
#else

	//int nid = NetCore::AddTimer(1000);
	//nid = NetCore::AddTimer(5000);
	NetCore::Config(8500);
	NetCore::RegisterEvnetHandler(NetWorkEvent);
	NetCore::RegisterTimerHandler(OnTimeOut);
	NetCore::Start();

	NetCore::AddTimer(1000);
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
/*
	while (true)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		std::string input;
		std::cin >> input;
		if (input == "start")
		{
			NetCore::Config(8500);
			NetCore::RegisterEvnetHandler(NetWorkEvent);
			NetCore::RegisterTimerHandler(OnTimeOut);
			NetCore::Start();
		}
		else if (input == "stop")
		{
			std::cout << "Wait stop!" << std::endl;
			NetCore::Stop();
		}
		else if (input == "exit")
		{
			break;
		}
	}*/
#endif



	return 0;
}

