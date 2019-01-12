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

public:
	CBaseWorker(bool bBlock = false)
	{
		m_emStatus = WORKER_STATUS_EXITED;
		m_bBlock = bBlock;
	};
	virtual ~CBaseWorker()
	{
		Stop();
		/*while (m_emStatus != WORKER_STATUS_EXITED)
		{
			CEasylog::GetInstance()->info("Wait thread exit...");
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		CEasylog::GetInstance()->info("Wait thread exit success!");*/
	};
	bool IsRunning()
	{
		return m_emStatus == WokerStatus::WORKER_STATUS_RUN ? true : false;
	}

protected:
	void DoWork()
	{
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
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
	}

public:
	void Start()
	{
		if (m_emStatus == WORKER_STATUS_EXITED)
		{
			std::thread work(std::bind(&CBaseWorker::DoWork,this));
			m_emStatus = WORKER_STATUS_RUN;
			if (m_bBlock)
				work.join();
			else
				work.detach();
		}
	}
	void Stop()
	{
		if(m_emStatus == WORKER_STATUS_RUN)
			m_emStatus = WORKER_STATUS_WAIT_EXIT;
	}
	//Ö´ÐÐÌå
	virtual bool Work() = 0;
};

#endif