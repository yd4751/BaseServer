#ifndef __H_MESSAGE_DEFINE_H__
#define __H_MESSAGE_DEFINE_H__
#include <cstdint>
#include <string>
#include <map>
using std::string;
namespace MessageDefine
{
	/*
		模拟RPC，封装调用参数为协议, (这里参数有类型限制：uint32_t  std::string   -- 暂时只封简单数据
		调用格式： Call(nCmd,Param1,Param2,Param3 ...)
		回调格式:  OnCall(Param1,Param2,Param3 ...)
		这样，和RemoteServer交互，就不需要再封装协议了
	*/
	/*
		LGS - LoginServer
		DBS - DataBaseServer
	*/
	/*
		账户登录：每个包根据每种协议要实现Parse、MakePacket，这里就需要实现：json 8个接口，pb 8个接口 ...
		ReqAccountLogin -> StreamDataToLGS -> ReplyFromLGSStreamData -> ReplyAccountLogin
		//如果可以实现结构体通用序列化，则和RemoteServer的交互就可以省下工作量
	*/
	//##### ReplyBaseInfo	-  通用回复
	enum ReplyErrorCode
	{
		REPLY_CODE_SUCCESS = 0,
		REPLY_CODE_ERROR_UNDEFINED,
		REPLY_CODE_ACCOUNT_NOT_EXIST,			//账户不存在
		REPLY_CODE_ACCOUNT_ERROR_INFO,			//账户信息异常
		REPLY_CODE_ACCOUNT_ERROR_PASSWORD,		//账户密码错误
	};
	typedef struct ReplyBaseInfo
	{
		ReplyErrorCode code;
		ReplyBaseInfo()
		{
			code = ReplyErrorCode::REPLY_CODE_SUCCESS;
		}
	}ReplyBaseInfo;

	//##### AccountCreate  -  账户创建
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

	//##### AccountUpdate  -  账户更新
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

	//##### AccountLogin  -  账户登录
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