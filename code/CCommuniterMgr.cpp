#include "CCommuniterMgr.h"
#include "CNetWork.h"

bool CCommuniterMgr::Work()
{
	//�����Ӵ���(����Do�ڲ��������ⲿ�������߳�����)
	CNetWork::GetInstance()->newConnections.Do([this] (std::list<std::shared_ptr<CCommuniter>>& data){
		while (true)
		{
			if (m_connections.LeftSize() <= 0 || data.empty())
				break;

			auto param = data.front();
			m_connections.Add(param->GetID(), param);
			CNetWork::GetInstance()->m_events.Add(param->GetID(), param);
			if (param->m_bNotiyConnect)
				param->OnConnected();

			data.pop_front();
		}
	});


	std::shared_ptr<CCommuniter> pGet = m_connections.GetMatch([](const std::shared_ptr<CCommuniter> node)->bool {
		if (node->Read()) return true;
		if (node->Write()) return true;
		if (node->GetStatus() == ConnectionStatus::CONNECT_STATUS_CLOSED) return true;
		return false;
	});
	if (pGet)
	{
		if (pGet->GetStatus() == ConnectionStatus::CONNECT_STATUS_CLOSED)
		{
			pGet->OnClosed();
			m_connections.Remove(pGet->GetID());
			CEasylog::GetInstance()->warn("Remove connection:", pGet->GetID(), m_connections.Size());
		}
		return true;
	}

	return false;
}