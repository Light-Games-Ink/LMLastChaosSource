// MonsterCombo.h: interface for the CMonsterCombo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONSTERCOMBO_H__A0046A35_9FFA_4984_98CE_7C324A4237A6__INCLUDED_)
#define AFX_MONSTERCOMBO_H__A0046A35_9FFA_4984_98CE_7C324A4237A6__INCLUDED_

#include <vector>

#define MAXCOMBOCASE		20
#define MAXEFFECT			5

class CArea;

struct MOBTYPE
{
	int		npcidx;		// �⿬�ϴ� npc �ε���
	char	count;		// �⿬�ϴ� npc ī��Ʈ
};

struct MOBREGEN
{
	char		step;		// ���° �����ΰ�?
	char		typecount;	// �����Ǵ� ��Ÿ���� ����
	MOBTYPE*	mobtype;	// �����Ǵ� ��Ÿ��

	MOBREGEN()
	{
		step = 0;
		typecount = 0;
		mobtype = NULL;
	};

	~MOBREGEN()
	{
		step = 0;
		if(mobtype)
		{
			delete[] mobtype;
			mobtype = NULL;
		}
	}
};

struct PENALTY
{
	int index;
	int level;
};

struct EFFECTPOS
{
	CNPC*	npc;
	float	x;
	float	z;
	float	h;
};

typedef std::vector<EFFECTPOS> vec_effectpos_t;

class CMissionCaseProto
{
public:
	int		m_nIndex;
	int		m_nNas;
	int		m_nPoint;
	CLCString m_name;

	// ����
	int				m_nRegenCount;		// ���� �ܰ�
	MOBREGEN*		m_npcRegen;		// ���� ����

	// �г�Ƽ
	std::vector<PENALTY>	m_listPanalty;		// �г�Ƽ ������ ��ų�ε���

	bool			LoadProto();

	const MOBREGEN*		FindMobRegen(int step);

	CMissionCaseProto();
	~CMissionCaseProto();
	// ����ǰ
};

class CMIssionCaseList
{
public:
	CMIssionCaseList();
	~CMIssionCaseList();

	typedef std::map<int, CMissionCaseProto*> map_t;

	int					m_nCount;
	CMissionCaseProto*	m_proto;
	map_t				map_;

	CMissionCaseProto*	FindProto(int index);
	bool				LoadList();
};

struct MISSION
{
	char			step;
	CMissionCaseProto*	proto;
	MISSION()
	{
		step = 0;
		proto = NULL;
	};
};

class CMonsterCombo
{
public:
	CMonsterCombo();
	virtual ~CMonsterCombo();

	int		m_nIndex;
	int		m_bIsBattle;		// �ο򿩺�
	int		m_nExtra;			// �������� ���
	int		m_nStage;			// �������� ��������
	int		m_nRegenStep;		// �������� regen
	char	m_cPlayerCount;		// ����� �����Ϸ���

	int		m_pulseFirst;		// 1��° ����Ʈ
	int		m_pulseSecond;		// 2��° ����Ʈ

	int	    m_nTotalMission;
	char	m_cPlayerNum;		// stage�� �ִ� ��Ƽ����

	static int m_virtualIndex;

#ifdef MONSTER_COMBO_FIX
	bool	m_bPartyRecall;
#endif // MONSTER_COMBO_FIX

	// ����Ʈ ��ġ ����
	vec_effectpos_t m_listEffect;

	MISSION*	m_case;
	CArea*		m_area;
	CNPC*		m_effectNPC[MAXEFFECT];

	void		SetArea(CArea*	area);
	char		Run();
	void		DeleteAllMob();

	// 0: �����������  1: ������������		2: ����
	char		NextNPCRegen();

	// �̼Ǽ���
	bool		SetMission(int step, int index);

	// step: ����������ȣ
	const MISSION*	FindMission(int step);

	// ���������� ������ȯ
	void RecallToStage();

	// �������� ����
	bool StartStage();

	// �г�Ƽ �ʱ�ȭ
	void ClearPenalty();

	// ���Ƽ ����
	void ApplyPenalty(int skillindex, int skilllevel);

	//  �������� NPC ����
	void GiftMobRegen();

	//  �̼����̽� �� ����Ʈ ���
	int GetTotalPoint();

	//
	int GetMaxCoinCount(int count)
	{
		int temp;
		temp = (int)(0.5f * count);

		float temp2 = (float)(0.5f * count);
		float temp3;
		temp3 = temp2 - (float)temp;

		if(temp3 >= 0.5)
		{
			temp += 1;
		}

		temp += m_nStage/15;

		if(temp < 1)
			temp = 1;

		return temp;
	}

	// �ұ�� �غ� ����Ʈ
	void FireEffectReady2();

	// �ұ�� ���� ����Ʈ
	void FireEffectAct();

	// ����� ���
	int GetTotalNas();

	// ������ npc ����
	void ComboNPCRegen(/*char bContinue*/);

	// ���� �̼����̽� ���� ����
	void SetNextVar();

	// ���� �ʱ�ȭ
	void InitStageVar();

	// �ʱ�ȭ
	bool Init(int count);
	// �޺� �ε��� ����
	void SetVirtualIndex();

	// �������� �˸�
	void NoticeStage();

	// �޺����� �ִ� �÷��̾� �α� ���
	void PrintCharIndex();

	void InitAllVar();

	bool SetMonsterCombo(CPC* pc);

#ifdef MONSTER_COMBO_FIX
	bool IsAllCharLive();
#endif // MONSTER_COMBO_FIX
};

#define MAX_GROUND_EFFECT2_NPC_NUM	15
class CGroundEffect2
{
public:
	bool IsStarted();
	CGroundEffect2();
	virtual ~CGroundEffect2();

	bool	Init(int zone, CArea* area);			// ó���� �ѹ� ����
	void	Start();	// �ߵ� ����
	void	Stop();								// �ߵ� ����
	void	Activity();							// �ߵ� ��
	CArea*	GetArea();
private:
	bool	EffectReady();
	bool	EffectFire();
	void	Clean();							// ���� �ʱ�ȭ

	vec_effectpos_t m_listEffect;		// ����Ʈ �߻���ġ

	// �ʱ⿡ ����
	CNPC*	m_effectNPC[30];
	CArea*	m_area;
	int		m_zonenum;
//	int		m_extra;

	int		m_pulseFirst;		// 1��° ����Ʈ  10�ʸ��� �ѹ�
	int		m_pulseSecond;		// 2��° ����Ʈ

	bool	m_bEffectStart;

//	int		m_guildindex1;
//	int		m_guildindex2;
};

#endif // !defined(AFX_MONSTERCOMBO_H__A0046A35_9FFA_4984_98CE_7C324A4237A6__INCLUDED_)
//
