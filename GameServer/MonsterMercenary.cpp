#include "stdhdrs.h"

#include "Server.h"
#include "../ShareLib/DBCmd.h"
#include "CmdMsg.h"
#include "MonsterMercenary.h"
#include "Log.h"

CMonsterMercenary::CMonsterMercenary()
{
	Init();
}

CMonsterMercenary::~CMonsterMercenary()
{
	ReleaseAll();
}

void CMonsterMercenary::Init()
{
	m_Data = NULL;
}
void CMonsterMercenary::ReleaseAll()
{
	if( m_Data )
	{
		int i=0;
		for(i=0; i<m_count; i++)
		{
#ifdef SYSTEM_MONSTER_MERCENARY_CARD_EX
			if( m_Data[i].npcInfo )
			{
				delete [] m_Data[i].npcInfo;
				m_Data[i].npcInfo = NULL;
			}
#else
			if( m_Data[i].npcList )
			{
				delete [] m_Data[i].npcList;
				m_Data[i].npcList = NULL;
			}
#endif
		}
		delete [] m_Data;
		m_Data = NULL;
	}
}
bool CMonsterMercenary::LoadData()
{
	CLCString sql(1024);
#ifdef SYSTEM_MONSTER_MERCENARY_CARD_EX
	sql = "SELECT * FROM t_monster_mercenary_ex order by a_class";
#else
	sql = "SELECT * FROM t_monster_mercenary group by a_class order by a_class";
#endif

	CDBCmd cmd;
	cmd.Init(&gserver->m_dbdata);
	cmd.SetQuery(sql);

	if( !cmd.Open() )
		return false;

	if( !cmd.MoveFirst() )
		return false;

	m_count = cmd.m_nrecords;
	MonsterMercenaryInfo * mmd = new MonsterMercenaryInfo[m_count];
	int i=0;

#ifdef SYSTEM_MONSTER_MERCENARY_CARD_EX
	do
	{
		CLCString buf(2048);
		cmd.GetRec("a_class"		, mmd[i].nClassIdx );
		cmd.GetRec("a_prob"			, mmd[i].nProb);
		cmd.GetRec("a_att_def"		, mmd[i].nAttDef);
		cmd.GetRec("a_hpmp"			, mmd[i].nHpMp);
		cmd.GetRec("a_hit"			, mmd[i].nHit);
		cmd.GetRec("a_dodge"		, mmd[i].nDodge);
		cmd.GetRec("a_bonus_mini"	, mmd[i].nBonusMini);
		cmd.GetRec("a_bonus_max"	, mmd[i].nBonusMax);
		cmd.GetRec("a_npc_count"	, mmd[i].nNpcCount);
		cmd.GetRec("a_npc_info"		, buf);

		mmd[i].npcInfo = new MercenaryNpcInfo[ mmd[i].nNpcCount ];

		if( buf )
		{
			char buf1[32]	= {0,};
			const char * p	= buf;

			int j=0;
			for(j=0; j<mmd[i].nNpcCount; j++)
			{
				p=AnyOneArg(p, buf1);
				if( strlen(buf1) < 1 )
					break;
				mmd[i].npcInfo[j].index = atoi(buf1);

				p=AnyOneArg(p, buf1);
				if( strlen(buf1) < 1 )
					break;
				mmd[i].npcInfo[j].walkSpeed = atoi(buf1);

				p=AnyOneArg(p, buf1);
				if( strlen(buf1) < 1 )
					break;
				mmd[i].npcInfo[j].runSpeed = atoi(buf1);

				p=AnyOneArg(p, buf1);
				if( strlen(buf1) < 1 )
					break;
				mmd[i].npcInfo[j].size = atoi(buf1);
			}
		}
		i++;
	}
	while( cmd.MoveNext() );

#else

	do
	{
		cmd.GetRec("a_class"		, mmd[i].nClassIdx );
		cmd.GetRec("a_prob"			, mmd[i].nProb);
		cmd.GetRec("a_att_def"		, mmd[i].nAttDef);
		cmd.GetRec("a_hpmp"			, mmd[i].nHpMp);
		cmd.GetRec("a_hit"			, mmd[i].nHit);
		cmd.GetRec("a_dodge"		, mmd[i].nDodge);
		cmd.GetRec("a_bonus_mini"	, mmd[i].nBonusMini);
		cmd.GetRec("a_bonus_max"	, mmd[i].nBonusMax);

		i++;
	}
	while( cmd.MoveNext() );

	for(i=0; i<m_count; i++)
	{
		sql.Format("select * from t_monster_mercenary where a_class=%d order by a_npc_idx", mmd[i].nClassIdx );
		cmd.SetQuery(sql);

		if( !cmd.Open() || !cmd.MoveFirst() )
		{
			return false;
		}

		mmd[i].nNpcCount = cmd.m_nrecords;
		mmd[i].npcList =new int[ mmd[i].nNpcCount ];

		int j=0;
		do
		{
			cmd.GetRec("a_npc_idx", mmd[i].npcList[j] );
			j++;
		}
		while( cmd.MoveNext() );
	}

#endif
	m_Data = mmd;
	mmd = NULL;
	return true;
}

MonsterMercenaryInfo * CMonsterMercenary::GetClassInfo()
{
	if( m_Data == NULL )
		return NULL;

	int curProb = GetRandom( 0, 10000 );

	int i=0, totalProb=0;
	for(i=0; i<m_count; i++)
	{
		totalProb += m_Data[i].nProb;
		if( totalProb >= curProb )
			return &m_Data[i];
	}

	return NULL;
}

int CMonsterMercenary::GetMercenaryIndex(int classIdx)
{
	if( m_Data == NULL )
		return -1;

	MonsterMercenaryInfo * mmd = GetClassInfo(classIdx);
#ifdef SYSTEM_MONSTER_MERCENARY_CARD_EX
	if( mmd == NULL || mmd->npcInfo == NULL ||  mmd->nNpcCount < 1 )
		return -1;
#else
	if( mmd == NULL || mmd->npcList == NULL ||  mmd->nNpcCount < 1 )
		return -1;
#endif

	int idx = GetRandom(0, mmd->nNpcCount-1);

#ifdef SYSTEM_MONSTER_MERCENARY_CARD_EX
	return mmd->npcInfo[idx].index;
#else
	return mmd->npcList[idx];
#endif
}

MonsterMercenaryInfo * CMonsterMercenary::GetClassInfo(int classIdx)
{
	if( m_Data == NULL )
		return NULL;

	int i=0;
	for(i=0; i<m_count; i++)
	{
		if( m_Data[i].nClassIdx == classIdx)
			return &m_Data[i];
	}
	return NULL;
}

MonsterMercenaryData * CMonsterMercenary::CreateClassData(int classIdx, int npcIdx)
{
	if( m_Data == NULL )
		return NULL;

	int i=0;
	for(i=0; i<m_count; i++)
	{
		if( m_Data[i].nClassIdx == classIdx)
		{
			MonsterMercenaryData * pCData = new MonsterMercenaryData;
			pCData->nClassIdx	= m_Data[i].nClassIdx;
			pCData->nAttDef		= m_Data[i].nAttDef;
			pCData->nHpMp		= m_Data[i].nHpMp;
			pCData->nHit		= m_Data[i].nHit;
			pCData->nDodge		= m_Data[i].nDodge;
#ifdef SYSTEM_MONSTER_MERCENARY_CARD_EX
			if( npcIdx -1 )
			{
				pCData->nWalkSpeed	= 8.0f;
				pCData->nRunSpeed	= 8.0f;
				pCData->nSize		= 1.0f;
			}
			else
			{
				int j=0;
				for(j=0; j<m_Data[i].nNpcCount; j++)
				{
					if(m_Data[i].npcInfo[j].index == npcIdx )
					{
						pCData->nWalkSpeed	= m_Data[i].npcInfo[j].walkSpeed;
						pCData->nRunSpeed	= m_Data[i].npcInfo[j].runSpeed;
						pCData->nSize		= m_Data[i].npcInfo[j].size;
					}
				}
			}
#endif
			return pCData;
		}
	}
	return NULL;
}

#ifdef SYSTEM_MONSTER_MERCENARY_CARD_EX
bool CMonsterMercenary::SetMercenaryData( CItem* pItem, int classIdx, int npcIndex)
{
	if( m_Data == NULL )
		return false;

	if( pItem == NULL )
		return false;

	MonsterMercenaryInfo * mmd;

	// ������ Ŭ������ ���� �޴´�.
	if( classIdx < 0 )
		mmd = GetClassInfo();
	else
		mmd = GetClassInfo(classIdx);

	if( mmd == NULL )
	{
		GAMELOG << init("MOSTER MERCENARY FAIL : get MercenaryInfo") << end;
		return false;
	}

	// ������ ��� ���ɽð� �Ⱓ�� / �ð���(ABS)
	if( pItem->m_itemProto->getItemFlag() & ITEM_FLAG_ABS )
	{
		pItem->setUsed(-1);
	}
	else
	{
		// �Ⱓ�� �������� �����Ҷ����� �ð� üũ
		pItem->setUsed(gserver->getNowSecond() + pItem->m_itemProto->getItemNum3() * 24 * 60 * 60); // m_num3 = day
		pItem->setFlag(pItem->getFlag() | FLAG_ITEM_SEALED); // �ŷ��� �ʵǵ��� ���� �ع�����.
	}

	// Npc DBIndex ����.
	if( npcIndex < 0 )
	{
		pItem->setPlus(gserver->m_MonsterMercenary.GetMercenaryIndex(mmd->nClassIdx)); // Mercenary npc index
	}
	else
	{
		pItem->setPlus(npcIndex);	   // Mercenary npc index
	}

	pItem->setUsed_2(mmd->nClassIdx);  // classIdx
	pItem->setPlus_2(0); // ���� 16 ��Ʈ�� ���ݷ��� �ִ´�. ���� 16��Ʈ�� ������ �ִ´�.

	int att=0, def=0;
	int nRand = GetRandom(0,1);

	switch( nRand )
	{
	case 0: // ���ݸ� ���ʽ��ش�.
		att = GetRandom(mmd->nBonusMini, mmd->nBonusMax);
		break;
	case 1: // ���� ���潺�� �ش�.
		def = GetRandom(mmd->nBonusMini, mmd->nBonusMax);
		break;
	}

	pItem->setPlus_2(pItem->getPlus_2() + (att << 8));
	pItem->setPlus_2(pItem->getPlus_2() + def);

	if( pItem->getUsed_2() == -1 )
	{
		GAMELOG << init("MOSTER MERCENARY FAIL : get MercenaryNpcIndex") << end;
		return false;
	}
	return true;
}

#endif // SYSTEM_MONSTER_MERCENARY_CARD_EX
//