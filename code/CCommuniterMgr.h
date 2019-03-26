#ifndef __H_COMMUNITER_MGR_H__
#define __H_COMMUNITER_MGR_H__
#include "DataDefine.h"
#include "CCommuniter.h"

class CCommuniterMgr:
	public CBaseWorker
{
	CNodeManager<int32_t, std::shared_ptr<CCommuniter>>		m_connections;

public:
	CCommuniterMgr():CBaseWorker(false)
	{
	};
	virtual ~CCommuniterMgr()
	{
		Stop();
	};

public:
	void Start()
	{
		CBaseWorker::Start();
	}
	void Stop()
	{
		m_connections.Clear();
		CBaseWorker::Stop();
	}
	bool CanJoin()
	{
		return !m_connections.IsFull();
	}
	void SetMaxSize(int32_t nMax)
	{
		m_connections.SetMaxSize(nMax);
	}
	void Add(int32_t id, std::shared_ptr<CCommuniter> node)
	{
		m_connections.Add(id,node);
	}
	void Remove(int32_t id)
	{
		m_connections.Remove(id);
	}
	int32_t Size()
	{
		return m_connections.Size();
	}

public:
	virtual bool Work();
	
};

#endif