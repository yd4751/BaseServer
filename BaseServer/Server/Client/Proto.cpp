#include "CServer.h"

ReturnType CServer::OnAuthSucess(int, std::shared_ptr<CMessage>)
{
	XDEBUG(__FUNCTION__);
	return ReturnType::Return_true;
}

ReturnType CServer::OnLoginSucess(int, std::shared_ptr<CMessage> msg)
{
	XDEBUG(__FUNCTION__);
	NS_Login::ReplyUserLogin reply;
	reply = ProtoParseJson::Parse<NS_Login::ReplyUserLogin>(msg);
	m_nUserID = reply.id;

	//µÇÂ¼ÓÎÏ·
	SendGameLogin(m_nUserID);
	return ReturnType();
}

ReturnType CServer::OnGuestLoginSucess(int, std::shared_ptr<CMessage>)
{
	return ReturnType();
}

void CServer::SendAuth()
{
	m_nodeServers.Send(ServerType::SERVER_GATE, NS_Gate::Request::Auth);
};

void CServer::SendLogin()
{
	Json::Value value;
	value["test"] = "hello!";
	m_nodeServers.Send(ServerType::SERVER_GATE, NS_Login::Request::Login, value.toStyledString());
}
void CServer::SendGuestLogin()
{
	m_nodeServers.Send(ServerType::SERVER_GATE, NS_Login::Request::GuestLogin);
}
void CServer::SendGameLogin(int id)
{
	NS_Game::ReqLogin req;
	req.id = id;
	m_nodeServers.Send(ServerType::SERVER_GATE, NS_Game::Request::Login,ProtoParseJson::MakePacket<NS_Game::ReqLogin>(req));
}
void CServer::SendRoomLogin()
{
	m_nodeServers.Send(ServerType::SERVER_GATE, 401001);
}
ReturnType CServer::OnGameLogin(int, std::shared_ptr<CMessage>)
{
	XINFO(__FUNCTION__);
	SendRoomLogin();
	return ReturnType();
}
ReturnType CServer::OnRoomLogin(int, std::shared_ptr<CMessage>)
{
	XINFO(__FUNCTION__);
	return ReturnType();
}
;