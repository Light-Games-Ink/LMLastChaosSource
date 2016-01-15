#ifndef __ACELL_H__
#define __ACELL_H__

#include "Character.h"

class CCell
{
public:
	bool Send(CNetMsg::SP& msg, CPC* ch = NULL);
	CCell();
	~CCell();

	CCharacter*		m_listChar;		// �� ���� �ɸ��� ����Ʈ ���
	CItem*			m_listItem;		// �� ���� ������ ����Ʈ ���
	int				m_nCountNPC;	// npc ��
	int				m_nCountPC;		// PC ��
	int				m_nTotalRegen;	// �ش� ������ �����Ǵ� NPC ��

	void CharToCell(CCharacter* ch);
	void CharToCellForSummonNpc(CCharacter* owner, CCharacter* ch);
	void CharFromCell(CCharacter* ch);
	void ItemToCell(CItem* item);
	void ItemFromCell(CItem* item);
};

#endif
//
