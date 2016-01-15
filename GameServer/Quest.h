// Quest.h: interface for the CQuest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEST_H__0F547455_B4DB_4214_8DA3_2F5F2BE49117__INCLUDED_)
#define AFX_QUEST_H__0F547455_B4DB_4214_8DA3_2F5F2BE49117__INCLUDED_

#include <vector>
#include "../ShareLib/MemoryPoolBase.h"

class CQuest;

class CAffinityProto;

typedef std::vector<CAffinityProto*>	vec_quest_affinityList_t;

// ����Ʈ ������ Ÿ��
class CQuestProto
{
public:

	// DB
	int			m_index;							// ����Ʈ DB ��ȣ
	int			m_type[2];							// ����Ʈ ���� : ���/Ƚ���� ���� �з�

	int			m_startType;						// ���� ���
	int			m_startData;						// ���� ����� ���� ����Ÿ
	int			m_prizeNPC;							// ����Ʈ�� �Ϸ��Ű�� NPC

	// �ʿ����� : ����Ʈ ���� ����
	int			m_needMinLevel;							// �ּҷ���
	int			m_needMaxLevel;							// �ִ뷹��
	int			m_needJob;								// ����
	int			m_needItemIndex[QUEST_MAX_NEED_ITEM];	// �ʿ������ �ε��� : ������ -1
	int			m_needItemCount[QUEST_MAX_NEED_ITEM];	// �ʿ������ ����

	int			m_needRvRType;
	int			m_needRvRGrade;

	int			m_prequestNum;							//��������Ʈ ��ȣ
	int			m_startNpcZoneNum;						//���� NPC ����ȣ
	int			m_prizeNpcZoneNum;						//���� NPC ����ȣ
	int		m_needExp;									//�ʿ� ����ġ

	// ���� ���� : ����Ʈ ���� ����
	int			m_conditionType[QUEST_MAX_CONDITION];	// ���� ����
	int			m_conditionIndex[QUEST_MAX_CONDITION];	// ���� ���� �ε���
	int			m_conditionNum[QUEST_MAX_CONDITION];	// ���� ��
	int			m_conditionData[QUEST_MAX_CONDITION][QUEST_MAX_CONDITION_DATA];	// ���� ���� �ΰ� ����Ÿ

	// ���� ����
	int			m_prizeType[QUEST_MAX_PRIZE];		// ���� ����
	int			m_prizeIndex[QUEST_MAX_PRIZE];		// ���� �ε���
	LONGLONG			m_prizeData[QUEST_MAX_PRIZE];		// ���� ����Ÿ

	int			m_optPrizeType[QUEST_MAX_OPTPRIZE];		// ���� ���� ����
	int			m_optPrizeIndex[QUEST_MAX_OPTPRIZE];		// ���� ���� �ε���
	int			m_optPrizeData[QUEST_MAX_OPTPRIZE];		// ���� ���� ����Ÿ
	int			m_failValue;						// ���� ��

	int			m_partyScale;

	vec_quest_affinityList_t m_affinityList;

	int			m_StartGiveItem[QUESTITEM_MAX_ARRAYCOUNT];		// ����Ʈ ���� �� �ִ� ������
	int			m_StartGiveKindCount;							// ����Ʈ ���� �� �ִ� ������ ����
	int			m_StartGiveNumCount[QUESTITEM_MAX_ARRAYCOUNT];	// ����Ʈ ���� �� �ִ� ������ ȹ�� ����
	CQuestProto();
};

class CQuestProtoList
{
public:
	typedef std::map<int, CQuestProto*> map_t;

	map_t			map_;
	CQuestProto*	m_proto;		// ����Ʈ ������ ����Ʈ
	int				m_nCount;		// ����Ʈ ��

	CQuestProtoList();
	~CQuestProtoList();

	bool Load();							// DB �б�
	CQuestProto* FindProto(int index);		// ����Ʈ proto ã��
	CQuestProto* GetProtoIndexByNPC(int mobIndex );	// ���ε��� proto ã��

private:
	std::vector<std::string> a_need_item_str;
	std::vector<std::string> a_need_item_count_str;

	std::vector<std::string> a_condition_type_str;
	std::vector<std::string> a_condition_index_str;
	std::vector<std::string> a_condition_num_str;
	std::vector<std::string> a_condition_data_str[QUEST_MAX_CONDITION];

	std::vector<std::string> a_prize_type_str;
	std::vector<std::string> a_prize_index_str;
	std::vector<std::string> a_prize_data_str;

	std::vector<std::string> a_opt_prize_type_str;
	std::vector<std::string> a_opt_prize_index_str;
	std::vector<std::string> a_opt_prize_data_str;
};

class CQuest : public MemoryPoolBaseWithMutex<CQuest>
{
	friend class CQuestList;

	const CQuestProto*	m_pQuestProto;
	char				m_cQuestState;
	int					m_nQuestValue[QUEST_MAX_CONDITION];
	bool				m_bComplete1;
	bool				m_bComplete2;

	CQuest*				m_pPrev;
	CQuest*				m_pNext;

	int					m_nFailValue;
	long				m_nCompleteTime;

public:
	CQuest(const CQuestProto* pQuestProto);
	~CQuest();

	void SetPrevQuest(CQuest* pPrev)
	{
		m_pPrev = pPrev;
	}
	void SetNextQuest(CQuest* pNext)
	{
		m_pNext = pNext;
	}
	CQuest* GetPrevQuest()
	{
		return m_pPrev;
	}
	CQuest* GetNextQuest()
	{
		return m_pNext;
	}

	char GetQuestState()
	{
		return m_cQuestState;
	}

	const CQuestProto* GetQuestProto()
	{
		return m_pQuestProto;
	}
	int GetQuestIndex()
	{
		return (m_pQuestProto) ? m_pQuestProto->m_index : -1;
	}
	int GetQuestType0()
	{
		return (m_pQuestProto) ? m_pQuestProto->m_type[0] : -1;
	}
	int GetQuestType1()
	{
		return (m_pQuestProto) ? m_pQuestProto->m_type[1] : -1;
	}
	int GetPrizeNPCIndex()
	{
		return (m_pQuestProto) ? m_pQuestProto->m_prizeNPC : -1;
	}
	int GetPrizeNPCZone()
	{
		return (m_pQuestProto) ? m_pQuestProto->m_prizeNpcZoneNum : -1;
	}

	bool IsTutorialQuest()
	{
		return (m_pQuestProto && m_pQuestProto->m_type[0] == QTYPE_KIND_TUTORIAL) ? true : false;
	}
	bool IsPetQuest()
	{
		return (	m_pQuestProto->m_index ==  PET_HORSE_QUEST_INDEX
					||	m_pQuestProto->m_index ==  PET_DRAGON_QUEST_INDEX
					||	m_pQuestProto->m_index ==  PET_BLUE_HORSE_QUEST_INDEX
					||	m_pQuestProto->m_index ==  PET_PINK_DRAGON_QUEST_INDEX
					||	m_pQuestProto->m_index ==  PET_UNKOWN_HORSE_QUEST_INDEX
					||	m_pQuestProto->m_index ==  PET_UNKOWN_DRAGON_QUEST_INDEX);
	}
	void SetComplete1(bool bComplete1)
	{
		m_bComplete1 = bComplete1;
	}
	void SetComplete2(bool bComplete2)
	{
		m_bComplete2 = bComplete2;
	}
	bool GetComplete1()
	{
		return m_bComplete1;
	}
	bool GetComplete2()
	{
		return m_bComplete2;
	}
	bool IsCompleted()
	{
		return (m_bComplete1 && m_bComplete2);
	}

	void IncreaseQuestValue(int idx, int val = 1)
	{
		if (idx >= 0 && idx < QUEST_MAX_CONDITION) m_nQuestValue[idx] += val;
	}
	void SetQuestValue(int idx, int val)
	{
		if (idx >= 0 && idx < QUEST_MAX_CONDITION) m_nQuestValue[idx] = val;
	}
	int GetQuestValue(int idx)
	{
		return (idx >= 0 && idx < QUEST_MAX_CONDITION) ? m_nQuestValue[idx] : 0;
	}

	void QuestUpdateData(CPC* pPC, CNPC* pNPC);
	void QuestUpdateData(CPC* pPC, CPC* tPC);

	void RemoeQuestItem(CPC* pc);

	void QuestUpdateDataForParty(CPC* pPC, CNPC* pNPC);
	void QuestUpdateDataForParty(CPC* pPC, CNPC* pNPC, int questIndex);

	int GetQuestType2()
	{
		return (m_pQuestProto) ? m_pQuestProto->m_type[1] : -1;
	}
	int	GetPartyScale()
	{
		return m_pQuestProto->m_partyScale;
	}
	bool CheckComplete(CPC * pc);

	void SetFailValue(int values)
	{
		m_nFailValue = values;
	}
	int GetFailValue()
	{
		return m_nFailValue;
	}
	void SetCompleteTime(long time)
	{
		m_nCompleteTime = time;
	}
	long GetCompleteTime()
	{
		return m_nCompleteTime;
	}

private:
	void SetQuestState(char cQuestState)
	{
		m_cQuestState = cQuestState;
	}
};

class CQuestList
{
	CQuest*			m_head;
	int				m_nCountRun;
	int				m_nCountDone;
	int				m_nCountAbandon;
	int				m_nCountComplete;

public:
	CQuestList();
	~CQuestList();

	int GetCountRun()
	{
		return m_nCountRun;
	}
	int GetCountDone()
	{
		return m_nCountDone;
	}
	int GetCountAbandon()
	{
		return m_nCountAbandon;
	}

	void DecreaseQuestComepleteCount()
	{
		if (m_nCountComplete > 0) m_nCountComplete--;
	}
	void IncreaseQuestComepleteCount()
	{
		m_nCountComplete++;
	}
	int GetQuestComepleteCount()
	{
		return m_nCountComplete;
	}
	void ResetQuestComepleteCount()
	{
		m_nCountComplete = 0;
	}

	CQuest* AddQuest(int nQuestIndex);
	void DelQuest(CPC* pPC, int questIndex);
	bool DelQuest(CPC* pPC, CQuest* pQuest, char cQuestState = QUEST_STATE_RUN );
	void DelQuestALL(CPC* pPC, char cQuestState);
	CQuest* FindQuest(int nQuestIndex);
	bool	FindQuest(int nQuestIndex, char cQuestState);
	CQuest* FindQuestByItem(int nitemIndex);
	CQuest* FindQuestByMob(int nMobIndex);

	bool	IsQuestType0( int qType0 );		//  [2/26/2007 KwonYongDae] ���� ����Ʈ Ÿ���� ���� ������ Ȯ��

	void SetQuestState(CQuest* pQuest, char cQuestState);

	// pCurrentQuest ������ cQuestState ������ ����Ʈ�� ��ȯ, pCurrentQuest�� NULL�̸� ó������ �˻�
	CQuest* GetNextQuest(CQuest* pCurrentQuest, char cQuestState = QUEST_STATE_ALL);

	void CheckComplete(CPC* pc);				// ����Ʈ �� Complete �Ǿ��µ� Msg �Ⱥ��´ٸ� ������ üũ!
	void clear();
	bool isRvrQuest(CPC* pc, int questIndex);
};

#endif // !defined(AFX_QUEST_H__0F547455_B4DB_4214_8DA3_2F5F2BE49117__INCLUDED_)
