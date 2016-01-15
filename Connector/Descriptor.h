#ifndef __ADESCRIPTOR_H__
#define __ADESCRIPTOR_H__

#include "../ShareLib/rnsocketiohandler.h"
#include "../ShareLib/rnsocketioservice.h"
#include "../ShareLib/MemoryPoolBase.h"

class CDescriptor : public rnSocketIOHandler, public MemoryPoolBase<CDescriptor>
{
public:
	explicit CDescriptor(rnSocketIOService* service);
	~CDescriptor();

	////////////
	// ���� ����
	rnSocketIOService*	service_;

	////////////
	// ���� ����
	int			m_connected;				// CON_ ��� ���� ���� : ���� ����
	int			m_connectreq;				// ���� ��û ƽ ī��Ʈ
	int			m_descnum;					// unique num assigned to desc
	int			m_playmode;					// login mode or play mode
	bool		m_bclosed;
	bool		m_bStartServer;				// �ʱ� ���ӿ� ���� ����(��0�� ������ ������)
	CLCString	m_ipAddr;					// ������ �ּ�
	int			m_portNumber;				// ������ ��Ʈ ��ȣ
	bool		m_bLoginServer;				// �α��� �������� �Ǻ�
	int			m_subno;					// ���� ��ȣ
	int			m_waitbill;					// ������ ��û ��� ��ٸ�
	int			m_billtimeout;				// ���� ���ð�

	void WaitBillReply(int serial)
	{
		if (serial)
		{
			m_waitbill = serial;
			m_billtimeout = 0;
		}
		else
		{
			m_waitbill = 0;
		}
	}

	// �� ����
	int			m_nCountZone;				// �ش� ������ �����ϴ� �� ��
	int*		m_nZoneIndex;				// �ش� ������ �����ϴ� ��

	////////////
	// ��ũ ����
	CDescriptor* m_pPrev;					// ����Ʈ ��ũ
	CDescriptor* m_pNext;

	void WriteToOutput(CNetMsg::SP& msg);	// ����
	bool GetLogin(CNetMsg::SP& msg);			// ���� ���̵� ��� �˻�

	virtual void operate(rnSocketIOService* service);
	virtual void onClose(rnSocketIOService* service);
};

#endif
//
