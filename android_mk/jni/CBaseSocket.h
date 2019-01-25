#ifndef __H_BASE_SOCKET_H__
#define __H_BASE_SOCKET_H__
#include <cstdint>
#include <string>
#include <vector>

class CBaseSocket
{
public:
	CBaseSocket() 
	{
	};
	virtual ~CBaseSocket() {};

public:
	virtual void Init() = 0;
	virtual int32_t CreateListen(std::string ip, int32_t port) = 0;
	virtual int32_t Connect(std::string ip, int32_t port) = 0;
	virtual void Close(int32_t id) = 0;
	virtual int32_t Accept() = 0;
	virtual void GetActives(std::vector<int32_t>& input, std::vector<int32_t>& output) = 0;
	virtual int32_t Read(int32_t id, char* readBuf, int32_t readLen) = 0;
	virtual int32_t Write(int32_t id, char* writeBuf, int32_t writeLen) = 0;
	virtual void SetASync(int32_t id) = 0;
};

#endif