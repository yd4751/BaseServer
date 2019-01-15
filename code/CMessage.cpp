#include "CMessage.h"
#include "CEasyLog.h"
//#include <cstdint>
#include <string.h>


bool CMessage::AllocDataSpace(uint32_t nMaxLength)
{
	//已分配空间
	if (m_data)
	{
		return false;
	}

	m_data = std::make_shared<CArray<uint8_t>>(nMaxLength);
	if (!m_data)
	{
		return false;
	}

	return true;
}
void CMessage::Reset()
{
	m_cmd = 0;
	m_length = 0;
	m_PortoType = ProtocolType::PROTO_TYPE_JSON;
	m_CoderType = CoderType::CODER_TYPE_NULL;

	m_StatusParse = PacketParseStatus::PARSE_HEAD;
}

bool  CPacket::CheckHead(const PacketHead& head)
{
	if (CoderType::CODER_TYPE_NULL >= head.type_coder || head.type_coder >= CoderType::CODER_TYPE_MAX)
		return false;

	if (ProtocolType::PROTO_TYPE_NULL >= head.type_proto || head.type_proto >= ProtocolType::PROTO_TYPE_MAX)
		return false;

	return true;
};
///
PacketHead CPacket::ParseHead(CBuffer& data)
{
	PacketHead headinfo;

	headinfo.type_coder = NetWorkSerilize::DeSerilizeUint<uint16_t>(data);
	headinfo.type_proto = NetWorkSerilize::DeSerilizeUint<uint16_t>(data);
	headinfo.cmd = NetWorkSerilize::DeSerilizeUint<uint32_t>(data);
	headinfo.length = NetWorkSerilize::DeSerilizeUint<uint32_t>(data);

	std::ostringstream str;
	str <<"["<<__FUNCTION__<<"]"<< headinfo.type_coder << " : " << headinfo.type_proto << " : " << " : " << headinfo.cmd << " : " << headinfo.length << "\n";
	CEasylog::GetInstance()->debug(str.str());
	return headinfo;
};
bool CPacket::ParseData(CBuffer& data, std::shared_ptr<CMessage> msg)
{
	std::ostringstream str;
	str <<"["<<__FUNCTION__<<"]"<< msg->GetSize() << "\n";
	CEasylog::GetInstance()->debug(str.str());

	//解码数据
	data.Read((char*)msg->GetData()->Get(), msg->GetSize());

	std::shared_ptr<CArray<uint8_t>> pDst(msg->GetData());
	uint32_t nDecodeLength = 0;
	nDecodeLength = m_coder.DecodeData((char*)msg->GetData()->Get(), msg->GetSize(), (char*)pDst.get()->Get());

	msg->SetSize(nDecodeLength);

	return true;
};

void CPacket::FillHead(CBuffer& data, PacketHead& head)
{
	data << NetWorkSerilize::SerilizeUint(head.type_coder);
	data << NetWorkSerilize::SerilizeUint(head.type_proto);
	data << NetWorkSerilize::SerilizeUint(head.cmd);
	data << NetWorkSerilize::SerilizeUint(head.length);
};


void CPacket::FillData(CBuffer& data, char* codeData, uint32_t codeLength)
{
	data.Write(codeData, codeLength);
};

bool CPacket::Parse(CBuffer& data, std::shared_ptr<CMessage> msg)
{
	if (PacketParseStatus::PARSE_DONE == msg->GetStatus())
	{
		msg->Reset();
	}

	if (data.Size() < sizeof(PacketHead))
	{
		return true;
	}

	if (PacketParseStatus::PARSE_HEAD == msg->GetStatus())
	{
		//这里会移除读取数据
		PacketHead head = ParseHead(data);
		//头部数据校验
		if (!CheckHead(head))
		{
			return false;
		}
		msg->UpdateStatus(PacketParseStatus::PARSE_DATA);
		msg->SetSize(head.length);
		msg->SetCommond(head.cmd);
		msg->SetCoderType(head.type_coder);
		msg->SetProtoType(head.type_proto);


		if (head.length > data.Size())
		{
			return true;
		}
	}

	if (!ParseData(data, msg))
	{
		return false;
	}

	msg->UpdateStatus(PacketParseStatus::PARSE_DONE);
	return true;
};

bool CPacket::Fill(CBuffer& data,uint32_t nCmd, const char* pInput,uint32_t inputSize, ProtocolType nProtoType)
{
	PacketHead head;
	
	//加密
	uint32_t nCodeLength = m_coder.CodeData(pInput, inputSize, (char*)m_coderBuffer.get()->Get());
	
	head.type_coder = CoderType::CODER_TYPE_NONE;
	head.type_proto = nProtoType;
	head.cmd = nCmd;
	head.length = nCodeLength;
	
	FillHead(data, head);
	CEasylog::GetInstance()->info("head size:",data.Size());
	FillData(data, (char*)m_coderBuffer.get()->Get(), nCodeLength);
	CEasylog::GetInstance()->info("packet size:", data.Size());
	return true;
}

uint32_t CCoder::CodeData(const char* pInput, uint32_t inputSize, char* pOutPut)
{
	//测试
	memcpy(pOutPut, pInput, inputSize);
	return inputSize;
};
uint32_t CCoder::DecodeData(const char* pInput, uint32_t inputSize, char* pOutPut)
{
	//测试
	memcpy(pOutPut, pInput, inputSize);
	return inputSize;
};