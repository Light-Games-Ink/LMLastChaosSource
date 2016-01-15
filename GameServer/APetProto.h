#ifndef __APET_PROTO_H__
#define __APET_PROTO_H__

#define HUMAN_PET_TYPE 1
#define BEAST_PET_TYPE 2
#define DEMON_PET_TYPE 3

enum APET_TRANS_TYPE		{ APET_TRANS_NONE=0, APET_TRANS_TIME, APET_TRANS_FAITH, APET_TRANS_STM };
enum APET_EVOLUTION_TYPE	{ APET_EV_NONE=0, APET_EV_STR, APET_EV_INT, APET_EV_DEX, APET_EV_CON };

#define APET_FLAG_ATTACK	(1<<0)
#define APET_FLAG_EXP		(1<<1)

#include <vector>
#include <map>

typedef struct _evinfo
{
	int		nLevel;
	int		nStm;
	int		nFaith;
	int		nStatType[2];	// APET_EVOLUTION_TYPE
	int		nEvPetIndex;
} APET_EV_INFO;

typedef struct _apetexpinfo
{
	int	nMaxAccExpParam1;
	int	nMaxAccExpParam2;
	int	nAccExpRateParam1;
	int	nAccExpRateParam2;
	int	nCooltime;
	int	nCooltimeRate;
} APET_EXP_INFO;

class CAPetProto
{
private:
	int		m_nIndex;
	int		m_nFlag;
	APET_EXP_INFO*	m_pExpInfo;

public:
	int		m_Type;
	CLCString	m_strName;
	// Bagic
	int		m_nStr;
	int		m_nCon;
	int		m_nDex;
	int		m_nInt;
	int		m_nItemIndex;
	int		m_nMaxFaith;
	int		m_nMaxStm;
	int		m_nMaxHP;
	int		m_nMaxMP;
	int		m_nRecoverHP;
	int		m_nRecoverMP;
	int		m_nAISlot;
	// Attack
	int		m_nAttack;
	int		m_nDefence;
	int		m_nMagicAttack;
	int		m_nMagicDefence;
	int		m_nHitPoint;
	int		m_nAvoidPoint;
	int		m_nMagicAvoidPoint;
	int		m_nAttackSpeed;
	int		m_nDeadly;
	int		m_nCritical;
	int		m_nAwful;
	int		m_nStrong;
	int		m_nNormal;
	int		m_nWeek;

	int		m_nDelay;			// ���ȯ �ð�
	int		m_nBagicSkill1;		// �� ������ �����Ǵ� �⺻ ��ų 1
	int		m_nBagicSkill2;		// �� ������ �����Ǵ� �⺻ ��ų 2
	int		m_TansType;			// ���ŵɶ� ����ϴ� Ÿ��APET_TRANS_TYPE
	int		m_nTransStart;		// ���� ���� ���� 1
	int		m_nTransEnd;		// ���� ���� ���� 2
	int		m_nMount[2];		// �¿�� �ִ� �ִ� �ο���
	int		m_nSummonSkill[2];		// ��ȯ�ɶ� ���Ǵ� ��ų
	int		m_nSpeed[2];

	std::vector<APET_EV_INFO>		m_listEvInfo;

public:
	CAPetProto();
	~CAPetProto();

	int		Index()
	{
		return m_nIndex;
	}
	void	Index( int idx )
	{
		if(m_nIndex<0) m_nIndex = idx;    // Index �� �������� ���� ����
	}

	void	InitValue();

	void					SetFlag( const int _flag );
	bool					IsFlag( const int _flag )
	{
		return ((m_nFlag&_flag)==_flag);
	}
	const APET_EXP_INFO*	GetExpInfo();
	void					SetExpInfo(const APET_EXP_INFO* _expInfo );
};

class CAPet;

class CAPetProtoList
{
public:
	typedef std::map<int, CAPetProto*> map_t;

public:
	CAPetProtoList();
	~CAPetProtoList();

	CAPetProto*	m_pApetProtoList;		// Apet List [Arrary]

	bool		Load();						// APet �⺻ ����(Proto) �б�

	CAPet*		Create(CPC* pOwner, int dbpet_idx, int proto_idx );			// �ش� idx�� PET �����ϱ� ( new �� �������� Delete ���ٲ� )
	CAPetProto*	FindProto(int apet_idx);		// �ش� idx�� ���� ��ȯ

private:
	map_t		map_;
};

#endif //__APET_PROTO_H__
//
