#ifndef __H_NETWORK_H__
#define __H_NETWORK_H__
#include "DataDefine.h"
#include "CAcceptor.h"
#include "CCommuniterGroups.h"
#include "CMessage.h"

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
	std::shared_ptr<CAcceptor>									m_Acceptor;
	CCommuniterGroups											m_CommuniterGroups;

	CNetWorkConfig												m_Config;

	std::vector<int32_t>										m_listenSocks;	//select缓存
	CNodeManager<int32_t, std::weak_ptr<CBaseConnection>>		m_events;

	std::mutex													m_netEventlock;
	cbEventHandler												m_pNetEventHandler;

public:
	CNetWork():CBaseWorker(false){};
	virtual ~CNetWork()
	{
		Stop();
	};

protected:
	void GetActiveIds(std::vector<int32_t>& input, std::vector<int32_t>& output)
	{
		input.clear();
		m_events.GetKeys(input);

		CSocketInterface::GetInstance()->GetActives(input, output);
		//if(!output.empty())
			CEasylog::GetInstance()->debug("Cur Actives size:",output.size());
	}

protected:
	void AddConnection(std::shared_ptr<CCommuniter> pNewCommuniter)
	{
		std::shared_ptr<CCommuniterMgr> pTarget = m_CommuniterGroups.GetEmpty();

		if (!pTarget)
		{
			IncreaseCommuniterGroup();
			pTarget = m_CommuniterGroups.GetEmpty();
			if (!pTarget)
			{
				//fail , close connection
				CEasylog::GetInstance()->warn("fail to find empty group!");
				return;
			}
		}
		CSocketInterface::GetInstance()->SetASync(pNewCommuniter->GetID());
		pNewCommuniter->RegisterHandler(std::bind(&CNetWork::OnConnectionEvent, this, std::placeholders::_1, std::placeholders::_2));
		if (!m_events.Add(pNewCommuniter->GetID(), std::dynamic_pointer_cast<CBaseConnection>(pNewCommuniter)))
		{
			//fail , close connection
			CEasylog::GetInstance()->error("fail to add event!", pNewCommuniter->GetID());
			return;
		}
		//m_events.ForceAdd(pNewCommuniter->GetID(), std::dynamic_pointer_cast<CBaseConnection>(pNewCommuniter));

		pTarget->Add(pNewCommuniter->GetID(), pNewCommuniter);
		CEasylog::GetInstance()->debug("CurGroupSize:",m_CommuniterGroups.Size(),"CurMgr size:", pTarget->Size()," event size:", m_events.Size());
	}

public:
	void IncreaseCommuniterGroup()
	{
		if (m_CommuniterGroups.Size() >= m_Config.m_CommuniterConfig.nMaxCommuniterGroupCount)
		{
			CEasylog::GetInstance()->warn("beyond max connection group! max:", m_Config.m_CommuniterConfig.nMaxCommuniterGroupCount);
			return;
		}
		std::shared_ptr<CCommuniterMgr> node = std::make_shared<CCommuniterMgr>();
		node->SetMaxSize(m_Config.m_CommuniterConfig.nMaxCommuniterGroupCapacity);
		m_CommuniterGroups.Add(m_CommuniterGroups.Size(), node);
	}

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
			//
			m_Acceptor = std::make_shared<CAcceptor>();
			m_Acceptor->SetID(-1);//Start后会设置id
			m_Acceptor->RegisterHandler(std::bind(&CNetWork::OnNewConnection, this, std::placeholders::_1, std::placeholders::_2));
			m_Acceptor->Start(m_Config.m_AcceptorConfig.nListenPort);
			m_events.Add(m_Acceptor->GetID(), m_Acceptor);
		}

		for (int32_t i = 0; i < m_Config.m_CommuniterConfig.nInitCommuniterGroupCount; i++)
		{
			IncreaseCommuniterGroup();
		}

		CBaseWorker::Start();
		CEasylog::GetInstance()->warn("NetWork run success!");
	}
	void Stop()
	{
		if(m_Acceptor) m_Acceptor->Stop();
		m_CommuniterGroups.Clear();
		CBaseWorker::Stop();
	}
	void OnNewConnection(int32_t id,std::shared_ptr<CMessage> msg)
	{
		//这个锁会造成死锁，OnConnectionEvent事件
		//std::lock_guard<std::mutex>	gurd(m_netEventlock);

		CEasylog::GetInstance()->debug(__FUNCTION__);
		std::shared_ptr<CCommuniter> pNewCommuniter = std::make_shared<CCommuniter>();
		pNewCommuniter->SetID(id);

		AddConnection(pNewCommuniter);
		//
		pNewCommuniter->OnConnected();
	}
	void OnConnectionEvent(int32_t id, std::shared_ptr<CMessage> msg)
	{
		std::lock_guard<std::mutex>	gurd(m_netEventlock);

		CEasylog::GetInstance()->info(__FUNCTION__,"RecvID:",id,"MsgLen:",msg->GetSize());
		if (m_pNetEventHandler)
		{
			m_pNetEventHandler(id,msg->GetCommond(),msg->GetSize(),(char*)(msg->GetData().get()->Get()),msg->GetProtoType());
		}
		//连接关闭处理
		if (msg->GetCommond() == ClientMangerCmd::DISCONNECT)
		{
			m_events.Remove(id);
		}
	}

public:
	//
	virtual bool Work()
	{
		//get active ids
		std::vector<int32_t>	activeids;

		GetActiveIds(m_listenSocks, activeids);
		for(auto it : activeids)
		{
			int32_t activeID = it;

			if (m_Acceptor && activeID == m_Acceptor->GetID())
			{
				m_Acceptor->UpdateStatus(ConnectionStatus::CONNECT_STATUS_READ);
			}
			else
			{
				auto it = m_events.Get(activeID);
				bool bInvalid = false;
				if (!it.expired())
				{
					std::shared_ptr<CBaseConnection> pCur(it);
					if (pCur)
					{
						pCur->UpdateStatus(ConnectionStatus::CONNECT_STATUS_READ);
					}
					else bInvalid = true;

				}
				else bInvalid = true;

				if (bInvalid)
				{
					m_events.Remove(activeID);
				}
			}
		}

		//
		//return activeids.empty() ? false : true;
		return false;
	}

public:
	//外部接口
	int32_t Connect(std::string ip, int32_t port)
	{
		if (!CBaseWorker::IsRunning()) return -1;

		std::shared_ptr<CCommuniter> pNewCommuniter = std::make_shared<CCommuniter>();
		if(pNewCommuniter->Connect(ip, port))
		{
			AddConnection(pNewCommuniter);
			return pNewCommuniter->GetID();
		}
		
		return -1;
	}
	//
	void Disconnect(int32_t id)
	{
		if (!CBaseWorker::IsRunning()) return ;

		std::shared_ptr<CBaseConnection> pCur(m_events.Get(id));
		if (pCur) pCur->Disconnect();
	}
	bool SendData(int32_t id, int nCmd, const char* pInput, int nInputLength, ProtocolType nProtoType)
	{
		if (!CBaseWorker::IsRunning()) return false;

		auto it = m_events.Get(id);
		if (it.expired())
			return false;
		std::shared_ptr<CBaseConnection> pCur(it);
		if (pCur)
		{
			pCur->SendData(nCmd, pInput, nInputLength, nProtoType);
			return true;
		}
		return false;
	}

	//注册回调
	void RegisterEventHandler(cbEventHandler handler)
	{
		m_pNetEventHandler = handler;
	}
};

#endif