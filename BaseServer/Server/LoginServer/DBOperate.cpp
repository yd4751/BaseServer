#include "CServer.h"

bool CServer::DBLogin(std::shared_ptr<CUser> pUser)
{
	//测试
	pUser->id = Tools::GetRandom(100000, 999999);//随机6位数id
	pUser->money = Tools::GetRandom(100, 100000);//随机携带
	pUser->sex = Tools::GetRandom(0, 2);//随机性别 0-未填写 1-男 2-女
	pUser->nickName = "大帽子:" + std::to_string(pUser->id);
	pUser->avatar = "www.google.com";
	return true;
};