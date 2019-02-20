// Server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "global_include.h"
#include "CLoginHall.h"
#include "CProtocolParse.h"
#include "CRedis.h"
#include "CRpc.h"
#include <memory>

#include <thread>
#include <chrono>
using namespace MessageDefine;
using namespace ProtoParse;


//直接以杀进程信息退出，过程会比较慢，如何处理这个杀进程形式...

int main()
{	
#if 1
	//模块初始化
	CEasylog::GetInstance()->Init("server.log",LOGLEVEL_DEBUG);
	//Server启动
	NetCore::Config(9999);

	NetCore::RegisterEvnetHandler(&CLoginHall::OnNetEventHandler);

	//
	//CLoginHall::GetInstance()->RegisterNextDispatcher()
	CLoginHall::GetInstance()->Start();
	NetCore::Start();

	std::cout << "===Main Thread:" << std::this_thread::get_id() << std::endl;
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	NetCore::Stop();
#else
	CRedis test;
	test.Connect("127.0.0.1",8888,"bighat");
	auto it = test.HGetAll<RedisStringPairsPtr>("mytable");
#endif
	return 0;
}

/*//协议封装测试
RequestClientLogin req(1234);
std::string str = ProtoParse::MakePacket(ProtocolType::PROTO_TYPE_JSON, req);

std::shared_ptr<CMessage> msg = std::make_shared<CMessage>();
msg->SetProtoType(ProtocolType::PROTO_TYPE_JSON);
msg->SetSize(str.length());
memcpy((char*)msg->GetData().get()->Get(), str.c_str(), str.length());

RequestClientLogin get = ProtoParse::Parse<RequestClientLogin>(msg);
*/

/*
std::shared_ptr<CArray<char>> pTestBuf(2048);
void RecvCall(std::shared_ptr<CArray<char>> pRecv,uint32_t nLength)
{
	CRpc pRpc(pRecv.get(), nLength);

	uint32_t nCmd;
	pRpc >> nCmd;
	std::string account;
	pRpc >> account;
	std::string password;
	pRpc >> password;
}

class TestRpc:
	public CRpc
{
public:
	TestRpc()
	{
	}
	virtual void SendCall(uint32_t nLength)
	{
		GetSerializeData(pTestBuf.get(), nLength);

		RecvCall(pTestBuf, nLength);
	}


};

//RPC测试
TestRpc test;
test.Call(1234, "bighat", "bighat123456");
*/
