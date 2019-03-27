#include "CServer.h"

std::shared_ptr<CUser> CServer::GetUserByUserID(int uid)
{
	return m_users.Get(uid);
};
std::shared_ptr<CUser> CServer::GetUserByConnectID(int fd)
{
	return m_users.GetMatch([&fd](std::shared_ptr<CUser> param) {
		if (param->fd == fd)
			return true;

		return false;
	});
};
void CServer::UpdatePlaying(uint32_t uid)
{
	XINFO(__FUNCTION__);
};
void CServer::UpdateExited(uint32_t uid)
{
	XINFO(__FUNCTION__);
};
void CServer::UpdateUserInfo(uint32_t uid, std::string info)
{
	XINFO(__FUNCTION__);
};
void CServer::SendData(uint32_t uid, uint32_t cmd, std::string message)
{
	XINFO(__FUNCTION__);
	auto pUser = GetUserByUserID(uid);
	assert(pUser != nullptr);
	
	Send(pUser->fd, cmd, message);
};