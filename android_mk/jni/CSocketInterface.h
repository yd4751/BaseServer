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

#define MAX_OPEN_FD 1024
#define MAX_CONNECT_WAIT_TIME  5000
class CSocketInterface:
	public CSingleton<CSocketInterface>,
	public CBaseSocket
{
	int32_t  server_socket;          //是保存服务器监听端口的 socket
#if defined(__WINDOWS__)
	fd_set  fdread;                 //创建一个文件集合
#elif defined(__LINUX__)
	int32_t efd;
	struct epoll_event ep[MAX_OPEN_FD];
	struct epoll_event epNewFd;

	struct sockaddr_in cliaddr;
	socklen_t clilen = sizeof(cliaddr);
#endif

public:
	CSocketInterface()
	{

	};
	~CSocketInterface()
	{
#if defined(__WINDOWS__)
		closesocket(server_socket);
		WSACleanup();
#elif defined(__LINUX__)
		close(server_socket);
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
	virtual int32_t CreateListen(std::string ip, int32_t port)
	{
#if defined(__WINDOWS__)
		server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (server_socket == INVALID_SOCKET)
		{
			CEasylog::GetInstance()->error("create socket fail!");
			return -1;
		}

		int opt = 1;
		if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) != 0)
		{
			CEasylog::GetInstance()->error("setsockopt fail!");
			return -1;
		}
		//绑定IP和端口      
		sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_port = htons(port);
		sin.sin_addr.S_un.S_addr = INADDR_ANY;
		if (bind(server_socket, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
		{
			CEasylog::GetInstance()->error("bind fail!");
			return -1;
		}
		//开始监听      
		if (listen(server_socket, 5) == SOCKET_ERROR)
		{
			CEasylog::GetInstance()->error("listen fail!");
			return -1;
		}


#elif defined(__LINUX__)
		server_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (server_socket <= 0)
		{
			CEasylog::GetInstance()->error("create socket fail!");
			return -1;
		}

		struct sockaddr_in servaddr;
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(port);
		// 端口复用
		int opt = 1;
		if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)) != 0)
		{
			perror("setsockopt fail!");
			return -1;
		}
		//
		if (bind(server_socket, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
		{
			perror("bind fail!");
			return -1;
		}
		//
		if (listen(server_socket, 20) != 0)
		{
			perror("listen fail!");
			return -1;
		}
	
		//
		epNewFd.events = EPOLLIN;
		epNewFd.data.fd = server_socket;
		epoll_ctl(efd, EPOLL_CTL_ADD, server_socket, &epNewFd);
#endif
		//异步
		SetASync(server_socket);
		return server_socket;
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
		timeo.tv_sec = timeout / 1000;
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
		fd_set readfds;
		fd_set writefds;
		struct timeval timeout;

		FD_ZERO(&readfds);
		FD_SET(fd, &writefds);

		timeout.tv_sec = time; //timeout is 10 minutes
		timeout.tv_usec = 0;

		ret = select(fd + 1, NULL, &writefds, NULL, &timeout);
		if (ret <= 0)
		{
			//connection time out
			return false;
		}

		if (!FD_ISSET(sockfd, &writefds))
		{
			//no events on sockfd found
			return false;
		}

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

		//connection successful!
		return true;
#endif
	}
	virtual int32_t Connect(std::string ip, int32_t port)
	{
		int32_t sclient = -1;
#if defined(__WINDOWS__)
		sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);		
		if (sclient == INVALID_SOCKET) 
		{ 		
			return -1; 
		}				
		//
		SetASync(sclient);
		sockaddr_in serAddr;		
		serAddr.sin_family = AF_INET;		
		serAddr.sin_port = htons(port);
		serAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
		int ret = connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr));
		int tt = errno;
		int ff = GetLastError();
		if(ret == SOCKET_ERROR && GetLastError() != WSAEWOULDBLOCK)//EINPROGRESS标识正在处理
		{  
			//连接失败 				
			closesocket(sclient);			
			return -1;		
		}
		if (ret == SOCKET_ERROR && !WaitASyncConnectDone(sclient,MAX_CONNECT_WAIT_TIME))
		{
			//连接失败 				
			closesocket(sclient);
			return -1;
		}
#elif defined(__LINUX__)
		struct sockaddr_in address;
		bzero(&address, sizeof(address));
		address.sin_family = AF_INET;
		inet_pton(AF_INET, ip.c_str(), &address.sin_addr);
		address.sin_port = htons(port);

		sclient = socket(PF_INET, SOCK_STREAM, 0);
		if (sclient <= 0)
		{
			return -1;
		}
		//
		SetASync(sclient);
		int ret = connect(sclient, (struct sockaddr*)&address, sizeof(address));
		if (ret != 0 && errno != EINPROGRESS)
		{
			return -1;
		}
		if (ret != 0 && !WaitASyncConnectDone(sclient, MAX_CONNECT_WAIT_TIME))
		{
			//连接失败 				
			closesocket(sclient);
			return -1;
		}
		//
		epNewFd.events = EPOLLIN;
		epNewFd.data.fd = sclient;
		epoll_ctl(efd, EPOLL_CTL_ADD, sclient, &epNewFd);
#endif
		return sclient;
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
	};


	virtual int32_t Accept()
	{
		int32_t client = -1;
#if defined(__WINDOWS__)
		client = accept(server_socket, NULL, NULL);
		if (INVALID_SOCKET == client)
			return -1;
#elif defined(__LINUX__)
		client = accept(server_socket, (struct sockaddr*)&cliaddr, &clilen);
		if (client > 0)
		{
			//
			epNewFd.events = EPOLLIN;
			epNewFd.data.fd = client;
			epoll_ctl(efd, EPOLL_CTL_ADD, client, &epNewFd);
		}
#endif
		return client;
	};


	virtual void GetActives(std::vector<int32_t>& input, std::vector<int32_t>& output)
	{
		output.clear();
#if defined(__WINDOWS__)
		FD_ZERO(&fdread);                            //清楚监听文件集中的所有文件
		for (auto it : input)
		{
			FD_SET(it, &fdread);
		}

		struct timeval tm;
		tm.tv_sec = 0;
		tm.tv_usec = 200;

		if (select(0, &fdread, NULL, NULL, &tm) == SOCKET_ERROR)
			return;

		for (auto it : input)
		{
			if (FD_ISSET(it, &fdread)) output.emplace_back(it);
		}
#elif defined(__LINUX__)
		int32_t nready = epoll_wait(efd, ep, MAX_OPEN_FD, -1);
		for (int i = 0; i < nready; ++i)
		{
			output.emplace_back((int32_t)ep[i].data.fd);
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