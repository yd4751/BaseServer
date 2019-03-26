#ifndef __H_BASE_WOKER_H__
#define __H_BASE_WOKER_H__
#include <thread>
#include <functional>
#include <chrono>
#include "CEasyLog.h"

enum WokerStatus
{
	WORKER_STATUS_EXITED,
	WORKER_STATUS_RUN,
	WORKER_STATUS_WAIT_EXIT,
};
class CBaseWorker
{
	WokerStatus				m_emStatus;
	bool					m_bBlock;

	std::chrono::milliseconds	m_nextSleepTime;

public:

	CBaseWorker(bool bBlock = false)
	{
		m_emStatus = WORKER_STATUS_EXITED;
		m_bBlock = bBlock;
		m_nextSleepTime = std::chrono::milliseconds(1);
	};
	virtual ~CBaseWorker()
	{
		Stop();
	};
	bool IsRunning()
	{
		return m_emStatus == WokerStatus::WORKER_STATUS_RUN ? true : false;
	}

protected:
	void DoWork()
	{
		m_emStatus = WORKER_STATUS_RUN;
		CEasylog::GetInstance()->info("Run thread:",std::this_thread::get_id());
		while (true)
		{
			if (WORKER_STATUS_WAIT_EXIT == m_emStatus)
			{
				CEasylog::GetInstance()->info("Exit thread:", std::this_thread::get_id());
				m_emStatus = WORKER_STATUS_EXITED;
				return;
			}

			if (!Work())
			{
				//sleep
				std::this_thread::sleep_for(m_nextSleepTime);
			}
		}
	}

public:
	void SetNextSleepTime(uint32_t nMillTime)
	{
		if (nMillTime == 0)
		{
			CEasylog::GetInstance()->warn("next sleep time:",nMillTime);
		}
		m_nextSleepTime = std::chrono::milliseconds(nMillTime);
	}
	void Start()
	{
		if (m_emStatus == WORKER_STATUS_EXITED)
		{
			std::thread work(std::bind(&CBaseWorker::DoWork,this));
			if (m_bBlock)
				work.join();
			else
				work.detach();
		}
	}
	void Stop()
	{
		if (m_emStatus == WORKER_STATUS_RUN)
		{
			m_emStatus = WORKER_STATUS_WAIT_EXIT;
			CEasylog::GetInstance()->info("Wait thread exit... id:",std::this_thread::get_id());
			while (m_emStatus != WORKER_STATUS_EXITED)
			{
				//CEasylog::GetInstance()->info("Wait thread exit...");
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			CEasylog::GetInstance()->info("Wait thread exit success!",std::this_thread::get_id());
		}
		
	}
	//Ö´ÐÐÌå
	virtual bool Work() = 0;
};

#endif