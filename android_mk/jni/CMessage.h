#ifndef __H_PACKET_H__
#define __H_PACKET_H__
//#include "DataDefine.h"
#include "CBuffer.h"
#include "NetWorkSerilize.h"
#include "CNetCoreInterface.h"
/*
������ṹ��
��ͷ��
��ʶ:2�ֽ�			//$$
����:2�ֽ� 
���4�ֽ�
���ݳ���:4�ֽ�

���ݲ��֣�

*/

struct PacketHead
{
	uint16_t			type_coder;			//��������
	uint16_t			type_proto;			//Э������
	uint32_t			cmd;
	uint32_t			length;
};

#define PACKET_HEAD_FLAG				0x2424    //$$
#define PACKET_MAX_LEN					(0x2000 + sizeof(PacketHead))
#define MESSAGE_MAX_LEN					0x2000
#define CODER_MAX_BUFF					0x4000

enum PacketParseStatus
{
	PARSE_HEAD,
	PARSE_DATA,
	PARSE_DONE,
};
enum CoderType
{
	CODER_TYPE_NULL = 0,
	CODER_TYPE_NONE,	//�޼���
	CODER_TYPE_XOR,			//������
	CODER_TYPE_MAX,
};

class CMessage
{
	uint32_t						m_length;				//Э�������ݳ���
	uint32_t						m_cmd;					//��������
	ProtocolType					m_PortoType;			//Э������
	CoderType						m_CoderType;			//��������

	std::shared_ptr<CArray<uint8_t>>		m_data;					//Э��������
	PacketParseStatus				m_StatusParse;			//����״̬

private:
	bool AllocDataSpace(uint32_t nMaxLength);

public:
	CMessage()
	{
		AllocDataSpace(PACKET_MAX_LEN);
		Reset();
	}

	void Reset();

public:
	void SetCoderType(uint32_t type) { m_CoderType = static_cast<CoderType>(type); }
	CoderType GetCoderType() { return m_CoderType; }

	void SetProtoType(uint32_t type) { m_PortoType = static_cast<ProtocolType>(type); }
	ProtocolType GetProtoType() { return m_PortoType; }

	void SetCommond(uint32_t nCmd) { m_cmd = nCmd; }
	uint32_t GetCommond() { return m_cmd;}

	void UpdateStatus(PacketParseStatus status) { m_StatusParse = status; };
	PacketParseStatus GetStatus() { return m_StatusParse; };

	uint32_t GetMaxSize() { return PACKET_MAX_LEN;	}
	void SetSize(uint32_t size) { m_length = size; }
	uint32_t GetSize() { return m_length; }

	std::shared_ptr<CArray<uint8_t>> GetData() { return m_data; }
};


class CCoder
{

public:
	CCoder() {};
	~CCoder() {};
public:
	uint32_t CodeData(const char* pInput, uint32_t inputSize, char* pOutPut);
	uint32_t DecodeData(const char* pInput, uint32_t inputSize, char* pOutPut);
};

///
class CBuffer;

class CPacket
{
	std::shared_ptr<CArray<uint8_t>> m_coderBuffer;

private:
	//return�� �ܳ� - ����ֵ - ���ݳ���
	typedef std::tuple<uint32_t, uint8_t, uint32_t> HeadInfo;
	PacketHead ParseHead(CBuffer& data);
	bool CheckHead(const PacketHead& head);

	//return: false - ���ݽ����쳣  true - ���쳣
	bool ParseData(CBuffer& data, std::shared_ptr<CMessage> msg);

	void FillHead(CBuffer& data, PacketHead& head);

	void FillData(CBuffer& data, char* codeData,uint32_t codeLength);
public:

	CCoder			m_coder;
	CPacket()
	{
		m_coderBuffer = std::make_shared<CArray<uint8_t>>(CODER_MAX_BUFF);
	}
public:
	//return �� false - �����쳣  true - �������쳣
	bool Parse(CBuffer& data, std::shared_ptr<CMessage> msg);

	bool Fill(CBuffer& data, uint32_t nCmd , const char* pInput, uint32_t inputSize, ProtocolType nProtoType);
};

#endif