#ifndef __ADESCRIPTOR_H__
#define __ADESCRIPTOR_H__

#include "../ShareLib/rnsocketiohandler.h"
#include "../ShareLib/rnsocketioservice.h"
#include "../ShareLib/MemoryPoolBase.h"

class CDescriptor : public rnSocketIOHandler, public MemoryPoolBase<CDescriptor>
{
public:
	explicit CDescriptor(rnSocketIOService* service, bool client_type);
	~CDescriptor();

	rnSocketIOService*	service_;
	bool		client_type_;				// ���Ӽ����� ���� Ŭ���̾�Ʈ�� ����
	// true = ���Ӽ���, false = ���� Ŭ���̾�Ʈ

	int			m_serverNo;					// ������ ���� ��ȣ
	int			m_subNo;					// ���� ���� ��ȣ
	int*		m_zones;					// �Ҽ� �� ����Ʈ
	int			m_countZone;				// �Ҽ� �� ����

	////////////
	// ���� ����
	int			m_connected;				// CON_ ��� ���� ���� : ���� ����
	int			m_uniqeKey;

	////////////
	// ��ũ ����
	CDescriptor* m_pPrev;					// ����Ʈ ��ũ
	CDescriptor* m_pNext;

	virtual void onClose(rnSocketIOService* service);
	virtual void operate(rnSocketIOService* service);

	void WriteToOutput(CNetMsg::SP& msg);	// ����

	bool GetLogin(CNetMsg::SP& msg);		// ���� ��û ó��
	int FindZone(int zone);					// �ش� �� ��ȣ ã��

	bool userAuth(CNetMsg::SP& msg);		// ���� ����

private:
	static int getUniqueKey();
};

#endif
//
