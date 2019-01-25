#ifndef __H_COMMUNITER_GROUPS_H__
#define __H_COMMUNITER_GROUPS_H__
#include "DataDefine.h"
#include "CCommuniterMgr.h"

class CCommuniterGroups
{
	CNodeManager<int32_t, std::shared_ptr<CCommuniterMgr>>		m_Groups;
	std::mutex													m_lock;

public:
	CCommuniterGroups()
	{
	};
	virtual ~CCommuniterGroups()
	{
		Clear();
		m_Groups.Clear();
	};
	void Add(int32_t id, std::shared_ptr<CCommuniterMgr> pNode)
	{
		std::lock_guard<std::mutex> gurd(m_lock);

		m_Groups.Add(id,pNode);
		pNode->Start();
	}
	void Remove(int32_t id)
	{
		std::lock_guard<std::mutex> gurd(m_lock);

		m_Groups.Remove(id);
	}
	int32_t Size()
	{
		return m_Groups.Size();
	}
	std::shared_ptr<CCommuniterMgr> GetEmpty()
	{
		std::lock_guard<std::mutex> gurd(m_lock);
		return m_Groups.GetMatch([](const std::shared_ptr<CCommuniterMgr> pCur) ->bool {
			return pCur->CanJoin();
		});
	}
	void Clear()
	{
		m_Groups.GetMatch([](const std::shared_ptr<CCommuniterMgr> pCur) {
			pCur->Stop();
			return false;
		});
		m_Groups.Clear();
	}

};

#endif