#include "CServerBase.h"

CServerBase::CServerBase() 
{
	m_curMessage = std::make_shared<CMessage>();
	m_curMessage->data = std::make_shared<CArray<char>>(DEFAULT_MESSAGE_PACKET_LEN_MAX);
	m_type = ServerType::SERVER_INVALID;
	RegisterMessage(NetCore::CONNECT, std::bind(&CServerBase::OnConnect, this, std::placeholders::_1, std::placeholders::_2));
	RegisterMessage(NetCore::DISCONNECT, std::bind(&CServerBase::OnDisconnect, this, std::placeholders::_1, std::placeholders::_2));
}

void CServerBase::Send(int fd,int cmd, Json::Value & packet)
{
	std::string str = packet.toStyledString();
	NetCore::SendData(fd, cmd, str.c_str(), str.length(), NetCore::ProtocolType::PROTO_TYPE_JSON);
}
void CServerBase::Send(int fd, int cmd, std::string str)
{
	NetCore::SendData(fd, cmd, str.c_str(), str.length(), NetCore::ProtocolType::PROTO_TYPE_JSON);
}
void CServerBase::Send(int fd, int cmd)
{
	NetCore::SendData(fd, cmd, nullptr, 0, NetCore::ProtocolType::PROTO_TYPE_JSON);
}
bool CServerBase::RegisterToServer(ServerType type, const std::string& ip, uint32_t port, uint32_t bindPort, int cmd,int cmdStart,int cmdEnd)
{
	m_nodeServers.Add(type, ip, port);

	NS_Center::ReqRegister req;
	req.type = (int)m_type;
	req.ip = Tools::GetLocalIP();
	req.port = bindPort;
	req.cmdStart = cmdStart;
	req.cmdEnd = cmdEnd;

	m_nodeServers.Send(type, cmd,ProtoParseJson::MakePacket<NS_Center::ReqRegister>(req));
	return true;
};
ReturnType CServerBase::OnRegisterSuccess(int fd, std::shared_ptr<CMessage> msg)
{
	return ReturnType::Return_true;
};