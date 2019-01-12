#include "CRpc.h"
#include "NetWorkSerilize.h"
#include "message_define.h"
#include "CEasyLog.h"

//ReplyErrorCode
//格式 类型:长度:数据    1:2:N

#define SERIALIZE_SEP_CHAR				':'
enum SerializeType
{
	SERIALIZE_TYPE_UINT32,
	SERIALIZE_TYPE_STRING,
};

template<>
void CRpc::SerializeParam(int param)
{
	uint32_t pInput = 0;
	pInput = static_cast<uint32_t>(param);
	m_paramBuf << static_cast<char>(SerializeType::SERIALIZE_TYPE_UINT32);
	m_paramBuf << static_cast<char>(SERIALIZE_SEP_CHAR);
	m_paramBuf << NetWorkSerilize::SerilizeUint( static_cast<uint16_t>(sizeof(uint32_t)) );
	m_paramBuf << static_cast<char>(SERIALIZE_SEP_CHAR);
	m_paramBuf << NetWorkSerilize::SerilizeUint(pInput);
};
template<>
void CRpc::SerializeParam(uint32_t pInput)
{
	m_paramBuf << static_cast<char>(SerializeType::SERIALIZE_TYPE_UINT32);
	m_paramBuf << static_cast<char>(SERIALIZE_SEP_CHAR);
	m_paramBuf << NetWorkSerilize::SerilizeUint(static_cast<uint16_t>(sizeof(uint32_t)));
	m_paramBuf << static_cast<char>(SERIALIZE_SEP_CHAR);
	m_paramBuf << NetWorkSerilize::SerilizeUint(pInput);
};
template<>
void CRpc::SerializeParam(std::string str)
{
	m_paramBuf << static_cast<char>(SerializeType::SERIALIZE_TYPE_STRING);
	m_paramBuf << static_cast<char>(SERIALIZE_SEP_CHAR);
	m_paramBuf << NetWorkSerilize::SerilizeUint( static_cast<uint16_t>(str.length()) );
	m_paramBuf << static_cast<char>(SERIALIZE_SEP_CHAR);
	m_paramBuf << str;
};
template<>
void CRpc::SerializeParam(MessageDefine::ReplyErrorCode pInput)
{
	SerializeParam(static_cast<uint32_t>(pInput));
}
template<>
void CRpc::SerializeParam(const char* pInput)
{
	SerializeParam(std::string(pInput));
}

template<>
void CRpc::operator >> (uint32_t& out)
{
	char type,sep;
	m_paramBuf >> type;
	m_paramBuf >> sep;
	uint16_t length = NetWorkSerilize::DeSerilizeUint<uint16_t>(m_paramBuf);
	SerializeType targeType = static_cast<SerializeType>(type);
	if (SerializeType::SERIALIZE_TYPE_UINT32 != targeType)
	{
		CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
	}

	m_paramBuf >> sep;
	if(static_cast<SerializeType>(type) == SerializeType::SERIALIZE_TYPE_UINT32)
		out = NetWorkSerilize::DeSerilizeUint<uint32_t>(m_paramBuf);
	else if (static_cast<SerializeType>(type) == SerializeType::SERIALIZE_TYPE_STRING)
	{
		for(unsigned int i=0; i< length; i++)
			m_paramBuf.Get();
	}
};
template<>
void CRpc::operator >> (MessageDefine::ReplyErrorCode& out)
{
	uint32_t out_new;
	(*this) >> out_new;
	out = static_cast<MessageDefine::ReplyErrorCode>(out_new);
}
template<>
void CRpc::operator >> (std::string& out)
{
	char type, sep;
	m_paramBuf >> type;
	m_paramBuf >> sep;
	uint16_t length = NetWorkSerilize::DeSerilizeUint<uint16_t>(m_paramBuf);
	SerializeType targeType = static_cast<SerializeType>(type);
	if (SerializeType::SERIALIZE_TYPE_STRING != targeType)
	{
		CEasylog::GetInstance()->error(__FILE__, __LINE__, __FUNCTION__);
	}
	m_paramBuf >> sep;
	if (static_cast<SerializeType>(type) == SerializeType::SERIALIZE_TYPE_UINT32)
		NetWorkSerilize::DeSerilizeUint<uint32_t>(m_paramBuf);
	else if (static_cast<SerializeType>(type) == SerializeType::SERIALIZE_TYPE_STRING)
	{
		for (unsigned int i = 0; i < length; i++)
			out.push_back(m_paramBuf.Get());
	}
}