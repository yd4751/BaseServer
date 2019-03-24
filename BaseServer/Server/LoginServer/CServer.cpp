#include "CServer.h"


CServer::CServer()
{
	m_bRunning = false;
	m_type = ServerType::SERVER_LOGIN;
}


CServer::~CServer()
{
}
bool CServer::OnNetEventHandler(int nClientID, int nCmd, int nMsgLength, char* msgBuf, NetCore::ProtocolType type)
{
	std::cout << __FUNCTION__ << "  ClientID:" << nClientID << "  nCmd:" << nCmd << std::endl;

	std::shared_ptr<CMessage> pCurMessage = CServer::GetInstance()->m_curMessage;
	pCurMessage->nCmd = nCmd;
	pCurMessage->nMsgLength = nMsgLength;
	pCurMessage->nProtoType = type;
	//��ʼ��β��
	pCurMessage->GetDataBuf()[nMsgLength] = 0;
	memcpy(pCurMessage->GetDataBuf(), msgBuf, nMsgLength * sizeof(char));

	if (Return_true == CServer::GetInstance()->Dispatch(nClientID, pCurMessage))
	{
		return true;
	}

	return false;
}
ReturnType CServer::OnConnect(int, std::shared_ptr<CMessage>)
{
	return ReturnType();
}
ReturnType CServer::OnDisconnect(int, std::shared_ptr<CMessage>)
{
	return ReturnType();
}
;
void CServer::Init()
{
}
void CServer::Start()
{
	if (m_bRunning)  return;

	//�����ļ�
	if (!m_config.Init())
	{
		std::cout << "init config fail!" << std::endl;
		assert(false);
		return;
	}
	//��־
	CEasylog::GetInstance()->Init("server.log", LOGLEVEL_DEBUG);
	//��Ϣע��
	Init();
	//����ģ��
	NetCore::Config(m_config.bindPort);
	NetCore::RegisterEvnetHandler(&CServer::OnNetEventHandler);
	NetCore::Start();

	//ע�ᵽ���ķ�����
	if (!RegisterToServer(ServerType::SERVER_CENTER, m_config.registerIP, m_config.registerPort, m_config.bindPort, NS_Center::Request::Register))
	{
		XWARN("Reigster server failed!");
		assert(false);
	}

	m_bRunning = true;
}

void CServer::Stop()
{
	if (!m_bRunning) return;

	NetCore::Stop();
	m_bRunning = false;
}
