#ifndef __H_CONNECTION_H__
#define __H_CONNECTION_H__
#include "DataDefine.h"
#include "CBuffer.h"
#include "CEasyLog.h"
#include "CMessage.h"
#define DEFAULT_RREAD_WRITE_BUFFER_LEN		4096
#define DEFAULT_PACKET_SINGLE_MAX_LEN		4096
#define DEFAULT_PACKET_CACHE_COUNT			10
#define DEFAULT_PACKET_CODER_BUFFER			(DEFAULT_PACKET_SINGLE_MAX_LEN*2)

class CSendBuf
{
public:
	char			buf[DEFAULT_RREAD_WRITE_BUFFER_LEN];
	int32_t			nSendSize;
	int32_t			nCurSendSize;
	CSendBuf()
	{
		nSendSize = 0;
		nCurSendSize = 0;
	}
	char* GetBuf() { return buf+ nCurSendSize; }
	void SetSend(int32_t nSend) 
	{ 
		nSendSize = nSend; 
		nCurSendSize = 0;
	}
	int32_t GetLeftSize()
	{
		return nSendSize - nCurSendSize;
	}
	int32_t GetMaxSize()
	{
		return sizeof(buf)*sizeof(char);
	}
	void UpdateSendSize(int32_t nSend)
	{
		nCurSendSize += nSend;
		if (nCurSendSize == nSendSize)
		{
			nCurSendSize = nSendSize = 0;
		}
	}
	bool IsEmpty()
	{
		return nCurSendSize != nSendSize ? false : true;
	}

};
class CConnection:
	public CBaseConnection
{
	//Ñ­»·»º³å
	CBuffer				m_BufRead;
	CBuffer				m_BufWrite;
	//
	CSendBuf			m_Recv;
	CSendBuf			m_Send;
	//
	CPacket				m_PacketParse;

public:
	CConnection() :
		m_BufRead(DEFAULT_PACKET_SINGLE_MAX_LEN*DEFAULT_PACKET_CACHE_COUNT),
		m_BufWrite(DEFAULT_PACKET_SINGLE_MAX_LEN*DEFAULT_PACKET_CACHE_COUNT)
	{
	}
	virtual ~CConnection()
	{
		CEasylog::GetInstance()->warn("Desconstruct connection:",GetID());
	};

public:
	virtual uint32_t SendData(uint32_t nCmd, ProtocolType nProtoType, const std::string strInput)
	{
		return SendData(nCmd, strInput.c_str(), strInput.length(), nProtoType);
	}
	virtual uint32_t SendData(uint32_t nCmd, const char* pInput, uint32_t inputLength, ProtocolType nProtoType)
	{ 
		if(GetStatus() == ConnectionStatus::CONNECT_STATUS_WAIT_CLOSE || GetStatus() == ConnectionStatus::CONNECT_STATUS_CLOSED)
			return ErrorCode::ERROR_CODE_CONNECT_CLOSED;

		if (inputLength > m_BufWrite.EmptySize())
			return ErrorCode::ERROR_CODE_PACKET_BUFF_NOT_ENOUGH;

		if (!m_PacketParse.Fill(m_BufWrite, nCmd, pInput, inputLength, nProtoType))
			return ErrorCode::ERROR_CODE_PACKET_CODER_ERROR;

		//CEasylog::GetInstance()->info(__FUNCTION__,"SendSize:",data.length(),"CurSize:", m_BufWrite.Size());
		return ErrorCode::ERROR_CODE_NULL;
	};

public:
	bool Read() { 
		
		if (GetStatus() == ConnectionStatus::CONNECT_STATUS_READ)
		{
			int32_t nCurReadLen = CSocketInterface::GetInstance()->Read(GetID(), m_Recv.GetBuf(), m_Recv.GetMaxSize());
			if (nCurReadLen < 0)
			{
				//CEasylog::GetInstance()->error("error in read!", nCurReadLen);
				return false;
			}
			else if (nCurReadLen == 0)
			{
				UpdateStatus(ConnectionStatus::CONNECT_STATUS_WAIT_CLOSE);
				CEasylog::GetInstance()->warn(__FUNCTION__, "Connection closed!",GetID(), nCurReadLen);
				return true;
			}
			else
			{
				m_BufRead.Write(m_Recv.GetBuf(), nCurReadLen);
				if (!m_PacketParse.Parse(m_BufRead, m_curMessage))
				{
					CEasylog::GetInstance()->warn("Error packet");
					//UpdateStatus(ConnectionStatus::CONNECT_STATUS_WAIT_CLOSE);
					UpdateStatus(ConnectionStatus::CONNECT_STATUS_CLOSED);
					return true;
				}
				if (m_curMessage->GetStatus() == PacketParseStatus::PARSE_DONE)
				{
					if (m_handler)
					{
						CEasylog::GetInstance()->info("Handler request:", " Cmd:", m_curMessage->GetCommond());
						m_handler(GetID(), m_curMessage);
					}
				}
				
				UpdateStatus(ConnectionStatus::CONNECT_STATUS_WRITE);
				return true;
			}
		}

		return false; 
	};
	bool Write() { 
		if (m_BufWrite.Size() > 0 && GetStatus() != ConnectionStatus::CONNECT_STATUS_READ)
		{
			int32_t nWriteLen = 0;
			if (m_Send.GetLeftSize() > 0)
				nWriteLen = m_Send.GetLeftSize();
			else
			{
				nWriteLen = m_Send.GetMaxSize() > m_BufWrite.Size() ? m_BufWrite.Size() : m_Send.GetMaxSize();
				m_BufWrite.Read(m_Send.GetBuf(), nWriteLen);
				m_Send.SetSend(nWriteLen);
			}

			int32_t nCurWriteLen = CSocketInterface::GetInstance()->Write(GetID(), m_Send.GetBuf(), nWriteLen);
			if (nCurWriteLen < 0)
			{
				//CEasylog::GetInstance()->error("error in write!", nCurWriteLen);
				return false;
			}
			else if (nCurWriteLen == 0)
			{
				UpdateStatus(ConnectionStatus::CONNECT_STATUS_CLOSED);
				//CEasylog::GetInstance()->debug(__FUNCTION__, "Connection closed!", GetID());
				return true;
			}
			else
			{
				m_Send.UpdateSendSize(nCurWriteLen);
				CEasylog::GetInstance()->debug(__FUNCTION__, "Write data:", nCurWriteLen,"LeftSize:", m_Send.GetLeftSize());
				return true;
			}
		}
		else if(GetStatus() == ConnectionStatus::CONNECT_STATUS_WAIT_CLOSE)
		{
			UpdateStatus(ConnectionStatus::CONNECT_STATUS_CLOSED);
			return true;
		}
		return false; 
	};
};

#endif