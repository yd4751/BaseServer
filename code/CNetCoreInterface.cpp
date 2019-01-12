#include "CNetCoreInterface.h"
#include "CNetWork.h"

extern "C"
{
	static CNetWorkConfig config;
	void CAPI_DECLSPEC Config(int nListenPort)
	{
		config.m_CommuniterConfig.nMaxCommuniterGroupCapacity = 1000;
		config.m_CommuniterConfig.nInitCommuniterGroupCount = 1;
		config.m_CommuniterConfig.nMaxCommuniterGroupCount = 1;
		config.m_AcceptorConfig.nListenPort = nListenPort;

		CEasylog::GetInstance()->Init("network_core.log", Loglvel::LOGLEVEL_INFO);
		CSocketInterface::GetInstance()->Init();
	};
	void CAPI_DECLSPEC RegisterEvnetHandler(cbEventHandler handler)
	{
		CNetWork::GetInstance()->RegisterEventHandler(handler);
	};
	int  CAPI_DECLSPEC Connect(const char* ip, int port)
	{
		return CNetWork::GetInstance()->Connect(ip, port);
	};
	bool CAPI_DECLSPEC SendData(int id, int cmd, const char* data, int length, ProtocolType type)
	{
		return CNetWork::GetInstance()->SendData(id, cmd, data, length, type);
	};
	void CAPI_DECLSPEC Start()
	{
		CNetWork::GetInstance()->Start(config);
	};
	void CAPI_DECLSPEC Stop()
	{
		CNetWork::GetInstance()->Stop();
	};

	//Timer
	//AddTask()
	//StopTask()
	//CancelTask()
}