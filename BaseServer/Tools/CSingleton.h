#ifndef __H_C_SINGLETON__H__
#define __H_C_SINGLETON__H__
#include <memory>
#include <mutex>

template<typename T>
class CSingleton
{
public:
	CSingleton(const CSingleton&) {};//½ûÖ¹¿½±´
	CSingleton& operator=(const CSingleton&) {};//½ûÖ¹¸³Öµ

	CSingleton(){};
	virtual ~CSingleton(){};

public:
	static std::shared_ptr<T> GetInstance()
	{
		static std::mutex m_lock;
		static std::shared_ptr<T>	m_pInstance;
		//Ë«ÖØËø»úÖÆ 
		if (!m_pInstance)
		{
			std::lock_guard<std::mutex> gurd(m_lock);
			if (!m_pInstance)
			{
				m_pInstance = std::make_shared<T>();
			}
		}

		return m_pInstance;
	}
};

#endif