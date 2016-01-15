#ifndef __PARTY_H__
#define __PARTY_H__

#define		MAX_PARTY_DIVITYPE		5		// �й��� ��(����ġ,������,����� ������)

class CNetMsg;
class CMonsterCombo;

#define PARTY_MATCHING_COMMENT			50		// ��Ƽ ��Ī ��Ƽ ��Ͻ� ������ ����
#define PARTY_MATCHING_DATA_PER_PAGE	10		// ��Ƽ ��Ī ����Ʈ���� �������� ǥ���� ������ ��
#define PARTY_MATCHING_LIMIT_LEVEL		2		// ��Ƽ ��Ī���� ���� ���� ����

class CPartyMatchMember
{
	int			m_nIndex;
	CLCString	m_strName;
	int			m_nLevel;
	int			m_nZone;
	char		m_nJob;
	char		m_nPartyType;

public:
	CPartyMatchMember(int nIndex, const char* strName, int nLevel, int nZone, char nJob, char nPartyType)
		: m_strName(MAX_CHAR_NAME_LENGTH + 1)
	{
		m_nIndex		= nIndex;
		m_strName		= strName;
		m_nLevel		= nLevel;
		m_nZone			= nZone;
		m_nJob			= nJob;
		m_nPartyType	= nPartyType;
	}

	int			GetIndex() const
	{
		return m_nIndex;
	}
	const char*	GetName() const
	{
		return (const char*)m_strName;
	}
	int			GetLevel() const
	{
		return m_nLevel;
	}
	int			GetZone() const
	{
		return m_nZone;
	}
	char		GetJob() const
	{
		return m_nJob;
	}
	char		GetPartyType() const
	{
		return m_nPartyType;
	}

	void		SetName(const char* strName)
	{
		m_strName = strName;
	}
	void		SetLevel(int nLevel)
	{
		m_nLevel = nLevel;
	}
	void		SetZone(int nZone)
	{
		m_nZone = nZone;
	}
};

class CPartyMatchParty
{
	int			m_nBossIndex;
	CLCString	m_strBossName;
	int			m_nBossLevel;
	int			m_nZone;
	int			m_nJobFlag;
	char		m_nPartyType;
	bool		m_bLimitLevel;
	CLCString	m_strComment;

	int			m_nMemberCnt;

public:
	CPartyMatchParty(int nBossIndex, const char* strBossName, int nBossLevel, int nZone, int nJobFlag, char nPartyType, bool bLimitLevel, const char* strComment)
		: m_strBossName(MAX_CHAR_NAME_LENGTH + 1)
		, m_strComment(PARTY_MATCHING_COMMENT + 1)
	{
		m_nBossIndex = nBossIndex;
		m_strBossName = strBossName;
		m_nBossLevel = nBossLevel;
		m_nZone = nZone;
		m_nJobFlag = nJobFlag;
		m_nPartyType = nPartyType;
		m_bLimitLevel = bLimitLevel;
		m_strComment = strComment;
	}

	int			GetBossIndex() const
	{
		return m_nBossIndex;
	}
	const char*	GetBossName() const
	{
		return (const char*)m_strBossName;
	}
	int			GetBossLevel() const
	{
		return m_nBossLevel;
	}
	int			GetZone() const
	{
		return m_nZone;
	}
	int			GetJobFlag() const
	{
		return m_nJobFlag;
	}
	char		GetPartyType() const
	{
		return m_nPartyType;
	}
	bool		IsLimitLevel() const
	{
		return m_bLimitLevel;
	}
	const char*	GetComment() const
	{
		return (const char*)m_strComment;
	}

	void		SetBossIndex(int nBossIndex)
	{
		m_nBossIndex = nBossIndex;
	}
	void		SetBossName(const char* strName)
	{
		m_strBossName = strName;
	}
	void		SetBossLevel(int nLevel)
	{
		m_nBossLevel = nLevel;
	}
	void		SetZone(int nZone)
	{
		m_nZone = nZone;
	}

	int			GetMemberCount() const
	{
		return m_nMemberCnt;
	}
	void		SetMemberCount( int nMemberCnt )
	{
		m_nMemberCnt = nMemberCnt;
	}
};

// ��Ƽ ���
class CPartyMember
{
	int				m_nCharIndex;
	CLCString		m_strCharName;

	CPC*			m_pChar;

public:
	CPartyMember(int nCharIndex, const char* strCharName, CPC* pc);
	~CPartyMember();

	int				m_nLevel;

	int GetCharIndex() const
	{
		return m_nCharIndex;
	}
	const char* GetCharName() const
	{
		return (const char*)m_strCharName;
	}

	CPC* GetMemberPCPtr() const;
	void SetMemberPCPtr(CPC* pc);
};

struct AftifactItemData
{
	CPC* pc;
	int item_index;

	AftifactItemData() : pc(NULL), item_index(-1) {}

	void _reset()
	{
		pc = NULL;
		item_index = -1;
	}

	void _insert(CPC* _pc, int _item_index)
	{
		pc = _pc;
		item_index = _item_index;
	}
};

class CParty
{
public:
	char			m_nPartyType[MAX_PARTY_DIVITYPE];			// ����ġ �й� Ÿ��	: 0 - �յ�, 1 - �Լ��켱, 2 - new��Ƽ
	int				m_nCount;
	CPartyMember*	m_listMember[MAX_PARTY_MEMBER];
	int				m_nRequest;
	CLCString		m_strRequest;

	int				m_nEndPartyTime;							// ��Ƽ ��ü �ð�

	int				m_nRaidZoneNum;
	int				m_nRaidAreaNum;
	int				m_nSubNumber;								// ������ ä���� �ɹ� �Լ��� ������ �ִ´�. �ش� ä�ο����� ������ ���̵� ���� ����

	typedef std::map<int/*guild_index*/, int/*exp*/> GUILD_SKILL_EXP;
	GUILD_SKILL_EXP	m_guild_skill_exp;

	AftifactItemData arti_data;

	void deletePartyRecallMember( int memberIndex );
	std::set<int>	m_warPartyRecall;

	CParty(char nPartyType, int nBossIndex, const char* strBossName, CPC* pBoss, int nRequest, const char* strRequest);
	~CParty();

	int GetRequestIndex() const
	{
		return m_nRequest;
	}
	const char* GetRequestName() const
	{
		return (const char*)m_strRequest;
	}
	int GetMemberCount() const
	{
		return m_nCount;
	}
	char GetPartyType(int nType) const
	{
		if(nType < 0 || nType >= MAX_PARTY_DIVITYPE) return -1;
		return m_nPartyType[nType];
	}
	int	GetMemberCountOnline();

	// boss �ε��� ��ȯ
	int GetBossIndex() const
	{
		return (m_listMember[0]) ? m_listMember[0]->GetCharIndex() : -1;
	}

	// �ش� ĳ���� �ε����� ���� ��Ƽ���� ����Ʈ �ε����� ���Ѵ�
	int FindMember(int nCharIndex);

	// �ش� ����Ʈ �ε����� ��� Char index�� ���Ѵ�.
	int GetMemberCharIndex(int nListIndex);

	void SetRequest(int nRequest, const char* strRequest);

	// request�� ����� ����
	int JoinRequest(const char* strRequestName, CPC* pRequest);

	// �ش� ĳ���͸� ����, �����ϰų� ������ true, ���ſ� �����ϸ� false
	void DeleteMember(int nCharIndex);

	// list index�� CPartyMember ���
	CPartyMember* GetMemberByListIndex(int nListIndex) const;
	CPartyMember* GetMemberByCharIndex(int charIndex) const;

	bool ChangeBoss(const char* strNewBossName);

	void SetPartyType(int nType,char PartyType)
	{
		if(nType < 0 || nType >= MAX_PARTY_DIVITYPE) return;
		m_nPartyType[nType] = PartyType;
	}
	void SetPartyTypeAll(char cPartyType);

	// ��Ƽ ����� ���� ������ NULL ó�� �Լ�
	void SetEndParty();

	int GetNearPartyMemberCount(CCharacter* pCenter);

	// pPC ����� ���� �ִ� nListIndex��° ��Ƽ���� ���Ѵ�
	CPC* GetNearMember(CCharacter* pPC, int nListIndex);

	// �ش� ĳ�����ε����� ����� PC �����͸� �����Ѵ�
	void SetMemberPCPtr(int nCharIndex, CPC* pPC);

	// ���� PC�� �����Ǿ� �ִ� ��� ��Ƽ������ �޽����� ����, nExcludeCharIndex ����
	void SendToAllPC(CNetMsg::SP& msg, int nExcludeCharIndex = -1);
#ifdef CONTINENT_PARTY_RECALL
	void SendToSameContinentPC(CNetMsg::SP& msg, int nExcludeCharIndex = -1);
#endif

	void SendToPartyRecallPC(CNetMsg::SP& msg, int nReqIndex);

	// ���� ���� �ִ� PC���Ը� �޽��� ����
	void SendToPCInSameZone(int nZone, int nArea, CNetMsg::SP& msg);

	// ��Ƽ ��ü �ð� ����
	void SetEndPartyTime(int nNow);

	// ��Ƽ ��ü �ð� ȹ��
	int  GetEndPartyTime();

	// ���̵��� �ϸ� ��Ƽ���� ���ܽ�Ű�µ� ���̵� ���н� �ٽ� ��Ƽ�� ���� �����ִ� �Լ�
	bool SetMemberRegister_AfterGoZone(CPC* pPC);

	// ��Ƽ ���� ��� ��ġ ����
	void SetPartyRecallUseInfo(int zoneID, CPos& pos, int guildIndex, char cIsInCastle, int nExcludeCharIndex);

	// ���Ӽ��� FRIEND �Լ�
	friend void ProcHelperPartyInfoParty(CNetMsg::SP& msg);

	int getGuildMemberCountInParty(CPC* pc);

	void calcGuildSkillExp(CPC* pc, int exp);
	int getGuildSkillExp(CPC* pc);
	void clearGuildSkillExp(CPC* pc);

	void updateCalcMember();

#ifdef GMTOOL
	bool		m_bGmtarget;
#endif // GMTOOL

	int m_comboAreaIndex;

#ifdef EXTREME_CUBE
	int	m_cubeUniqueIdx;
#endif // EXTREME_CUBE
};

#endif
//
