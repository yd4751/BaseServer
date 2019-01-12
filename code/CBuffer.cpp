#include "CBuffer.h"

void CBuffer::Put(const char& chr)
{
	if (m_nEnd == m_nBufEnd) m_nEnd = m_nBufStart;
	(*m_buffer)[m_nEnd++] = chr;
}


CBuffer::CBuffer(uint32_t nMaxSize) :m_nBufStart(0), m_nBufEnd(0), m_nBegin(0), m_nEnd(0)
{
	m_buffer = std::make_shared<CArray<char>>(nMaxSize);
	m_nBufStart = 0;
	m_nBufEnd = m_nBufStart + m_buffer->GetSize();
	m_nBegin = m_nBufStart;
	m_nEnd = m_nBegin;

};


uint32_t CBuffer::Size() {
	if (m_nEnd >= m_nBegin) return m_nEnd - m_nBegin;

	return (m_nBufEnd - m_nBegin + m_nEnd);
}
bool CBuffer::Write(const char* input, uint32_t len)
{
	if (len + Size() > m_buffer->GetSize())
	{
		//not enough space
		return false;
	}

	for (uint32_t i = 0; i < len; ++i)
	{
		Put(input[i]);
	}
	return true;
}
bool CBuffer::Write(CBuffer& pSrc)
{
	if (pSrc.Size() + Size() > m_buffer->GetSize())
	{
		//not enough space
		return false;
	}

	for (uint32_t i = 0; i < pSrc.Size(); ++i) Put(pSrc.Get());
	return true;
}

char CBuffer::Get()
{
	if (m_nBegin == m_nBufEnd) m_nBegin = m_nBufStart;
	return (*m_buffer)[m_nBegin++];
}

uint32_t CBuffer::Read(char* input, uint32_t len)
{
	uint32_t nGetLen = Size() > len ? len : Size();

	for (uint32_t i = 0; i < nGetLen; ++i) input[i] = Get();
	return nGetLen;
}


void CBuffer::operator >>(char& chr)
{
	chr = Get();
}
void CBuffer::operator << (std::string str)
{
	for (auto& it : str)
	{
		Put(it);
	}
}
void CBuffer::operator <<(char chr)
{
	Put(chr);
};