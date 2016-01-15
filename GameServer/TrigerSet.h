#ifndef __TRIGER_SET_H__
#define __TRIGER_SET_H__

// �ϳ��� Ʈ���� ���� ����
class CTrigger
{
public:

/////////////////////// ���� ����
	/////////DB ����////�ٲ�� �ȵ�
	int			m_index;								// DB Index
	int			m_triggerset_type;						// Ʈ���� �Ӽ�
	int			m_trigger_touchID;						// �̰��� �ĺ��ڷ� ���
	int			m_trigger_playID;
	int			m_trigger_type;
	CLCString	m_trigger_data;
	int			m_nSaveLevel;							// �߰� ������ ���� ����
	int			m_group_index;
	int			m_nType_sub;							// ���� Ÿ���� �ִٸ� �̰��� ���
	int			m_bContinue;							// Ʈ���Ű� �����Ǵ� ������ ����
	int			m_bDomino;								// ���̳� ���� Ʈ���� �ߵ����� ����
	int			m_bPlayOnePC;							// �Ϲ� �ʵ忡�� �� ���Ը� ������ �Ѵ�.

	////////�޸� ����////�ʱ�ȭ ���
	bool		m_bUsed;								// �̹� Ʈ���Ű� �ߵ��Ͽ����� ����
	long		m_TriggerTime;							// Ʈ���� ��� ���� �ð�����

////////////////// �� Ʈ���ź� ����

	/////// ITEM_COMPOSITION
	int		m_nItem_index_ItemComposition;				// �ռ��� ������ �ε���
	int		m_nMeterial_IndexCount;						// ��� ������ ����
	int		m_nMeterial_index[TRIGGER_MAX_ARRAYCOUNT];	// ��� ������ �ε���
	int		m_nMeterial_count[TRIGGER_MAX_ARRAYCOUNT];	// ��� ������ ����

	/////// ITEM_DROP
	int		m_ItemDrop_index;							// ����Ǵ� ������ �ε���
	int		m_ItemDrop_percent;							// ����Ǵ� Ȯ��, 1 ~ 100%

	/////// OPEN_THE_DOOR	- ���� Ÿ�� ���
	int		m_Killed_Npc_Index;							// ���� ���� ���� �׿��� �ϴ� npc ����
	int		m_Killed_Npc_Count;							// ���� ���� ���� �׿��� �ϴ� npc ����
	int		m_NeedItem_IndexCount;						// ���� ���� ���� ���� �ϴ� item ���� ��
	int		m_NeedItem_Index[TRIGGER_MAX_ARRAYCOUNT];	// ���� ���� ���� ���� �ϴ� item �ε���
	int		m_NeedItem_Count[TRIGGER_MAX_ARRAYCOUNT];	// ���� ���� ���� ���� �ϴ� item ����

	/////// START_RAID

	/////// MESSAGE_PRINT
	int		m_nStringIndex;								// t_string_index �� ����

	/////// SUMMON_NPC
	int		m_nSummonNpc_indexCount;					// ��ȯ�Ǵ� npc ���� ��
	int		m_nSummonNpc_index[TRIGGER_MAX_ARRAYCOUNT];	// ��ȯ�Ǵ� npc �ε���
	int		m_nSummonNpc_count[TRIGGER_MAX_ARRAYCOUNT];	// ��ȯ�Ǵ� npc ����

	/////// USE_DEBUF
	int		m_nUseDebuf_indexCount;						// ����� �ɸ��� ���� ��
	int		m_nUseDebuf_index[TRIGGER_MAX_ARRAYCOUNT];	// ����� �ɸ��� �ε���

	/////// ACTIVE_OBJECT

	/////// REMOVE_NPC	- ���� Ÿ�� ���
	int		m_Item_Index_removeNPC;						// ���̴µ� ���Ǵ� item index
	int		m_KilledNpc_Index_ForRemoveNpc;				// �״� npc index
	int		m_KillCount;								// item �� �״� npc ��

	/////// TIME_LIMIT
	long	m_TimeLimitSec;								// ���� �ð� (sec)

	/////// QUEST_COMPLETE
	int		m_nQuestIndex;								// �Ϸ��� ����Ʈ �ε���

	/////// ITEM_GET_INVEN
	int		m_nGetItem_TotalCount;						// �κ��� ���� ������ �� ����
	int		m_nGetItem_Index[TRIGGER_MAX_ARRAYCOUNT];	// �κ��� ���� ������ �ε���
	int		m_nGetItem_Count[TRIGGER_MAX_ARRAYCOUNT];	// �κ��� ���� ������ ����
	int		m_nGetItem_Prob[TRIGGER_MAX_ARRAYCOUNT];	// �κ��� ���� ȹ�� Ȯ��

	/////// CONSECUTIVE_DIALOGUE
	int		m_nMainDialogueString;						// ���� ��ȭ ó�� â
	int		m_nNextDialogueString1;						// ���� ��ȭ ��Ʈ�� �ε���
	int		m_nNextDialogueString2;						// ���� ��ȭ ��Ʈ�� �ε���
	int		m_nNextDialogueString3;						// ���� ��ȭ ��Ʈ�� �ε���
	int		m_nNextDialogueString4;						// ���� ��ȭ ��Ʈ�� �ε���
	int		m_nNextDialogueString5;						// ���� ��ȭ ��Ʈ�� �ε���
	int		m_nNextDialogueTriggerID1;					// ���� ��ȭ Ʈ���� ���̵�
	int		m_nNextDialogueTriggerID2;
	int		m_nNextDialogueTriggerID3;
	int		m_nNextDialogueTriggerID4;
	int		m_nNextDialogueTriggerID5;

	/////// ACTIVE_EFFECT
	CLCString	m_strEffectName;						// ����Ʈ �̸�
	int			m_nTargetType;							// �ڽ����� �ٸ� npc ����
	int			m_nEffectNPCIndex;						// �ٸ� npc�̸� npc �ε���

	CTrigger();
	~CTrigger();

	void		SetTriggerTime(long time)
	{
		m_TriggerTime = time;
	};
	long		GetTriggerTime()
	{
		return m_TriggerTime;
	};

	void		CopyTriggerDate(CTrigger* trigger);
};

// Ʈ���ŵ��� ������ ��� �ִ� ������ ����Ʈ
class CTrigger_List
{
public:

	// DB
	CTrigger*	m_Trigger;
	int			m_nCount;
	CArea*		m_pArea_UseTrigerEvent;

	// ����� 1�� npc 963 ��� �׾����� üũ(�ϵ��ڵ����� ���)
	int			m_nNPC963_KilledCount;
	int			m_flagTriggerEvent;				// Ʈ���� �̺�Ʈ ��� �÷��� ù��°

	int     Check_TriggerFlag(int mask)
	{
		return m_flagTriggerEvent & mask;
	}
	void    Set_TriggerFlag(int state)
	{
		m_flagTriggerEvent |= state;
	}
	void    Reset_TriggerFlag(int state)
	{
		m_flagTriggerEvent &= ~state;
	}

	void	InitTriggerList();
	bool	Load_Trigger();

	CTrigger_List();
	~CTrigger_List();

	bool	ResetTriggerInfo(bool bDelete);			//	Ʈ���� �ʱ�ȭ, ���̵忡�� ����� ��, bSave�� DB���̺굵 �ʱ�ȭ ���� ����
	bool	SaveTriggerInfo(int saveInfo, int roomNumber=0);	//	Ʈ���� ������ ������ ��
	void	LoadTriggerSaveInfo(int roomNumber);	//  ���� ���� �� Ʈ������ �߰� ������ �ҷ� ��
	bool	LoadTriggerInfo(int triggerSetType);	//  ���� ���� �� Ʈ���� ������ �ҷ� ��

	bool	SyncForClient_TriggerInfo(CPC* ch);		//  Ŭ���̾�Ʈ ����� ���� Ʈ���� ������ ����
	void	TriggerTimerCheck();					//	Ÿ�̸Ӹ� ����ϴ� Ʈ���� üũ, �ϵ��ڵ� ó��

	void	SetArea(CArea* pArea)
	{
		m_pArea_UseTrigerEvent = pArea;
	};
	CArea*	GetArea()
	{
		return m_pArea_UseTrigerEvent;
	};

private:
	void		Swap_Trigger(CTrigger* trigger1, CTrigger* trigger2);
};

typedef struct NPC_REGEN_FOR_RAID
{
	int		m_npcIdx;
	float	m_pos_x;
	float	m_pos_z;
	float	m_pos_h;
	float	m_pos_r;
	int		m_y_layer;
	int		m_storeMiddle;				//�߰� ���� ��� - ��޿� ���� npc ���� ó��
} NPC_REGEN_FOR_RAID;

// �ν���Ʈ ������ ���� ������ ����
// ���� Ž���̶� �˰��� �ʿ� ����
// �� ������ DB �����̹Ƿ� �� ���� �ʿ���
class CNpc_Regen_Raid
{
public:

	int m_nCount_regen;
	NPC_REGEN_FOR_RAID* m_regenList_InstantDoungen;

	CNpc_Regen_Raid();
	~CNpc_Regen_Raid();
};

#endif
