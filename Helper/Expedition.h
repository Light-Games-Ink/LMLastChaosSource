#ifndef __EXPEDITION_H__
#define __EXPEDITION_H__

#define MAX_EXPED_MEMBER			32		// ������ �ִ� �ο���
#define MAX_EXPED_GMEMBER			8		// ������ 1�� �׷� �ִ� �ο���
#define MAX_EXPED_GROUP				4		// �����볻 �׷� ��
#define MAX_EXPED_DIVITYPE			5		// �й��� ��

class CNetMsg;

// ������ ��� Ŭ����
class CExpedMember
{
	int				m_nCharIndex;			// ĳ���� �ε���
	CLCString		m_strCharName;			// ĳ���� �̸�
	int				m_nGroupType;			// �׷� Ÿ��
	int				m_nMemberType;			// ��� Ÿ��
	int				m_nLabelType;			// ǥ��
	int				m_nListIndex;			// �׷쳻 �迭 �ε���
	int				m_nQuitType;			// ����,������ ����

public:
	CExpedMember(int nCharIndex, const char* strCharName, int nGroupType, int nMemberType, int nListIndex, int nLevel );
	~CExpedMember();

	int				m_nLevel;			// On/Off Line

	int GetCharIndex() const
	{
		return m_nCharIndex;
	}
	const char* GetCharName() const
	{
		return (const char*)m_strCharName;
	}
	int GetGroupType() const
	{
		return m_nGroupType;
	}
	int GetMemberType() const
	{
		return m_nMemberType;
	}
	int GetLabelType() const
	{
		return m_nLabelType;
	}
	int GetQuitType() const
	{
		return m_nQuitType;
	}
	int GetListIndex() const
	{
		return m_nListIndex;
	}

	void SetGroupType(int nGroupType);
	void SetListIndex(int nListIndex);
	void SetMemberType(int nMemberType);
	void SetLabelType(int nLabelType);
	void SetQuitType(int nQuitType);
};

//������ Ŭ����
class CExpedition
{
	char			m_nExpedType[MAX_EXPED_DIVITYPE];					// �й� ���(����ġ,�Ϲ� ������,����� ������)
	int				m_nTotCount;										// ������ �ο���
	int				m_nGTotCount[MAX_EXPED_GROUP];						// �׷� �ο���
	CExpedMember*	m_listMember[MAX_EXPED_GROUP][MAX_EXPED_GMEMBER];	// 4�� �׷�, 8��
	int				m_nRequestIndex;									// ��û�� ĳ���� �ε���
	CLCString		m_strRequestName;									// ���� ��û�� ĳ���� �̸�

	int				m_nSubNo;

public:

	int				m_nRaidZoneNum;
	int				m_nRaidAreaNum;
	int				m_nSubNumber;										// ������ ä���� �ɹ� �Լ��� ������ �ִ´�. �ش� ä�ο����� ������ ���̵� ���� ����
	int				m_nDifficulty;

	CExpedition(int nSubNo, char nExpeType1, char nExpeType2, char nExpeType3, char nExpeType4, char nExpeType5, int nBossIndex, const char* strBossName, int nGroupType, int nMemberType, int nBossLevel );

	~CExpedition();

	int GetRequestIndex() const
	{
		return m_nRequestIndex;
	}
	const char* GetRequestName() const
	{
		return (const char*)m_strRequestName;
	}
	int GetMemberCount() const
	{
		return m_nTotCount;
	}
	char GetExpedType(int nType);

	int GetMemberCountOnline();

	// boss �ε��� ��ȯ
	int GetBossIndex();
	const char* GetBossName();

	// �ش� ĳ���� �ε����� ���� ��Ƽ���� ����Ʈ �ε����� ���Ѵ�
	int FindMemberListIndex(int nCharIndex);
	int FindMemberGroupIndex(int nCharIndex);

	// �ش� ����Ʈ �ε����� ��� Char index�� ���Ѵ�.
	int GetMemberCharIndex(int nGroup, int nListIndex);

	void SetRequest(int nRequest, const char* strRequest);

	// request�� ����� ����
	CExpedMember* JoinRequest(const char* strRequestName, int nMemberType, int nLevel );

	// ��� ����
	CExpedMember* Join(int nCharIndex, const char* strCharName, int nMemberType, int nLevel );

	// �ش� ĳ���͸� ����, �����ϰų� ������ true, ���ſ� �����ϸ� false
	void DeleteMember(int nCharIndex);

	// char index�� CExpeditionMember ���
	CExpedMember* GetMemberByCharIndex(int nCharIndex) const;

	// list index�� CExpeditionMember ���
	CExpedMember* GetMemberByListIndex(int nGroup, int nListIndex) const;

	CExpedMember* GetMemberByListIndex(int nGroup, int nListIndex, int nMemberType) const;

	bool ChangeBoss(int nTargetIndex, int nOldBossIndex );

	void SetExpedType(int nType,char ExpedType)
	{
		if(nType < 0 || nType >= MAX_EXPED_DIVITYPE) return;
		m_nExpedType[nType] = ExpedType;
	}

	int GetGroupMemberCount(int nGroupType);
	int GetGroupBeEmptySlot(void);

	int GetGroupMembertypeCount(int nGroupType, int nMemberType);
	bool SetMBoss(int nNewMBossIndex);
	bool ResetMBoss(int nNewMBossIndex);

	bool MoveGroup(int nSourceGroup, int nMoveCharIndex, int nTargetGroup, int nTargetListindex);
	const CExpedMember* FindNextBoss(void);

	// ������ ���� �Լ�
	void SetEndExped();

	int GetSubNo() const
	{
		return m_nSubNo;
	}
};

#endif
