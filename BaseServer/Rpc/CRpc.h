#ifndef __H_CRPC_H__
#define __H_CRPC_H__
#include <stdint.h>
#include <memory>
#include "CBuffer.h"

#define RPC_PARAM_BUF_LEN_MAX				2048

class CRpc
{
	CBuffer				m_paramBuf;
	char*				m_pSendBuf;

private:
	template<typename T>
	void SerializeParam(T);

	template<typename T, typename... Rest>
	void SerializeParam(T first, Rest... rest)
	{
		SerializeParam(first);
		SerializeParam(rest...);
	};

public:
	CRpc():m_paramBuf(RPC_PARAM_BUF_LEN_MAX)
	{
		m_pSendBuf = nullptr;
	}
	CRpc(char* pRecvBuf,uint32_t nRecvLength) :m_paramBuf(RPC_PARAM_BUF_LEN_MAX)
	{
		m_paramBuf.Write(pRecvBuf, nRecvLength);
	}

	void InitSendBuf(char* pSend) { m_pSendBuf = pSend; };
	template<typename T, typename... Rest>
	void Call(T first, Rest... rest)
	{
		m_paramBuf.Reset();
		SerializeParam(first, rest...);
		SendCall(m_paramBuf.Size());
	}
	template<typename T, typename... Rest>
	uint32_t SerializeToBuf(char* pInput, T first, Rest... rest)
	{
		m_paramBuf.Reset();
		SerializeParam(first, rest...);
		uint32_t nSize = m_paramBuf.Size();
		m_paramBuf.Read(pInput, nSize);
		return nSize;
	}

	template<typename T>
	void operator >> (T&);
public:
	void GetSerializeData(char* pDstBuf,uint32_t length)
	{
		m_paramBuf.Read(pDstBuf, length);
	};
	virtual void SendCall(uint32_t nLength) {};
};
#endif