#ifndef __H_MESSAGE_DEFINE_H__
#define __H_MESSAGE_DEFINE_H__
#include <cstdint>
#include <string>
#include <map>
using std::string;
namespace MessageDefine
{
	/*
		ģ��RPC����װ���ò���ΪЭ��, (����������������ƣ�uint32_t  std::string   -- ��ʱֻ�������
		���ø�ʽ�� Call(nCmd,Param1,Param2,Param3 ...)
		�ص���ʽ:  OnCall(Param1,Param2,Param3 ...)
		��������RemoteServer�������Ͳ���Ҫ�ٷ�װЭ����
	*/
	/*
		LGS - LoginServer
		DBS - DataBaseServer
	*/
	/*
		�˻���¼��ÿ��������ÿ��Э��Ҫʵ��Parse��MakePacket���������Ҫʵ�֣�json 8���ӿڣ�pb 8���ӿ� ...
		ReqAccountLogin -> StreamDataToLGS -> ReplyFromLGSStreamData -> ReplyAccountLogin
		//�������ʵ�ֽṹ��ͨ�����л������RemoteServer�Ľ����Ϳ���ʡ�¹�����
	*/
	//##### ReplyBaseInfo	-  ͨ�ûظ�
	enum ReplyErrorCode
	{
		REPLY_CODE_SUCCESS = 0,
		REPLY_CODE_ERROR_UNDEFINED,
		REPLY_CODE_ACCOUNT_NOT_EXIST,			//�˻�������
		REPLY_CODE_ACCOUNT_ERROR_INFO,			//�˻���Ϣ�쳣
		REPLY_CODE_ACCOUNT_ERROR_PASSWORD,		//�˻��������
	};
	typedef struct ReplyBaseInfo
	{
		ReplyErrorCode code;
		ReplyBaseInfo()
		{
			code = ReplyErrorCode::REPLY_CODE_SUCCESS;
		}
	}ReplyBaseInfo;

	//##### AccountCreate  -  �˻�����
	//request
	typedef struct RequestAccountCreate
	{
		string					account;
		string					password;
		RequestAccountCreate() {}
		RequestAccountCreate(string account, string password)
		{
			this->account = account;
			this->password = password;
		}
	}RequestAccountCreate;
	//Reply
	typedef struct ReplyAccountCreate:
		public ReplyBaseInfo
	{
		uint32_t				id;
		string					account;
		
		ReplyAccountCreate() {}
	}ReplyAccountCreate;

	//##### AccountUpdate  -  �˻�����
	//request
	typedef struct RequestAccountUpdate
	{
		uint32_t				id;
		std::map<std::string, std::string>	updateInfo;
		RequestAccountUpdate()
		{
			id = -1;
			updateInfo.clear();
		}
		
	}RequestAccountUpdate;
	//Reply
	typedef struct ReplyAccountUpdate:
		public ReplyBaseInfo
	{
		uint32_t				id;
		ReplyAccountUpdate() {}
	}ReplyAccountUpdate;

	//##### AccountLogin  -  �˻���¼
	//AccountLogin
	//Request
	typedef struct RequestAccountLogin
	{
		string					account;
		string					password;
		RequestAccountLogin() {}
		RequestAccountLogin(string account, string password)
		{
			this->account = account;
			this->password = password;
		}
	}RequestAccountLogin;
	//Reply
	typedef struct ReplyAccountLogin:
		public ReplyBaseInfo
	{
		string					account;
		uint32_t				id;
		string					nick_name;

		ReplyAccountLogin() {}
		ReplyAccountLogin(string account)
		{
			this->account = account;
		}
	}ReplyAccountLogin;

	//AccountLogout
	//Request
	typedef struct RequestAccountLogout
	{
		string					account;
		RequestAccountLogout() {}
		RequestAccountLogout(string account)
		{
			this->account = account;
		}
	}RequestAccountLogout;
	//Reply
	typedef struct ReplyAccountLogout:
		public ReplyBaseInfo
	{
		string					account;
		ReplyAccountLogout() {}
		ReplyAccountLogout(string account)
		{
			this->account = account;
		}
	}ReplyAccountLogout;
};

#endif