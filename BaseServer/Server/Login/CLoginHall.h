#ifndef __H_LOGIN_HALL_H__
#define __H_LOGIN_HALL_H__
#include "global_include.h"

#include "CMessageHandler.h"
#include "ServerCmdDefine.h"
#include "ClientCmdDefine.h"
#include "CRemoteServer.h"
#include "CBaseUserManager.h"

class CNetCore;
class CRemoteServer;
#define DEFAULT_MESSAGE_PACKET_LEN_MAX			4096

/*
//��¼���,�����Ҫ֧�ֶ��ֱ�ʶ�����䡢�ֻ��š�uid�ȣ�����ô��һ�ε�¼��Ҫ�����ݿ⣬����redis��ṹֻ��Uid - UserInfo����
//���ݿ��ṹ
1���û���  uid �ǳ� ͷ��
2����Ȩ��  uid ��Ȩ���� Ψһ��ʶ ����ƾ֤
*/

/*
�û�״̬��
	(�ȴ���¼,�ɻ���ṹ����)
	�ѵ�¼
	��Ϸ��
	������
	�ȴ��˳�
//�ǳ��û�����
�߼��������Լ��û� -> ���´����û�Ϊ�ǳ�״̬ -> ֪ͨ��Ҫ�Ͽ��������� -> ����رջص��¼� -> ����ǳ�״̬�û�
�������ӱ����Ͽ� -> �û��Ƿ���Եǳ�
						->  ��  ->  ֱ�������û�
						->  ��  ->  �����û�Ϊ����״̬
ע��
������Ҫ���ǣ�����δ���ʱ���ִ������롢�ǳ���Ϣ�����
*/

/*
//��ҵ���߼��㽻���ӿ�
logic -> loginhall : UpdatePlaying()
logic -> loginhall : UpdateExited()
logic -> loginhall : UpdateUserInfo()   //json������map���ݽṹ
logic -> loginhall : SendData()

loginhall -> logic : Disconnect()
loginhall -> logic : Reconnect()
loginhall -> logic : UpdateUserInfo()   //json������map���ݽṹ
loginhall -> logic : OnMessage()

������Ϸ���ݽ����洢...  ������д����˵
//���ж�ʱ����ʵ�֣�������˼�ŵ��������
RegisterTimerHandler
AddTimer()		//�ײ�ֻά��timerid �� ʱ��������ԣ������ص������������ϲ����
DelTimer()

logic -> loginhall -> libnetcore
*/

class CLoginHall:
	public std::enable_shared_from_this<CLoginHall>,
	public CMessageHandler
{
	std::map<uint32_t, std::shared_ptr<CRemoteServer> >			m_RemoteServers;
	
	std::map <int32_t, int32_t>									m_ClientRequestCache;  //ConnectID - RequeCommon
	CBaseUserManager<CBaseUser>									m_UserManager;

public:
	//
	std::shared_ptr<CMessage>									m_curMessage;
public:
	CLoginHall()
	{
		m_curMessage = std::make_shared<CMessage>();
		m_curMessage->data = std::make_shared<CArray<char>>(DEFAULT_MESSAGE_PACKET_LEN_MAX);
	}
	~CLoginHall() {}
	static std::shared_ptr<CLoginHall> GetInstance()
	{
		static std::shared_ptr<CLoginHall> m_pLoginHall;
		if (!m_pLoginHall) m_pLoginHall = std::make_shared<CLoginHall>();
		return m_pLoginHall;
	}

public:
	void Start();

	static bool OnNetEventHandler(int nClientID, int nCmd, int nMsgLength, char* msgBuf, NetCore::ProtocolType type)
	{
		std::cout << __FUNCTION__ << "  ClientID:" << nClientID << "  nCmd:" << nCmd << std::endl;

		std::shared_ptr<CMessage> pCurMessage = CLoginHall::GetInstance()->m_curMessage;
		pCurMessage->nCmd = nCmd;
		pCurMessage->nMsgLength = nMsgLength;
		pCurMessage->nProtoType = type;
		//��ʼ��β��
		pCurMessage->GetDataBuf()[nMsgLength] = 0;
		memcpy(pCurMessage->GetDataBuf(), msgBuf, nMsgLength * sizeof(char));

		if (Return_true == CLoginHall::GetInstance()->Dispatch(nClientID, pCurMessage))
		{
			return true;
		}
		return false;
	}

protected:
	std::shared_ptr<CRemoteServer> GetRemoteServer(RemoteServerType type);

public:
	ReturnType Dispatch(int nClientID, std::shared_ptr<CMessage>);

	//RedisServer Reply
public:
	ReturnType OnRedisReplyLogin(int nClientID, std::shared_ptr<CMessage>);
	ReturnType OnRedisReplyLogout(int nClientID, std::shared_ptr<CMessage>);

	//DataBaseServer Reply
public:
	//Account: create update del
	ReturnType OnDataBaseReplyAccountCreate(int nClientID, std::shared_ptr<CMessage>);
	ReturnType OnDataBaseReplyAccountUpdate(int nClientID, std::shared_ptr<CMessage>);
	ReturnType OnDataBaseReplyAccountDelete(int nClientID, std::shared_ptr<CMessage>);
	//
	ReturnType OnDataBaseReplyLogin(int nClientID, std::shared_ptr<CMessage>);
	ReturnType OnDataBaseReplyLogout(int nClientID, std::shared_ptr<CMessage>);
	ReturnType OnDataBaseReplyGuestLogin(int nClientID, std::shared_ptr<CMessage>);

	//�ͻ���ҵ��
	//���ӵ�¼�ǳ�
public:
	//
	bool CheckIsLoginUser(const std::string account);
	//����Ƿ������˳�
	bool CheckCanLogout(std::shared_ptr<CBaseUser>) { return true; }
	ReturnType OnDisconnect(int nClientID, std::shared_ptr<CMessage>);
	ReturnType OnLogin(int nClientID, std::shared_ptr<CMessage>);
	ReturnType OnLogout(int nClientID, std::shared_ptr<CMessage>);
	ReturnType OnGuestLogin(int nClientID, std::shared_ptr<CMessage>);

	//�˻�����
public:
	ReturnType OnAccountCreate(int nClientID, std::shared_ptr<CMessage>);
	ReturnType OnAccountUpdate(int nClientID, std::shared_ptr<CMessage>);
	ReturnType OnAccountDelete(int nClientID, std::shared_ptr<CMessage>);

	//�ű�
public:
	void UpdatePlaying(int32_t nUid);
	void UpdateExited(int32_t nUid);
	void UpdateUserInfo(int32_t nUid, const std::map<std::string, CValueInfo>& info);
	void SendData(int32_t nUid, int32_t nCmd, std::string msg);
};
#endif