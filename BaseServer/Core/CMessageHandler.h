#ifndef __H_CMESSAGE_HANDLER_H__
#define __H_CMESSAGE_HANDLER_H__
#include "global_include.h"


enum ReturnType
{
	Return_false=0,
	Return_true,
	Return_error,
};
class CMessage
{
public:
	int nCmd;
	int nMsgLength;
	NetCore::ProtocolType nProtoType;
	std::shared_ptr<CArray<char>> data;
	char* GetDataBuf() 
	{
		return data.get()->Get();
	}
};

class CMessageHandler
{
	typedef std::function<ReturnType(int, std::shared_ptr<CMessage>)> Dispatcher;
	typedef std::function<ReturnType(int, std::shared_ptr<CMessage>)>  MessageHandler;

	std::map<uint32_t, MessageHandler>			m_Messagehandler;
	Dispatcher									m_pNextDispatcher;

public:
	CMessageHandler() {};
	~CMessageHandler() {};

public:
	ReturnType Dispatch(int nClientID, std::shared_ptr<CMessage> msg)
	{
		auto it = m_Messagehandler.find(msg->nCmd);
		if (it == m_Messagehandler.end())
		{
			if (m_pNextDispatcher) return m_pNextDispatcher(nClientID, msg);

			return ReturnType::Return_error;
		}

		return it->second(nClientID, msg);
	}
	void RegisterNextDispatcher(Dispatcher pDispatcher) {
		m_pNextDispatcher = pDispatcher;
	}
	void RegisterMessage(uint32_t nCmd, MessageHandler pHandler)
	{
		m_Messagehandler[nCmd] = pHandler;
	};
	void UnRegisterMessage(uint32_t nCmd)
	{
		auto it = m_Messagehandler.find(nCmd);
		if (it == m_Messagehandler.end())
			return;

		m_Messagehandler.erase(it);
	};
};

#endif