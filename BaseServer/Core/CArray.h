#ifndef __H_C_ARRAY_H__
#define __H_C_ARRAY_H__
#include <stdint.h>
#include <cassert>
#include <memory>
#include <string.h>
template<typename T>
class CArray
{
	T*		m_data;
	std::uint32_t	m_nMaxLength;
public:
	CArray(std::uint32_t nMaxLen)
	{
		m_nMaxLength = nMaxLen;
		m_data = new T[nMaxLen];
		memset(m_data,0, sizeof(T)*nMaxLen);
	};
	virtual ~CArray()
	{
		delete m_data;
		m_data = nullptr;
	};

	std::uint32_t GetSize()
	{
		return m_nMaxLength;
	}
	T* Get()
	{
		return m_data;
	}
	T& operator[](std::uint32_t nIndex)
	{
		if (m_nMaxLength <= nIndex || nIndex < 0) abort();
		return m_data[nIndex];
	}
};

#endif