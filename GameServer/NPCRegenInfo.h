#ifndef __ANPCREGENINFO_H__
#define __ANPCREGENINFO_H__

class CNPCRegenInfo
{
public:
	CNPCRegenInfo();
	~CNPCRegenInfo();

	////////////////////////////////////////
	// NPC_REGEN_INFO ���� �б�

	// DB �ʵ�
	int		m_index;			// ��� ���ڵ� �ε���
	int		m_npcIdx;			// �� �ε���
	int		m_zoneNo;			// ����ȣ
	float	m_regenX;			// ���� ��ġ : x
	int		m_regenY;			// ���� ��ġ : yLayer
	float	m_regenZ;			// ���� ��ġ : z
	float	m_regenR;			// ���� ��ġ : r
	int		m_regenSec;			// ���� ������ : ��
	int		m_totalNum;			// �ִ� ���� �� : �ִ� ���� �Ǵ� ��

	// ���������� ������ ���� ���
	bool	m_bAlive;			// ���� ���� ��� �ִ��� �׾�����
	int		m_lastDieTime;		// �ֱ� ���� �ð�
	int		m_numRegen;			// ���� ���� ��

	// �����Ǵ� NPC�� �Ķ����
	int		m_paramHP;			// HP�� ����

	bool	m_bRaidMoveRandom;	// RAID �� ������ �̵� ����

	bool		m_bRegen;			// npc ���� ���� ����

	CNPCProto*	m_npcProto;

	void SetHPParam(int val)
	{
		m_paramHP = val;
	}
	void clean();
};

class CArea;

class CNPCRegenList
{
public:
	CNPCRegenList();
	~CNPCRegenList();

	int				m_type;		// ����
	CNPCRegenInfo*	m_infoList;	// ���
	int				m_nCount;	// ��
	int				m_wPos;		// ���� ���� ��ġ

	void Init();
	void SetCount(int n);
	void AddRegenInfo(int index, int npc_index, int regenSec, float regenX, int yLayer, float regenZ, float regenR, int totalNum, int zonenNo, bool bRegen = true, CNPCProto* pNpcProto = NULL);
//	void AddRegenInfo(CNPCRegenInfo* info);

	bool Find(int idx, int zoneNo, bool pdZone = false, float x = 0, int y = 0, float z = 0);

	bool DelRegenInfo(CNPCRegenInfo * p);

	// �۴��� : bw
	bool SetRegened(CArea* pArea, int protoindex, int zoneNo, float x = 0, int y = 0, float z = 0);
	/*
	{
		int i, j, index;
		if( !pArea ) return false;

		if( !pArea->m_regenList ) return false;

		for(i = 0; i < m_nCount; i++)
		{
			if( m_infoList[i].m_zoneNo == zoneNo )
			{
				for(j = 0; j < m_nCount; j++)
				{
					if( m_infoList[i].m_index == pArea->m_regenList[j] )
					{
						pArea->m_regenList[j] = -1;
						return true;
					}
				}
			}
		}

		return false;
	}
	*/

	/*
	bool Find(int idx, int zoneNo)
	{
		CNPCRegenInfo s;
		s.m_npcIdx = idx;
		s.m_zoneNo = zoneNo;

		CNPCRegenInfo* result = (CNPCRegenInfo*)bsearch(&s, m_infoList, m_nCount, sizeof(CNPCRegenInfo), CompIndex);

		if(result == NULL)
		{
			delete result;
			return false;
		}

		result = NULL;
		delete result;
		return true;
	}

	private:
	static int CompIndex(const void* p1, const void* p2)
	{
		CNPCRegenInfo * i1 = (CNPCRegenInfo*) p1;
		CNPCRegenInfo * i2 = (CNPCRegenInfo*) p2;

		if( (i1->m_npcIdx - i2->m_npcIdx) == 0 )
			return i1->m_zoneNo - i2->m_zoneNo;

		return i1->m_npcIdx - i2->m_npcIdx;
	}
	*/

	CNPCRegenInfo * GetNpcRegenInfo(int idx);	// ����Ʈ�� ����� �ε����� CNPCRegenInfo ��������
};

#endif
//
