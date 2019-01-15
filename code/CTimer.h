#ifndef __H_CTIMER_H__
#define __H_CTIMER_H__
#include "DataDefine.h"
#include "CNetCoreInterface.h"

#define MAX_TIMER_COUNT				10000

typedef struct TimerInfo
{
	int32_t			id;
	int32_t			expiredTime;
	int32_t			nCurExpiredTime;

	TimerInfo(int32_t nTimerID,int32_t expired)
	{
		id = nTimerID;
		expiredTime = expired;
		nCurExpiredTime = 0;
	}
	TimerInfo()
	{
		id = -1;
		expiredTime = 0;
		nCurExpiredTime = 0;
	}
	void UpdateExpired(int32_t nDelayTime)
	{
		nCurExpiredTime += nDelayTime;
	}
	int32_t GetRemainTime() 
	{
		return expiredTime - nCurExpiredTime;
	}
	void ResetExpired()
	{
		nCurExpiredTime = 0;
	}

}TimerInfo;

class CTimer:
	public CSingleton<CTimer>,
	public CBaseWorker
{
	int32_t								m_curTimerID;
	int32_t								m_nCurDelyTime;
	int32_t								m_nextDelyTime;

	cbTimerHandler						m_pHandler;
	//这样性能可能不太高，先凑活用
	CNodeManager<int32_t, std::shared_ptr<TimerInfo>>		m_Timers;

public:
	CTimer()
	{
		m_nCurDelyTime = 0;
		m_nextDelyTime = 0;
		m_curTimerID = 0;
		m_Timers.Clear();
	}
	~CTimer()
	{
		m_Timers.Clear();
		Stop();
	}

public:
	void Start()
	{
		CBaseWorker::Start();
	}
	void Stop()
	{
		CBaseWorker::Stop();
	}
	virtual bool Work()
	{
		while (true)
		{
			//
			m_nCurDelyTime = m_nextDelyTime;

			std::shared_ptr<TimerInfo > pCur = m_Timers.GetMatch([&](std::shared_ptr<TimerInfo > cur)->bool {
				cur->UpdateExpired(this->m_nCurDelyTime);
				int32_t nRemainTime = cur->GetRemainTime();
				this->m_nextDelyTime = nRemainTime <= this->m_nextDelyTime ? nRemainTime : this->m_nextDelyTime;
				return nRemainTime <= 0 ? true : false;
			});

			if (pCur)
			{
				if (m_pHandler)
				{
					std::lock_guard<std::mutex> gurd(m_netEventlock);

					m_pHandler(pCur->id);
				}
				//回调中可能会重置timer
				if(pCur->GetRemainTime() <= 0)
				{
					m_Timers.Remove(pCur->id);
				}
				m_nextDelyTime = 0;
				continue;
			}
			else
			{
				if (this->m_nextDelyTime == 0) this->m_nextDelyTime = 10;
				std::this_thread::sleep_for(std::chrono::milliseconds(this->m_nextDelyTime));
			}
		}
	};

protected:
	int32_t GetValidID()
	{
		int32_t nStartID = m_curTimerID + 1;
		while(true)
		{
			if(!m_Timers.Exist(nStartID))
				break;

			++nStartID;
			if (nStartID > MAX_TIMER_COUNT)
			{
				nStartID = 0;
				break;
			}

			if (nStartID == m_curTimerID)
			{
				return -1;
			}
		}

		m_curTimerID = nStartID;
		return m_curTimerID;
	}

public:
	void RegisterTimerHandler(cbTimerHandler handler)
	{
		m_pHandler = handler;
	};
	int	 AddTimer(int expiredTime)
	{
		int32_t nNewID = GetValidID();
		m_Timers.Add(nNewID,std::make_shared<TimerInfo>(nNewID, expiredTime));
		return nNewID;
	};
	void CancelTimer(int timerID)
	{
		m_Timers.Remove(timerID);
	};
	void ResetTimer(int timerID)
	{
		std::shared_ptr<TimerInfo > pCur = m_Timers.Get(timerID);
		if (pCur)
		{
			pCur->ResetExpired();
		}
	};
	int  GetTimerReaminTime(int timerID)
	{
		std::shared_ptr<TimerInfo > pCur = m_Timers.Get(timerID);
		if (pCur)
		{
			return pCur->GetRemainTime();
		}

		return -1;
	};
};
#endif