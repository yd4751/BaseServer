#ifndef __H_NODE_MANAGER_H__
#define __H_NODE_MANAGER_H__

#include <map>
#include <vector>
#include <mutex>
#include <functional>

#define MAX_NODEMANAGER_CAPACITY		0


template<typename T1, typename T2>
class CNodeManager
{
	std::map<T1, T2>			m_nodes;
	std::mutex					m_lock;

	int32_t						m_nMaxNodeSize;

public:
	CNodeManager()
	{
		m_nodes.clear();
		//0表示不限制
		m_nMaxNodeSize = MAX_NODEMANAGER_CAPACITY;
	};

	virtual ~CNodeManager() {};

public:
	void SetMaxSize(int32_t nMaxSize)
	{
		m_nMaxNodeSize = nMaxSize;
	}
	bool Add(T1 key, T2 value)
	{
		std::lock_guard<std::mutex> gurd(m_lock);
		if (m_nMaxNodeSize > 0 && m_nMaxNodeSize == m_nodes.size())
			return false;

		auto it = m_nodes.find(key);
		if (it != m_nodes.end()) 
			return false;

		m_nodes[key] = value;
		return true;
	}
	bool Remove(T1 key)
	{
		std::lock_guard<std::mutex> gurd(m_lock);

		auto it = m_nodes.find(key);
		if (it == m_nodes.end()) return false;

		m_nodes.erase(key);
		return true;
	}
	void ForceAdd(T1 key, T2 value)
	{
		std::lock_guard<std::mutex> gurd(m_lock);
		if (m_nMaxNodeSize > 0 && m_nMaxNodeSize == m_nodes.size() && m_nodes.find(key) == m_nodes.end())
			return ;
		m_nodes[key] = value;
	}
	void Clear()
	{
		std::lock_guard<std::mutex> gurd(m_lock);
		m_nodes.clear();
	}
	T2 Get(T1 key)
	{
		std::lock_guard<std::mutex> gurd(m_lock);
		 
		auto it = m_nodes.find(key);
		if (it == m_nodes.end()) return T2();

		return m_nodes[key];
	}
	bool Exist(T1 key)
	{
		std::lock_guard<std::mutex> gurd(m_lock);
		auto it = m_nodes.find(key);
		if (it == m_nodes.end()) return false;
		return true;
	}
	bool IsFull()
	{
		std::lock_guard<std::mutex> gurd(m_lock);
		if (m_nMaxNodeSize == 0) return false;

		return m_nodes.size() == m_nMaxNodeSize ? true : false;
	}

	typedef std::function<bool(const T2 param)>  matchCondition;
	T2  GetMatch(matchCondition condition)
	{
		std::lock_guard<std::mutex> gurd(m_lock);
		for (auto it : m_nodes)
		{
			if (condition(it.second))
			{
				return it.second;
			}
		}

		return nullptr;
	}
	void GetKeys(std::vector<T1>& buf)
	{
		std::lock_guard<std::mutex> gurd(m_lock);
		for (auto it : m_nodes)
		{
			buf.emplace_back(it.first);
		}
	}
	int32_t Size()
	{
		std::lock_guard<std::mutex> gurd(m_lock);
		return m_nodes.size();
	}
	int32_t LeftSize()
	{
		//负数表示无穷大
		std::lock_guard<std::mutex> gurd(m_lock);
		return m_nMaxNodeSize - m_nodes.size();
	}
};

#endif