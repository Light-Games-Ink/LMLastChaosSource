#ifndef __EXCHANGE_ITEMS_H__
#define __EXCHANGE_ITEMS_H__

class CPC;

class CExchangeItemsData
{
public:
	CPC*		m_ch;							// �÷��̾�
	int			m_regCount;						// ��� ������ ��
	int			m_itemindex[MAX_EXCHANGE_ITEMS];// ������ ������ (�κ��丮�� �Ϲ� ������)
	LONGLONG	m_count[MAX_EXCHANGE_ITEMS];	// ������ ��ȯ ����
	GoldType_t  m_nasCount;						// ��ȯ�� ���� ��

	CExchangeItemsData();

	void SetCharacter(CPC* pc);								// �÷��̾� ����
	int Add(int itemindex, LONGLONG count, bool* bUpdate);	// ������ �߰�
	int Del(int itemindex, LONGLONG count);					// ������ ����
	int Find(int item_idx);									// ������ ã��
	int FindEmpty();										// ���ڸ� ã��
};

class CExchangeItems
{
public:
	MSG_EXCHANGE_REQ_TYPE	m_status;					// ��ȯ ���� ��Ȳ
	CExchangeItemsData		m_src;						// ��ȯ ��û�� ��� ������
	CExchangeItemsData		m_dest;						// ��ȯ ���� ��� ������

	CExchangeItems();

	void SetCharacter(CPC* src, CPC* dest);				// �÷��̾� ����
	int Add(CPC* ch, int item_idx, LONGLONG count, bool* bUpdate);	// ������ ��ȯ ��Ͽ� �߰�
	int Del(CPC* ch, int item_idx, LONGLONG count);		// ������ ��ȯ ��Ͽ� ����
	int Find(CPC* ch, int item_idx);					// ã��

	/////////////////////
	// Function name	: CanExchange
	// Description	    : ��ȯ ���� ���� �Ǵ�
	// Return type		: int
	//            		: 0 - ����
	//            		: 1 - src �Ұ���
	//            		: 2 - dest �Ұ���
	int CanExchange();

	void DoExchange();
};

#endif
//
