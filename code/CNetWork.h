#ifndef __H_NETWORK_H__
#define __H_NETWORK_H__
#include "DataDefine.h"
#include "CCommuniterGroups.h"
#include "CMessage.h"
#include "CTimer.h"
#include <condition_variable>//条件变量
#include "CDataCache.h"

class CNetWorkConfig
{
public:
	//
	struct CommuniterConfig
	{
		int32_t			nInitCommuniterGroupCount;			//初始化个数
		int32_t			nMaxCommuniterGroupCount;			//最大个数
		int32_t			nMaxCommuniterGroupCapacity;		//单个最大容量
	}					m_CommuniterConfig;

	//
	struct AcceptorConfig
	{
		int32_t nListenPort;
	}					m_AcceptorConfig;

public:
	CNetWorkConfig()
	{
		m_CommuniterConfig.nInitCommuniterGroupCount = 1;
		m_CommuniterConfig.nMaxCommuniterGroupCount = 1;
		m_CommuniterConfig.nMaxCommuniterGroupCapacity = 100;

		m_AcceptorConfig.nListenPort = 0;
	}
};
class CNetWork:
	public CSingleton<CNetWork>,
	public CBaseWorker
{

	CNetWorkConfig												m_Config;

	cbEventHandler												m_pNetEventHandler;

	std::map<int32_t, std::shared_ptr<CCommuniter>>				m_connects;
	std::vector<int32_t>										m_ReadActive;

public:
	CNetWork():CBaseWorker(false){};
	virtual ~CNetWork()
	{
		Stop();
	};

public:


	void Start(const CNetWorkConfig& config)
	{
		if (CBaseWorker::IsRunning())
		{
			CEasylog::GetInstance()->warn("NetWork is already running...");
			return;
		}

		m_Config = config;

		CEasylog::GetInstance()->debug(__FUNCTION__);

		if (m_Config.m_AcceptorConfig.nListenPort > 0)
		{
			std::shared_ptr<CCommuniter> pNewCommuniter = std::make_shared<CCommuniter>();
			pNewCommuniter->m_socket = CSocketInterface::GetInstance()->CreateListen("0.0.0.0", m_Config.m_AcceptorConfig.nListenPort);
			pNewCommuniter->UpdateStatus(ConnectionStatus::CONNECT_STATUS_WRITE);//不触发初始化事件
			pNewCommuniter->bServer = true;
			pNewCommuniter->RegisterHandler(std::bind(&CNetWork::OnConnectionEvent, this, std::placeholders::_1, std::placeholders::_2));
			m_connects.emplace(pNewCommuniter->GetID(), pNewCommuniter);
		}

		CBaseWorker::Start();
		CEasylog::GetInstance()->warn("NetWork run success!");

		//
		CTimer::GetInstance()->Start();
		CEasylog::GetInstance()->warn("TimerWork run success!");
	}
	void Stop()
	{
		CTimer::GetInstance()->Stop();

		for (auto it : m_connects)
		{
			CSocketInterface::GetInstance()->Close(it.second->m_socket->fd);
			it.second->Disconnect();
		}
		m_connects.clear();
		CBaseWorker::Stop();
	}

	void OnConnectionEvent(int32_t id, std::shared_ptr<CMessage> msg)
	{
		//std::lock_guard<std::mutex>	gurd(m_netEventlock);

		CEasylog::GetInstance()->info(__FUNCTION__,"RecvID:",id,"MsgLen:",msg->GetSize());
		if (msg->GetSize() > 0)
		{
			CEasylog::GetInstance()->info((char*)(msg->GetData().get()->Get()));
		}
		if (m_pNetEventHandler)
		{
			m_pNetEventHandler(id,msg->GetCommond(),msg->GetSize(),(char*)(msg->GetData().get()->Get()),msg->GetProtoType());
		}
	}

public:
	//
	virtual bool Work()
	{	
		//timer
		if (CTimer::GetInstance()->Work())
		{
			return true;
		}
		bool bWork = false;
		//get active ids
		CSocketInterface::GetInstance()->Actives(m_ReadActive);
		for (auto it : m_ReadActive)
		{
			auto pConn = m_connects.find(it);
			bWork = pConn->second->Read();
			if (pConn->second->bServer)
			{
				std::shared_ptr<CCommuniter> pNewCommuniter = std::make_shared<CCommuniter>();
				pNewCommuniter->bServer = false;
				pNewCommuniter->m_socket = CSocketInterface::GetInstance()->Accept(pConn->second->m_socket);
				pNewCommuniter->RegisterHandler(std::bind(&CNetWork::OnConnectionEvent, this, std::placeholders::_1, std::placeholders::_2));
				m_connects.emplace(pNewCommuniter->GetID(), pNewCommuniter);
			}
		}
		//
		for (auto it : m_connects)
		{
			if (it.second->InitConnect()) bWork = true;
			else if (it.second->Write()) bWork = true;
			else if (it.second->InitClose()) bWork = true;

			if (it.second->GetStatus() == ConnectionStatus::CONNECT_STATUS_CLOSED)
			{
				CEasylog::GetInstance()->info("fd:", it.first, " Do erase!");
				CSocketInterface::GetInstance()->Close(it.second->m_socket->fd);
				m_connects.erase(it.first);
				return true;
			}
		}
		return bWork;
	}

public:
	//外部接口
	int32_t Connect(std::string ip, int32_t port)
	{
		if (!CBaseWorker::IsRunning()) return -1;

		std::shared_ptr<CCommuniter> pNewCommuniter = std::make_shared<CCommuniter>();
		pNewCommuniter->m_socket = CSocketInterface::GetInstance()->Connect(ip,port);
		if (pNewCommuniter->m_socket == nullptr)
			return -1;
		pNewCommuniter->UpdateStatus(ConnectionStatus::CONNECT_STATUS_WRITE);//不触发初始化事件
		pNewCommuniter->RegisterHandler(std::bind(&CNetWork::OnConnectionEvent, this, std::placeholders::_1, std::placeholders::_2));
		m_connects.emplace(pNewCommuniter->GetID(), pNewCommuniter);

		return pNewCommuniter->GetID();
	}
	//
	void Disconnect(int32_t id)
	{
		if (!CBaseWorker::IsRunning()) return ;
		auto it = m_connects.find(id);
		if (it != m_connects.end())
			it->second->Disconnect();
	}
	bool SendData(int32_t id, int nCmd, const char* pInput, int nInputLength, ProtocolType nProtoType)
	{
		if (!CBaseWorker::IsRunning()) return false;
		auto it = m_connects.find(id);
		if (it != m_connects.end())
			it->second->SendData(nCmd, pInput, nInputLength, nProtoType);

		return false;
	}

	//注册回调
	void RegisterEventHandler(cbEventHandler handler)
	{
		m_pNetEventHandler = handler;
	}
};

#endif