#ifndef __H_CBUFFER_H__
#define __H_CBUFFER_H__
#include <memory>
#include <string>
#include "CArray.h"

class CBuffer
{
	std::shared_ptr<CArray<char>>	m_buffer;
	std::uint32_t					m_nBufStart;
	std::uint32_t					m_nBufEnd;

	std::uint32_t					m_nBegin;
	std::uint32_t					m_nEnd;

private:
	void Put(const char& chr);

public:

	CBuffer(uint32_t nMaxSize);

	virtual ~CBuffer() {};

public:
	void Reset()
	{
		m_nBufStart = 0;
		m_nBufEnd = m_buffer->GetSize();
		m_nBegin = 0;
		m_nEnd = 0;
	}
	uint32_t Size();
	uint32_t EmptySize() { return m_buffer->GetSize() - Size(); }
	bool Write(const char* input, uint32_t len);
	bool Write(CBuffer& pSrc);

	char Get();
	void get(char& chr) { chr = Get(); }
	uint32_t Read(char* input, uint32_t len);

public:
	void operator >>(char& chr);
	void operator <<(std::string str);
	void operator <<(char chr);
};

#endif