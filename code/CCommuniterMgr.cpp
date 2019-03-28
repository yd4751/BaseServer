#include "CCommuniterMgr.h"
#include "CNetWork.h"

bool CCommuniterMgr::Work()
{

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