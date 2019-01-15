#include "CNetCoreInterface.h"
#include "CNetWork.h"
#include "CTimer.h"

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
	void CAPI_DECLSPEC Disconnect(int id)
	{
		CNetWork::GetInstance()->Disconnect(id);
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
	void CAPI_DECLSPEC RegisterTimerHandler(cbTimerHandler handler)
	{
		CTimer::GetInstance()->RegisterTimerHandler(handler);
	};
	int	 CAPI_DECLSPEC AddTimer(int expiredTime)
	{
		return CTimer::GetInstance()->AddTimer(expiredTime);
	};
	void CAPI_DECLSPEC CancelTimer(int timerID)
	{
		CTimer::GetInstance()->CancelTimer(timerID);
	};
	void CAPI_DECLSPEC ResetTimer(int timerID)
	{
		CTimer::GetInstance()->ResetTimer(timerID);
	};
	int  CAPI_DECLSPEC GetTimerReaminTime(int timerID)
	{
		return CTimer::GetInstance()->GetTimerReaminTime(timerID);
	};
}