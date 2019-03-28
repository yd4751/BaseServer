#ifndef __H_BASE_CONNECTION_H__
#define _H_BASE_CONNECTION_H__
#include <stdint.h>
#include <memory>
#include <functional>
#include "CMessage.h"
#include "CSocketInterface.h"
#include "CNetCoreInterface.h"

class CMessage;
typedef std::function<void(int32_t,std::shared_ptr<CMessage>)>  callBackMessage;

enum ConnectionStatus
{
	CONNECT_STATUS_INIT,
	CONNECT_STATUS_WRITE,
	CONNECT_STATUS_READ,
	CONNECT_STATUS_WAIT_CLOSE,
	CONNECT_STATUS_CLOSED,
};
class CBaseConnection
{
	ConnectionStatus				m_status;

public:
	bool bServer = false;
	std::shared_ptr<CSocketInfo >  m_socket = nullptr;

	CBaseConnection()
	{
		CEasylog::GetInstance()->info("new Construct Connection!");
		m_status = CONNECT_STATUS_INIT;
		m_curMessage = std::make_shared<CMessage>();
	};
	virtual ~CBaseConnection() 
	{
		CSocketInterface::GetInstance()->Close(m_socket->fd);
	};

public:
	void UpdateStatus(ConnectionStatus status)
	{
		switch (m_status)
		{
		case CONNECT_STATUS_WAIT_CLOSE:
		case CONNECT_STATUS_CLOSED:
			CEasylog::GetInstance()->info("update close status! ", status);
			if (status == CONNECT_STATUS_WRITE || status == CONNECT_STATUS_READ)
				return;
		}

		//²»´øËø
		m_status = status;
	}
	ConnectionStatus GetStatus()
	{
		return m_status;
	}
	bool CheckInStatus(ConnectionStatus status)
	{
		return m_status == status ? true : false;
	}
	void RegisterHandler(callBackMessage handler)
	{
		m_handler = handler;
	}
	int32_t GetID() { return m_socket->fd; }

	callBackMessage					m_handler;
	std::shared_ptr<CMessage>		m_curMessage;

	virtual uint32_t SendData(std::string data) { return 0; };
	virtual uint32_t SendData(uint32_t nCmd, ProtocolType nProtoType, const std::string strInput) { return ErrorCode::ERROR_CODE_NULL; }
	virtual uint32_t SendData(uint32_t nCmd, const char* pInput, uint32_t inputLength, ProtocolType nProtoType) { return ErrorCode::ERROR_CODE_NULL; }

	void Disconnect()
	{
		UpdateStatus(ConnectionStatus::CONNECT_STATUS_WAIT_CLOSE);
	}
	void OnClosed()
	{
		if (m_handler)
		{
			m_curMessage->SetSize(0);
			m_curMessage->SetCommond(ClientMangerCmd::DISCONNECT);
			m_handler(GetID(), m_curMessage);
		}
	}
	void OnConnected()
	{
		if (m_handler)
		{
			m_curMessage->SetSize(0);
			m_curMessage->SetCommond(ClientMangerCmd::CONNECT);
			m_handler(GetID(), m_curMessage);
		}
		m_status = CONNECT_STATUS_WRITE;
	}
};

#endif