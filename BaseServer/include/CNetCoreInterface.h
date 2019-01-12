#ifndef __H_NETCORE_INTERFACE_H__
#define __H_NETCORE_INTERFACE_H__

enum ClientMangerCmd
{
	MANAGER_INVALID=10000,
	CONNECT,
	DISCONNECT,
};
enum ProtocolType
{
	PROTO_TYPE_JSON = 1,
	PROTO_TYPE_PB = 2,
};
typedef bool(*cbEventHandler)(int nClientID, int nCmd, int nMsgLength, char* msgBuf, ProtocolType type);

#if defined(__LINUX__)
#define CAPI_DECLSPEC __attribute__ ((visibility("default")))
#elif defined(__WINDOWS__)

//#define CAPI_DECLSPEC __declspec(dllexport)
#define CAPI_DECLSPEC __declspec(dllimport)

#endif

extern "C"
{
	void  CAPI_DECLSPEC Config(int);
	void CAPI_DECLSPEC RegisterEvnetHandler(cbEventHandler);
	int CAPI_DECLSPEC  Connect(const char* ip, int port);
	bool CAPI_DECLSPEC SendData(int id , int cmd ,const char* data, int length, ProtocolType type);
	bool CAPI_DECLSPEC Start();
	void CAPI_DECLSPEC Stop();

	//Timer
	//AddTask()
	//StopTask()
	//CancelTask()
}


#endif