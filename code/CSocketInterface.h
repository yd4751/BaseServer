#ifndef __H_SOCKETINTERFACE_H__
#define __H_SOCKETINTERFACE_H__
#if defined(__WINDOWS__)
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib") 
#elif defined(__LINUX__)
#include<arpa/inet.h>
#include<sys/epoll.h>	//epoll
#include<fcntl.h>		//fcntl()
#include <unistd.h>		//close()
#include <errno.h>
#include <string.h>
#endif

#include "CBaseSocket.h"
#include "CSingleton.h"
#include "CTools.h"
#include "CEasyLog.h"

#define MAX_OPEN_FD 2048
#define MAX_CONNECT_WAIT_TIME  500
#if defined(__WINDOWS__)
#define FD_SETSIZE MAX_OPEN_FD  //windows默认最大select数为64
#endif
class CSocketInterface:
	public CSingleton<CSocketInterface>,
	public CBaseSocket
{
	struct sockaddr_in cliaddr;
	uint32_t clilen = sizeof(cliaddr);
#if defined(__WINDOWS__)
	fd_set  fdread;                 //创建一个文件集合
#elif defined(__LINUX__)
	int32_t efd;
	struct epoll_event ep[MAX_OPEN_FD];
	struct epoll_event epNewFd;	
#endif

public:
	CSocketInterface()
	{

	};
	~CSocketInterface()
	{
#if defined(__WINDOWS__)
		WSACleanup();
#elif defined(__LINUX__)
		close(efd);
#endif
	};

public:
	virtual void Init()
	{
#if defined(__WINDOWS__)
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA wsaData;

		if (WSAStartup(sockVersion, &wsaData) != 0)
		{
			return;
		}
#elif defined(__LINUX__)
		// 创建一个epoll fd
		efd = epoll_create(MAX_OPEN_FD);
#endif
	};
	virtual std::shared_ptr<CSocketInfo> CreateListen(std::string ip, int32_t port)
	{
		std::shared_ptr<CSocketInfo> pSocket = std::make_shared<CSocketInfo>();
#if defined(__WINDOWS__)
		pSocket->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (pSocket->fd == INVALID_SOCKET)
		{
			CEasylog::GetInstance()->error("create socket fail!");
			return nullptr;
		}

		int opt = 1;
		if (setsockopt(pSocket->fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) != 0)
		{
			CEasylog::GetInstance()->error("setsockopt fail!");
			return nullptr;
		}
		//绑定IP和端口      
		sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_port = htons(port);
		sin.sin_addr.S_un.S_addr = INADDR_ANY;
		if (bind(pSocket->fd, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
		{
			CEasylog::GetInstance()->error("bind fail!");
			return nullptr;
		}
		//开始监听      
		if (listen(pSocket->fd, 5) == SOCKET_ERROR)
		{
			CEasylog::GetInstance()->error("listen fail!");
			return nullptr;
		}


#elif defined(__LINUX__)
		pSocket->fd = socket(AF_INET, SOCK_STREAM, 0);
		if (pSocket->fd <= 0)
		{
			CEasylog::GetInstance()->error("create socket fail!");
			return nullptr;
		}

		struct sockaddr_in servaddr;
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(port);
		// 端口复用
		int opt = 1;
		if (setsockopt(pSocket->fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)) != 0)
		{
			perror("setsockopt fail!");
			return nullptr;
		}
		//
		if (bind(pSocket->fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
		{
			perror("bind fail!");
			return nullptr;
		}
		//
		if (listen(pSocket->fd, 20) != 0)
		{
			perror("listen fail!");
			return nullptr;
		}
	
		//
		epNewFd.events = EPOLLIN;
		epNewFd.data.fd = pSocket->fd;
		epoll_ctl(efd, EPOLL_CTL_ADD, pSocket->fd, &epNewFd);
#endif
		//异步
		SetASync(pSocket->fd);
		m_allSockets.emplace(pSocket->fd, pSocket);
		return pSocket;
	};

	virtual void SetASync(int32_t id)
	{
#if defined(__WINDOWS__)
		//异步
		ULONG NonBlock = 1;
		if (ioctlsocket(id, FIONBIO, &NonBlock) == SOCKET_ERROR)
		{
			//return -1;
		}
#elif defined(__LINUX__)
		int fl = fcntl(id, F_GETFL);
		fcntl(id, F_SETFL, fl | O_NONBLOCK);
#endif
	};

	bool WaitASyncConnectDone(int32_t fd,int32_t nMaxWaitTime)//ms
	{
#if defined(__WINDOWS__)
		fd_set set;
		FD_ZERO(&set);
		FD_SET(fd, &set);  //相反的是FD_CLR(_sock_fd,&set)


		time_t timeout = nMaxWaitTime;          //(超时时间设置为nMaxWaitTime毫秒)
		struct timeval timeo;
		timeo.tv_sec = (long)timeout / 1000;
		timeo.tv_usec = (timeout % 1000) * 1000;

		int retval = select(fd + 1, NULL, &set, NULL, &timeo);           //事件监听
		if (retval < 0)
		{
			//建立链接错误
			return false;
		}
		else if (retval == 0) // 超时
		{
			//超时链接没有建立
			return false;
		}

		//将检测到_socket_fd读事件或写时间，并不能说明connect成功
		if (FD_ISSET(fd, &set))
		{
			int error = 0;
			int len = sizeof(error);
			if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (char*)&error, &len) < 0)
			{
				return false;
			}
			if (error != 0) // 失败
			{
				return false;
			}
		}
		return true;
#elif defined(__LINUX__)
		fd_set writefds;
		struct timeval timeout;

		FD_ZERO(&writefds);
		FD_SET(fd, &writefds);

		timeout.tv_sec = nMaxWaitTime/1000; //timeout is nMaxWaitTime ms
		timeout.tv_usec = (nMaxWaitTime%1000)*1000;

		int ret = select(fd + 1, NULL, &writefds, NULL, &timeout);
		if (ret <= 0)
		{
			//connection time out
			return false;
		}

		if (FD_ISSET(fd, &writefds))
		{
			int error = 0;
			socklen_t length = sizeof(error);
			if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &length) < 0)
			{
				//get socket option failed
				return false;
			}

			if (error != 0)
			{
				//connection failed after select with the error;
				return false;
			}
		}

		//connection successful!
		return true;
#endif
	}
	virtual std::shared_ptr<CSocketInfo> Connect(std::string ip, int32_t port)
	{
		std::shared_ptr<CSocketInfo> pSocket = std::make_shared<CSocketInfo>();
#if defined(__WINDOWS__)
		pSocket->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (pSocket->fd == INVALID_SOCKET)
		{ 		
			return nullptr;
		}				
		//
		SetASync(pSocket->fd);
		sockaddr_in serAddr;		
		serAddr.sin_family = AF_INET;		
		serAddr.sin_port = htons(port);
		serAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
		int ret = connect(pSocket->fd, (sockaddr *)&serAddr, sizeof(serAddr));
		int tt = errno;
		int ff = GetLastError();
		if(ret == SOCKET_ERROR && GetLastError() != WSAEWOULDBLOCK)//EINPROGRESS标识正在处理
		{  
			//连接失败 				
			Close(pSocket->fd);
			return nullptr;		
		}
		if (ret == SOCKET_ERROR && !WaitASyncConnectDone(pSocket->fd,MAX_CONNECT_WAIT_TIME))
		{
			//连接失败 				
			Close(pSocket->fd);
			return nullptr;
		}
#elif defined(__LINUX__)
		struct sockaddr_in address;
		bzero(&address, sizeof(address));
		address.sin_family = AF_INET;
		inet_pton(AF_INET, ip.c_str(), &address.sin_addr);
		address.sin_port = htons(port);

		pSocket->fd = socket(PF_INET, SOCK_STREAM, 0);
		if (pSocket->fd <= 0)
		{
			return nullptr;
		}
		//
		SetASync(pSocket->fd);
		int ret = connect(pSocket->fd, (struct sockaddr*)&address, sizeof(address));
		if (ret != 0 && errno != EINPROGRESS)
		{
			return nullptr;
		}
		if (ret != 0 && !WaitASyncConnectDone(pSocket->fd, MAX_CONNECT_WAIT_TIME))
		{
			//连接失败 				
			Close(pSocket->fd);
			return nullptr;
		}
		//
		epNewFd.events = EPOLLIN;
		epNewFd.data.fd = pSocket->fd;
		epoll_ctl(efd, EPOLL_CTL_ADD, pSocket->fd, &epNewFd);
#endif
		m_allSockets.emplace(pSocket->fd, pSocket);
		return pSocket;
	};


	virtual void Close(int32_t id)
	{
#if defined(__WINDOWS__)
		::shutdown(id,2);
		closesocket(id);
#elif defined(__LINUX__)
		epoll_ctl(efd, EPOLL_CTL_DEL, id, NULL);
		close(id);
#endif
		m_allSockets.erase(id);
	};

	virtual std::shared_ptr<CSocketInfo> Accept(std::shared_ptr<CSocketInfo> pServer)
	{
		std::shared_ptr<CSocketInfo> pSocket = std::make_shared<CSocketInfo>();
		

#if defined(__WINDOWS__)
		pSocket->fd = accept(pServer->fd, (struct sockaddr*)&cliaddr, (int*)&clilen);
		if (INVALID_SOCKET == pSocket->fd)
			return nullptr;

		struct sockaddr_in *sockAddr = (struct sockaddr_in*)&cliaddr;
		pSocket->port = ntohs(sockAddr->sin_port);
		pSocket->ip.assign(inet_ntoa(sockAddr->sin_addr));
#elif defined(__LINUX__)
		pSocket->fd = accept(pServer->fd, (struct sockaddr*)&cliaddr, (socklen_t*)&clilen);
		if (pSocket->fd <= 0)
			return nullptr;
	
		struct sockaddr_in *sockAddrInfo = (struct sockaddr_in*)&cliaddr;
		char strAddr[INET_ADDRSTRLEN] = { 0 };

		pSocket->port = ntohs(sockAddrInfo->sin_port);
		inet_ntop(AF_INET,&(sockAddrInfo->sin_addr), strAddr, clilen);
		pSocket->ip.assign(strAddr);
		//
		epNewFd.events = EPOLLIN;
		epNewFd.data.fd = pSocket->fd;
		epoll_ctl(efd, EPOLL_CTL_ADD, pSocket->fd, &epNewFd);
#endif
		CEasylog::GetInstance()->info("IP:",pSocket->ip," Port:",pSocket->port);
		m_allSockets.emplace(pSocket->fd, pSocket);
		return pSocket;
	};


	virtual void Actives(std::vector<int32_t>& avtives)
	{
		avtives.clear();
#if defined(__WINDOWS__)
		FD_ZERO(&fdread);                            //清楚监听文件集中的所有文件
		for (auto it : m_allSockets)
		{
			FD_SET(it.second->fd, &fdread);
		}

		struct timeval tm;
		tm.tv_sec = 0;
		tm.tv_usec = 2;

		//return : 0超时  -1所有描述符清0  >0有激活
		if (select(0, &fdread, NULL, NULL, &tm) > 0)
		{
			for (auto it : m_allSockets)
			{
				if (FD_ISSET(it.second->fd, &fdread))
				{
					avtives.emplace_back(it.second->fd);
					//it.second->status = SocketStatus::READ;
					//CEasylog::GetInstance()->info("FD :", it.first, " ready to read!");
				}
			}
			//CEasylog::GetInstance()->info("TotalSize:", m_allSockets.size()," ActiveSize:", avtives.size());
		}
#elif defined(__LINUX__)
		int32_t nready = epoll_wait(efd, ep, MAX_OPEN_FD, 2);//毫秒，0会立即返回，-1将不确定
		for (int i = 0; i < nready; ++i)
		{
			avtives.emplace_back((int32_t)ep[i].data.fd);
		}
#endif
	};
	virtual int32_t Read(int32_t id, char* readBuf, int32_t readLen)
	{
		int32_t nRet = -1;
#if defined(__WINDOWS__)
		nRet =::recv((SOCKET)id, readBuf, readLen, 0);
		if (nRet < 0)
		{
			if (nRet == SOCKET_ERROR)
			{
				int ret = WSAGetLastError();
				switch (ret)
				{
				case 10054://对端关闭连接
					return 0;
				}

				return -1;
			}
			return 0;
		}
#elif defined(__LINUX__)
		//nRet = ::read(id, readBuf, readLen);
		nRet = ::recv(id, readBuf, readLen,0);
		if (nRet < 0)
		{
			if (errno == EAGAIN || errno == EINPROGRESS || errno == EINTR)
			{
			}
			else
			{
				perror("read error");
				return 0;
				//CEasylog::GetInstance().error(strerror(errno));
			}
	}
#endif
		return nRet;
	};
	virtual int32_t Write(int32_t id, char* writeBuf, int32_t writeLen)
	{
		int32_t nRet = -1;
#if defined(__WINDOWS__)
		nRet = ::send((SOCKET)id, writeBuf, writeLen, 0);
		if (nRet < 0)
		{
			if (nRet == SOCKET_ERROR)
			{
				int ret = WSAGetLastError();
				switch (ret)
				{
				case 10054://对端关闭连接
					return 0;
				}
				return -1;
			}
			return 0;
		}

#elif defined(__LINUX__)
		//nRet = ::write(id, writeBuf, writeLen);
		nRet = ::send(id, writeBuf, writeLen,0);
		if (nRet < 0)
		{
			if (errno == EAGAIN || errno == EINPROGRESS || errno == EINTR)
			{
			}
			else
			{
				perror("write error");
				return 0;
				//CEasylog::GetInstance().error(strerror(errno));
			}
		}
#endif
		return nRet;
	};

};

#endif