#ifndef __H_TIMERTASK_H__
#define __H_TIMERTASK_H__
#include "global_include.h"

//bool  true ��������  false ֹͣ����
typedef std::function<bool()>		cbTimerTask;

class CTimerTask:
	public CSingleton<CTimerTask>
{
	std::map<int32_t, cbTimerTask>			m_TaskManager;

public:
	CTimerTask()
	{
		NetCore::RegisterTimerHandler(&CTimerTask::TimerHandler);
	}
	~CTimerTask()
	{
		Stop();
	}
protected:
	void Stop()
	{
		for (auto it : m_TaskManager)
		{
			NetCore::CancelTimer(it.first);
		}
	}

public:
	static void TimerHandler(int timerID)
	{
		CTimerTask::GetInstance()->OnTimer(timerID);
	}

public:
	int32_t AddTask(cbTimerTask handler,int32_t expiredTime)
	{
		if (handler == nullptr) return -1;
		
		int32_t nTaskID = NetCore::AddTimer(expiredTime);
		m_TaskManager[nTaskID] = handler;
		return nTaskID;
	};
	void RemoveTask(int32_t nTaskId)
	{
		auto it = m_TaskManager.find(nTaskId);
		if (it == m_TaskManager.end())
			return;

		NetCore::CancelTimer(nTaskId);
		m_TaskManager.erase(it);
	}
	
	void OnTimer(int timerID)
	{
		auto it = m_TaskManager.find(timerID);
		if (it != m_TaskManager.end())
		{
			if (it->second())
			{
				//��������
				NetCore::ResetTimer(timerID);
			}
			else
			{
				m_TaskManager.erase(it);
			}
		}
	}


};
#endif