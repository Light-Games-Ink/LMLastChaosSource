#ifndef __ADESCRIPTOR_H__
#define __ADESCRIPTOR_H__

#include "../ShareLib/rnsocketioservice.h"
#include "../ShareLib/rnsocketiohandler.h"
#include "../ShareLib/MemoryPoolBase.h"

class CDescriptor : public rnSocketIOHandler, public MemoryPoolBase<CDescriptor>
{
public:
	explicit CDescriptor(rnSocketIOService* service);
	~CDescriptor();

	int			m_serverNo;					// ������ ���� ��ȣ
	int			m_subNo;					// ���� ���� ��ȣ
	int*		m_zones;					// �Ҽ� �� ����Ʈ
	int			m_countZone;				// �Ҽ� �� ����

	rnSocketIOService* service_;

	////////////
	// ���� ����
	int			m_connected;				// CON_ ��� ���� ���� : ���� ����
	int			m_logined;					// �α��� ���

	////////////
	// ��ũ ����
	CDescriptor* m_pPrev;					// ����Ʈ ��ũ
	CDescriptor* m_pNext;

public:
	static CDescriptor* create(rnSocketIOService* service);

	void WriteToOutput(CNetMsg::SP& msg);		// ����
	bool GetLogin(CNetMsg::SP& msg);			// ���� ��û ó��
	int FindZone(int zone);					// �ش� �� ��ȣ ã��
	virtual void operate(rnSocketIOService* service);
	virtual void onClose(rnSocketIOService* service);
};

#endif
//
