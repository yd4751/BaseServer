#ifndef __H_NETCORE_INTERFACE_H__
#define __H_NETCORE_INTERFACE_H__

enum ClientMangerCmd
{
	MANAGER_INVALID = 0,
	CONNECT,
	DISCONNECT,
};
enum ProtocolType
{
	PROTO_TYPE_NULL = 0,
	PROTO_TYPE_JSON,
	PROTO_TYPE_PB,
	PROTO_TYPE_MAX,
};
enum ErrorCode
{
	ERROR_CODE_NULL = 0,
	ERROR_CODE_PACKET_BUFF_NOT_ENOUGH,
	ERROR_CODE_PACKET_CODER_ERROR,
	ERROR_CODE_PACKET_CODER_BUFFER_ERROR,
	ERROR_CODE_CONNECT_CLOSED,
};
typedef bool(*cbEventHandler)(int nClientID, int nCmd, int nMsgLength, char* msgBuf, ProtocolType type);
typedef void(*cbTimerHandler)(int timerID);
typedef void(*LogHandler)(const char*, int);

#if defined(__WINDOWS__)
#ifndef CAPI_DECLSPEC
#define CAPI_DECLSPEC __declspec(dllexport)
#else
#define CAPI_DECLSPEC __declspec(dllimport)
#endif
#else
#define CAPI_DECLSPEC
#endif
extern "C"
{
	void CAPI_DECLSPEC Config(int);
	void CAPI_DECLSPEC RegisterEvnetHandler(cbEventHandler);
	int  CAPI_DECLSPEC Connect(const char* ip, int port);
	void CAPI_DECLSPEC Disconnect(int id);
	bool CAPI_DECLSPEC SendData(int id , int cmd ,const char* data, int length, ProtocolType type);
	void CAPI_DECLSPEC Start();
	void CAPI_DECLSPEC Stop();

	//Timer
	void CAPI_DECLSPEC RegisterTimerHandler(cbTimerHandler);
	int	 CAPI_DECLSPEC AddTimer(int expiredTime);	
	void CAPI_DECLSPEC CancelTimer(int timerID);
	void CAPI_DECLSPEC ResetTimer(int timerID);
	int  CAPI_DECLSPEC GetTimerReaminTime(int timerID);

	//extern
	void CAPI_DECLSPEC RegisterLogHandler(LogHandler);
}


#endif