#ifndef __H_CDATACACHE_H__
#define __H_CDATACACHE_H__
#include <list>
#include <mutex>
#include <functional>
//#include <memory>

//class CCommuniter;
template<typename T>
class CDataCache
{
	std::mutex				m_lock;
	std::list<T>			m_cache;

public:
	CDataCache() {};
	~CDataCache() {};

public:
	void Add(T param)
	{
		std::lock_guard<std::mutex>	guard(m_lock);
		m_cache.emplace_back(param);
	}
	void Get(int32_t nCount, std::list<T>& out)
	{
		std::lock_guard<std::mutex>	guard(m_lock);
		int nGetSize = nCount > m_cache.size() ? m_cache.size() : nCount;
		//负数表示取全部
		if (nCount < 0) nGetSize = m_cache.size();

		while (nGetSize > 0)
		{
			out.emplace_back(m_cache.front());
			m_cache.pop_front();
			--nGetSize;
		}
	}

	typedef std::function<bool(T)>  cbCondition;
	void Do(cbCondition work)
	{
		std::lock_guard<std::mutex>	guard(m_lock);
		for (auto& it : m_cache)
		{
			if (!work(it)) continue;
			
			return;
		}
	}
	typedef std::function<void(std::list<T>& data)>  cbCondition2;
	void Do(cbCondition2 work)
	{
		std::lock_guard<std::mutex>	guard(m_lock);
		work(m_cache);
	}
};

#endif