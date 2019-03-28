#ifndef __H_BASE_SOCKET_H__
#define __H_BASE_SOCKET_H__
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <memory>

enum SocketStatus
{
	READ = 0,
	WRITE ,
};
class CSocketInfo
{
public:
	int32_t			fd = -1;
	SocketStatus	status = SocketStatus::WRITE;
	std::string		ip = "";
	uint32_t		port = 0;
};
class CBaseSocket
{
public:
	CBaseSocket() 
	{
	};
	virtual ~CBaseSocket() {};

public:
	std::map<int32_t, std::shared_ptr<CSocketInfo>>		m_allSockets;
	virtual void Init() = 0;
	virtual std::shared_ptr<CSocketInfo> CreateListen(std::string ip, int32_t port) = 0;
	virtual std::shared_ptr<CSocketInfo> Connect(std::string ip, int32_t port) = 0;
	virtual void Close(int32_t id) = 0;
	virtual std::shared_ptr<CSocketInfo> Accept(std::shared_ptr<CSocketInfo>) = 0;
	virtual void Actives(std::vector<int32_t>& avtives) = 0;
	virtual int32_t Read(int32_t id, char* readBuf, int32_t readLen) = 0;
	virtual int32_t Write(int32_t id, char* writeBuf, int32_t writeLen) = 0;
	virtual void SetASync(int32_t id) = 0;
};

#endif