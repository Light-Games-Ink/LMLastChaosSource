#ifndef __ADESCRIPTOR_H__
#define __ADESCRIPTOR_H__

#include "../ShareLib/rnsocketioservice.h"
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

	////////////
	// ���� ����
	rnSocketIOService*	service_;			// file descriptor for socket

	////////////
	// ���� ����
	int			m_connected;				// CON_ ��� ���� ���� : ���� ����

	////////////
	// ��ũ ����
	CDescriptor* m_pPrev;					// ����Ʈ ��ũ
	CDescriptor* m_pNext;

	void WriteToOutput(CNetMsg::SP& msg);		// ����
	bool GetLogin(CNetMsg::SP& msg);		// ���� ��û ó��
	int FindZone(int zone);					// �ش� �� ��ȣ ã��
	void SendPartyInfo();					// ��Ƽ/��Ƽ ��Ī ���� ����

	virtual void onClose(rnSocketIOService* service);
	virtual void operate(rnSocketIOService* service);
};

#endif
//
