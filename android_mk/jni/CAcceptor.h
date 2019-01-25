#ifndef __H_ACCEPTOR_H__
#define __H_ACCEPTOR_H__
//#include "CBaseConnection.h"
#include "DataDefine.h"
#include "CEasyLog.h"

class CAcceptor:
	public CBaseConnection,
	public CBaseWorker
{


public:
	CAcceptor()
	{
	};
	virtual ~CAcceptor()
	{
		Stop();
	};

public:
	void Start(int32_t nPort)
	{
		int32_t nListenSockid = CSocketInterface::GetInstance()->CreateListen("", nPort) ;
		if (nListenSockid < 0)
		{
			CEasylog::GetInstance()->error("Fail to listen, port:", nPort);
			assert(false);
		}
		SetID(nListenSockid);
		CEasylog::GetInstance()->info("ListenPort:", nPort,"SockID:",nListenSockid);
		CBaseWorker::Start();
	}
	void Stop()
	{
		Disconnect();
		CSocketInterface::GetInstance()->Close(GetID());
		CBaseWorker::Stop();
	}
	//
	virtual bool Work()
	{
		if (GetStatus() == ConnectionStatus::CONNECT_STATUS_READ)
		{
			//��ȡ������
			int32_t nNewID = CSocketInterface::GetInstance()->Accept();
			CEasylog::GetInstance()->info(__FUNCTION__, "GetNewConnection:", nNewID);
			if (m_handler)
			{
				m_handler(nNewID,m_curMessage);
			}
			UpdateStatus(ConnectionStatus::CONNECT_STATUS_WRITE);
			return true;
		}
		//���¿�����˯һ��
		return false;
	}
	
};

#endif