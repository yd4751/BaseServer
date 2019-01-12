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
//登录相关,如何需要支持多种标识（邮箱、手机号、uid等），那么第一次登录需要走数据库，这样redis表结构只用Uid - UserInfo即可
//数据库表结构
1、用户表  uid 昵称 头像
2、授权表  uid 授权类型 唯一标识 密码凭证
*/

/*
用户状态：
	(等待登录,由缓存结构处理)
	已登录
	游戏中
	离线中
	等待退出
//登出用户处理
逻辑层清理自己用户 -> 更新大厅用户为登出状态 -> 通知需要断开网络连接 -> 网络关闭回调事件 -> 清理登出状态用户
网络连接被动断开 -> 用户是否可以登出
						->  是  ->  直接清理用户
						->  否  ->  更新用户为离线状态
注：
这里需要考虑，流程未完成时，又触发登入、登出消息的情况
*/

/*
//与业务逻辑层交互接口
logic -> loginhall : UpdatePlaying()
logic -> loginhall : UpdateExited()
logic -> loginhall : UpdateUserInfo()   //json或类似map数据结构

loginhall -> logic : Disconnect()
loginhall -> logic : Reconnect()
loginhall -> logic : UpdateUserInfo()   //json或类似map数据结构

其他游戏数据交互存储...  待定，写到再说
*/

class CLoginHall:
	public std::enable_shared_from_this<CLoginHall>,
	public CMessageHandler
{
	std::map<uint32_t, std::shared_ptr<CRemoteServer> >			m_RemoteServers;
	
	//std::map<int32_t,CUesrIdentify>							m_PreLoginUser;		//等待登录结果玩家，  UserID/ACCOUNT - ConnectID
	std::map<std::string,int32_t>								m_PreLoginUser;		//等待登录结果玩家，  UserID/ACCOUNT - ConnectID
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

	//RemoteServer Reply
public:
	ReturnType OnRedisReplyLogin(int nClientID, std::shared_ptr<CMessage>);
	ReturnType OnRedisReplyLogout(int nClientID, std::shared_ptr<CMessage>);

	//连接登录登出
public:
	//玩家是否允许退出
	bool CheckCanLogout(std::shared_ptr<CBaseUser>) { return true; }
	//
	ReturnType OnDisconnect(int nClientID, std::shared_ptr<CMessage>);
	//
	ReturnType OnLogin(int nClientID, std::shared_ptr<CMessage>);
	//
	ReturnType OnLogout(int nClientID, std::shared_ptr<CMessage>);

};
#endif