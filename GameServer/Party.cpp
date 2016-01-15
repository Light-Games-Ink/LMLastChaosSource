#include "stdhdrs.h"

#include "Server.h"
#include "Party.h"
#include "CmdMsg.h"

#include "doFunc.h"
#include "Battle.h"

////////////////////////////
// implement of CPartyMember
CPartyMember::CPartyMember(int nCharIndex, const char* strCharName, CPC* pc)
	: m_strCharName(MAX_CHAR_NAME_LENGTH + 1)
{
	m_nCharIndex = nCharIndex;
	m_strCharName = strCharName;

	m_pChar = pc;
	if( pc ) m_nLevel = pc->m_level;
	else m_nLevel = 0;
}

CPartyMember::~CPartyMember()
{
	m_nCharIndex = 0;
	m_strCharName = "";
	m_nLevel = 0;

	m_pChar = NULL;
}

CPC* CPartyMember::GetMemberPCPtr() const
{
	if (PCManager::instance()->getPlayerByCharIndex(m_nCharIndex) != m_pChar )
	{
		return NULL;
	}
	return m_pChar;
}

void CPartyMember::SetMemberPCPtr(CPC* pc)
{
	if (m_pChar != NULL && pc == NULL)
		m_pChar->m_party = NULL;

	m_pChar = pc;
	if( pc )
		m_nLevel = pc->m_level;
}

CParty::CParty(char nPartyType, int nBossIndex, const char* strBossName, CPC* pBoss, int nRequest, const char* strRequest )
	: m_strRequest(MAX_CHAR_NAME_LENGTH + 1)
{
	m_nPartyType[MSG_DIVITYPE_EXP]		= nPartyType;			   // ����ġ(�յ�(����),�Լ��켱,����)
	m_nPartyType[MSG_DIVITYPE_ITEM]		= MSG_PARTY_TYPE_FIRSTGET; // ������ ����(�Լ��켱)
	m_nPartyType[MSG_DIVITYPE_SPECIAL]	= MSG_PARTY_TYPE_OPENBOX;  // ������ ����(���ڿ���)
	m_nPartyType[MSG_DIVITYPE_SP]		= nPartyType;
	m_nPartyType[MSG_DIVITYPE_MONEY]	= nPartyType;

	memset(m_listMember, 0, sizeof(CPartyMember*) * MAX_PARTY_MEMBER);

	m_listMember[0] = new CPartyMember(nBossIndex, strBossName, pBoss );

	m_nRequest = nRequest;
	m_strRequest = strRequest;
	m_nCount = 1;

#ifdef GMTOOL
	m_bGmtarget = false;
#endif // GMTOOL

	m_comboAreaIndex = -1;

#ifdef EXTREME_CUBE
	m_cubeUniqueIdx = -1;
#endif // EXTREME_CUBE

	m_nEndPartyTime=0;      // ��Ƽ ��ü �ð�

	m_nRaidZoneNum = -1;
	m_nRaidAreaNum = -1;
	m_nSubNumber = gserver->m_subno;
}

CParty::~CParty()
{
	m_nPartyType[MSG_DIVITYPE_EXP] = -1;
	m_nPartyType[MSG_DIVITYPE_ITEM] = -1;
	m_nPartyType[MSG_DIVITYPE_SPECIAL] = -1;
	m_nPartyType[MSG_DIVITYPE_SP] = -1;
	m_nPartyType[MSG_DIVITYPE_MONEY] = -1;

	m_nCount = 0;
	int i;
	for (i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		if (m_listMember[i] != NULL)
		{
			if (m_listMember[i]->GetMemberPCPtr())
			{
				m_listMember[i]->SetMemberPCPtr(NULL);
			}
			delete m_listMember[i];
			m_listMember[i] = NULL;
		}
	}
	m_nRequest = -1;

	m_comboAreaIndex = -1;

#ifdef EXTREME_CUBE
	m_cubeUniqueIdx = -1;
#endif // EXTREME_CUBE

	m_nEndPartyTime=0;		// ��Ƽ ��ü �ð�
}

int CParty::GetMemberCountOnline()
{
	int ret = 0;
	int i = 0;
	for (i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		if( m_listMember[i] != NULL && m_listMember[i]->m_nLevel > 0 )
			ret++;
	}

	return ret;
}
//////////////////////////////////////////////////////////////////////
// ���� ���� ���� �Լ���
int CParty::GetNearPartyMemberCount(CCharacter* pCenter)
{
	int i;
	int ret = 0;
	for (i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		if(m_listMember[i] != NULL
				&& m_listMember[i]->GetMemberPCPtr()
				&& CheckInNearCellExt(pCenter, m_listMember[i]->GetMemberPCPtr())
		  )
		{
			ret++;
		}
	}

	return ret;
}

//���� �ٿ� ����(09.05.11)
void CParty::SendToAllPC(CNetMsg::SP& msg, int nExcludeCharIndex)
{
	int i;
	CPartyMember* pMember = NULL;
	for (i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		if(m_listMember[i] != NULL)
		{
			pMember =  (CPartyMember*)m_listMember[i];
			if(pMember != NULL)
			{
				if(pMember->GetCharIndex() != nExcludeCharIndex)
				{
					CPC *pPC = TO_PC(pMember->GetMemberPCPtr());
					if(pPC != NULL && pPC->m_index > 0 && pPC->m_level>0 )
					{
						SEND_Q(msg, pPC->m_desc);
					}
				}
			}
		}
	}
}
#ifdef CONTINENT_PARTY_RECALL
void CParty::SendToSameContinentPC(CNetMsg::SP& msg, int nExcludeCharIndex)
{
	CPC *pc = PCManager::instance()->getPlayerByCharIndex(nExcludeCharIndex);

	if (!pc)
		return ;

	int continent_no = -1;
	int i;
	if(!pc->m_pZone)
		return ;
	CPartyMember* pMember = NULL;
	bool IsPartyMemberinSameContinent = false;
	continent_no = pc->m_pZone->m_iContinent_no;
	for(i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		if(m_listMember[i] != NULL)
		{
			pMember =  (CPartyMember*)m_listMember[i];
			if(pMember != NULL)
			{
				if(pMember->GetCharIndex() != nExcludeCharIndex)
				{
					CPC *pPC = pMember->GetMemberPCPtr();
					if(pPC != NULL)
					{
						if(pPC->m_pZone->m_iContinent_no == continent_no)
						{
							if( pPC->m_plusEffect & CLIENT_OPTION_WARP )
							{
								CNetMsg::SP rmsg(new CNetMsg);
								WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTALLOW, pPC->GetName());
								SEND_Q( rmsg, pc->m_desc );
								IsPartyMemberinSameContinent = true;
							}
							else
							{
								if(!pPC->m_pZone->IsGuildRoom() && !pPC->m_pZone->IsPartyRaidZone())
								{
									IsPartyMemberinSameContinent = true;
									SEND_Q(msg, pPC->m_desc);
								}
							}
						}
					}
				}
			}
		}
	}
	if(!IsPartyMemberinSameContinent) // ��Ƽ�� ������ ��� ��Ƽ������ ���� ����� ���� ��..�����޼��� �߼�
	{
		PartyRecallErrorMsg(msg, MSG_EX_PARTY_RECALL_ERROR_NOT_FOUND_PARTY_PLAYER);
		SEND_Q(msg, pc->m_desc);
	}
}
#endif
//���� �ٿ� ����(09.05.11)
void CParty::SendToPCInSameZone(int nZone, int nArea, CNetMsg::SP& msg)
{
	int i;
	CPartyMember* pMember = NULL;

	for (i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		if(m_listMember[i] != NULL)
		{
			pMember = (CPartyMember*)m_listMember[i];
			if(pMember != NULL)
			{
				CPC *pPC = pMember->GetMemberPCPtr();
				if(pPC != NULL && pPC->m_index>0 && pPC->m_level > 0 &&
						pPC->m_pZone && pPC->m_pArea &&
						pPC->m_pZone->m_index == nZone &&
						pPC->m_pArea->m_index == nArea)
				{
					SEND_Q(msg, pPC->m_desc);
				}
			}
		}
	}
}

CPC* CParty::GetNearMember(CCharacter* pPC, int nListIndex)
{
	if (nListIndex < 0 || nListIndex >= MAX_PARTY_MEMBER)
		return NULL;
	if (m_listMember[nListIndex] == NULL)
		return NULL;
	if (m_listMember[nListIndex]->GetMemberPCPtr() == NULL)
		return NULL;
	if (!CheckInNearCellExt(pPC, m_listMember[nListIndex]->GetMemberPCPtr()))
		return NULL;
	return m_listMember[nListIndex]->GetMemberPCPtr();
}

void CParty::SetMemberPCPtr(int nCharIndex, CPC* pPC)
{
	int i;
	for (i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		if(m_listMember[i] != NULL
				&& m_listMember[i]->GetCharIndex() == nCharIndex
		  )
		{
			m_listMember[i]->SetMemberPCPtr(pPC);
			return ;
		}
	}
	return ;
}

// ��Ƽ ��ü �ð� ���� (20�� ��� �� ��ü)
void CParty::SetEndPartyTime(int nNow)
{
	m_nEndPartyTime = nNow;
}

// ��Ƽ ��ü �ð� ȹ�� (20�� ��� �� ��ü)
int CParty::GetEndPartyTime()
{
	return m_nEndPartyTime;
}

bool CParty::SetMemberRegister_AfterGoZone(CPC* pPC)
{
	if (!pPC || !pPC->m_desc)
		return false;

	SetMemberPCPtr(pPC->m_index, pPC);

	{
		//��Ƽ Ÿ�� ����
		CNetMsg::SP rmsg(new CNetMsg);
		PartyTypeinfo(rmsg, GetPartyType(MSG_DIVITYPE_SP), GetPartyType(MSG_DIVITYPE_ITEM),GetPartyType(MSG_DIVITYPE_SPECIAL));
		SEND_Q(rmsg, pPC->m_desc);
	}

	// ó�� ���ӿ� ��Ƽ�� ������
	for (int i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		const CPartyMember* pMember = GetMemberByListIndex(i);

		if (pMember && pMember->GetCharIndex() != pPC->m_index)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			PartyAddMsg(rmsg, pMember->GetCharIndex(), pMember->GetCharName(), pMember->GetMemberPCPtr(), ((GetBossIndex() == pMember->GetCharIndex()) ? 1 : 0));
			SEND_Q(rmsg, pPC->m_desc);
		}
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		PartyAddMsg(rmsg, pPC->m_index, pPC->GetName(), pPC, ((GetBossIndex() == pPC->m_index) ? 1 : 0));
		SendToAllPC(rmsg, pPC->m_index);
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		HelperPartyOnline( rmsg, GetBossIndex() , pPC->m_index , pPC->m_level );
		SEND_Q(rmsg, gserver->m_helper);
	}

	return true;
}

void CParty::SetPartyRecallUseInfo(int zoneID, CPos& pos, int guildIndex, char cIsInCastle, int nExcludeCharIndex)
{
	int i;
	CPartyMember* pMember = NULL;
	for (i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		if(m_listMember[i] != NULL)
		{
			pMember =  (CPartyMember*)m_listMember[i];
			if(pMember->GetCharIndex() != nExcludeCharIndex)
			{
				CPC *pPC = TO_PC(pMember->GetMemberPCPtr());
				if(pPC != NULL && pPC->m_index > 0 && pPC->m_level>0 )
				{
					pPC->getPartyRecallInfo()->setPartyRecallInfo(zoneID, pos, guildIndex, cIsInCastle);
				}
			}
		}
	}
}


// ���� ���� ���� �Լ���
//////////////////////////////////////////////////////////////////////

//��Ƽ ��ǥ�� ����
void CParty::SetPartyTypeAll(char cPartyType)
{
	m_nPartyType[MSG_DIVITYPE_EXP]		= cPartyType;
	m_nPartyType[MSG_DIVITYPE_SP]		= cPartyType;
	m_nPartyType[MSG_DIVITYPE_MONEY]	= cPartyType;
}

CPartyMember* CParty::GetMemberByListIndex(int nListIndex) const
{
	if (nListIndex < 0 || nListIndex >= MAX_PARTY_MEMBER)
		return NULL;
	if (m_listMember[nListIndex] == NULL)
		return NULL;
	return m_listMember[nListIndex];
}

CPartyMember* CParty::GetMemberByCharIndex(int charIndex) const
{
	for (int i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		if (	   m_listMember[i] != NULL
				   && m_listMember[i]->GetCharIndex() == charIndex
		   )
		{
			return m_listMember[i];
		}
	}
	return NULL;
}


int CParty::FindMember(int nCharIndex)
{
	int i;
	for (i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		if (	   m_listMember[i] != NULL
				   && m_listMember[i]->GetCharIndex() == nCharIndex
		   )
		{
			return i;
		}
	}
	return -1;
}

int CParty::GetMemberCharIndex(int nListIndex)
{
	if (nListIndex < 0 || nListIndex >= MAX_PARTY_MEMBER)
		return -1;
	if (m_listMember[nListIndex] == NULL)
		return -1;
	return m_listMember[nListIndex]->GetCharIndex();
}

void CParty::SetRequest(int nRequest, const char* strRequest)
{
	// ���� ���������� ��û�ڸ� ã�� ��Ƽ ������ �����Ѵ�.
	if (m_nRequest != -1)
	{
		CPC* pPC = PCManager::instance()->getPlayerByCharIndex(m_nRequest);
		if (pPC)
			pPC->m_party = NULL;
	}
	m_nRequest = nRequest;
	m_strRequest = strRequest;
}

int CParty::JoinRequest(const char* strRequestName, CPC* pRequest)
{
	if (GetRequestIndex() < 1)
		return -1;

	// ���ڸ��� ã��
	int i;
	for (i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		if (m_listMember[i] == NULL)
			break;
	}
	if (i == MAX_PARTY_MEMBER)
	{
		m_nRequest = -1;
		return -1;
	}

	// ���ڸ��� �����ϰ�
	m_listMember[i] = new CPartyMember(m_nRequest, strRequestName, pRequest);
	m_nCount++;

	// request �ʱ�ȭ �ϰ�
	m_nRequest = -1;

	updateCalcMember();

	return i;
}

void CParty::DeleteMember(int nCharIndex)
{
	this->deletePartyRecallMember(nCharIndex);

	int i;
	for (i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		if (m_listMember[i])
		{
			if (m_listMember[i]->GetCharIndex() == nCharIndex)
			{
				delete m_listMember[i];
				m_listMember[i] = NULL;
				m_nCount--;
				if (i == 0)
				{
					gserver->m_listParty.erase(nCharIndex);
				}

				CPC* pc = PCManager::instance()->getPlayerByCharIndex(nCharIndex);
				if(pc != NULL)
					pc->CalcStatus(true);
			}
		}
		if (i < MAX_PARTY_MEMBER - 1 && m_listMember[i] == NULL)
		{
			m_listMember[i] = m_listMember[i + 1];
			m_listMember[i + 1] = NULL;
			if (i == 0)
			{
				if(m_listMember[i] != NULL)
					gserver->m_listParty.insert(map_listparty_t::value_type(m_listMember[i]->GetCharIndex(), this));
			}
		}
	}
	return ;
}

bool CParty::ChangeBoss(const char* strNewBossName)
{
	int i;
	for (i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		if (m_listMember[i] && m_listMember[i]->m_nLevel > 0 )
		{
			if (strcmp(m_listMember[i]->GetCharName(), strNewBossName) == 0)
			{
				CPartyMember* pMember = m_listMember[0];
				m_listMember[0] = m_listMember[i];
				m_listMember[i] = pMember;
				gserver->m_listParty.erase(pMember->GetCharIndex());
				gserver->m_listParty.insert(map_listparty_t::value_type(m_listMember[0]->GetCharIndex(), this));
				return true;
			}
		}
	}
	return false;
}

void CParty::SetEndParty()
{
	updateCalcMember();

	{
		// ��Ƽ ��ü �˸�
		CNetMsg::SP rmsg(new CNetMsg);
		PartyMsg(rmsg, MSG_PARTY_END);
		SendToAllPC(rmsg);
	}

	// �ʴ� ����
	if (GetRequestIndex() > 0)
	{
		CPC* pRequestPC = PCManager::instance()->getPlayerByCharIndex(GetRequestIndex());
		if (pRequestPC)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			PartyMsg(rmsg, MSG_PARTY_REJECT_SRC);
			SEND_Q(rmsg, pRequestPC->m_desc);
		}
		SetRequest(-1, "");
	}

	for(int i=0; i < MAX_PARTY_MEMBER; i++)
	{
		CPartyMember* pMember = (CPartyMember*)GetMemberByListIndex(i);
		if(pMember)
		{
			CPC* pc = pMember->GetMemberPCPtr();
			if(pc && (pc->m_pZone->IsExpedRaidZone() || pc->m_pZone->IsPartyRaidZone())
					&& pc->m_nJoinInzone_ZoneNo >=0 && pc->m_nJoinInzone_RoomNo >= 0)
			{
				CNetMsg::SP rmsg(new CNetMsg);
				RaidInzoneQuitReq(rmsg,1,0);
				do_Raid(pc, rmsg);
			}
		}
	}

	CPartyMember* pMember=NULL;

	for(int i=0; i < MAX_PARTY_MEMBER; i++)
	{
		pMember = (CPartyMember*)GetMemberByListIndex(i);
		if(pMember)
			SetMemberPCPtr(pMember->GetCharIndex(), NULL);
		delete m_listMember[i];
		m_listMember[i] = NULL;
	}
}

void CParty::SendToPartyRecallPC(CNetMsg::SP& msg, int nReqIndex)
{
	CPC* reqPC = PCManager::instance()->getPlayerByCharIndex(nReqIndex);
	if(!reqPC)
		return ;
	int i;
	CPartyMember* pMember = NULL;
	for (i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		if(m_listMember[i] != NULL)
		{
			pMember =  (CPartyMember*)m_listMember[i];
			if(pMember != NULL)
			{
				if(pMember->GetCharIndex() != nReqIndex)
				{
					CPC *pPC = pMember->GetMemberPCPtr();
					if(pPC != NULL && pPC->m_index > 0 && pPC->m_level>0 )
					{
						if( pPC->m_plusEffect & CLIENT_OPTION_WARP )
						{
							CNetMsg::SP rmsg(new CNetMsg);
							WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTALLOW, pPC->GetName());
							SEND_Q( rmsg, reqPC->m_desc );							
						}
						else
						{
							if(STATE(pPC->m_desc) == CON_PLAYING)
							{
								m_warPartyRecall.insert(pMember->GetCharIndex());
							}
							SEND_Q(msg, pPC->m_desc);
						}
/////////��ȯ�ź��ڿ��Դ� �޼��� ������ ����/////////
					}
				}
			}
		}
	}
}

void CParty::deletePartyRecallMember( int memberIndex )
{
	CPC* pResPC = PCManager::instance()->getPlayerByCharIndex(memberIndex);
	if(pResPC == NULL)
		return ;

	std::set<int>::iterator it;
	it = this->m_warPartyRecall.find(memberIndex);
	if(it != this->m_warPartyRecall.end())
	{
		this->m_warPartyRecall.erase(it);
	}
}

int CParty::getGuildMemberCountInParty( CPC* pc )
{
	if(pc == NULL)
		return 0;

	int count = 0;

	CPC* member = NULL;

	for (int i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		if(m_listMember[i] != NULL)
		{
			member = m_listMember[i]->GetMemberPCPtr();
			if(member != NULL
				&& pc != NULL
				&& member->m_guildInfo != NULL
				&& pc->m_guildInfo != NULL
				&& pc->m_guildInfo->guild() == member->m_guildInfo->guild())
			{
				count++;
			}
		}
	}

	return count;
}

void CParty::calcGuildSkillExp(CPC* pc, int exp)
{
	//��� ����Ȱ�� ��ų�� ���� ����ġ ���
	//TODO
	/*
	1. ��Ƽ���߿��� ��� ��ų�� ������ ������ �ִ��� Ȯ��
	2. ������ ���鼭 ��ų�� ������ ������ �ִٸ� �ʿ� ��� �ε����� ����ġ ������ ������Ʈ
	2-1. �ʿ� �����Ͱ� �̹� �����ϴ� ��쿡�� ���� �����Ϳ� �߰� �����͸� ���ظ� �ְ� ���� ��쿡�� ���� �߰�.
	3. 
	*/
	//��ų�� ����� ������ �ִ� ��쿡��

	if(pc == NULL)
		return ;
	
	if(pc->m_guildInfo == NULL)
		return ;

	if(pc->m_assist.FindBySkillIndex(SKILL_GUILD_PARTY_EXP) != 0)
	{
		//����ġ�� ������ �۾��� ���ش�. ������ ������ 2���̻� ������ ���
		int member_count = getGuildMemberCountInParty(pc);
		if( member_count >= 2)
		{
			exp = exp / (member_count -1);

			//�ʿ��� ã�� ������ insert ������ update
			GUILD_SKILL_EXP::iterator it = m_guild_skill_exp.find(pc->m_guildInfo->guild()->index());
			if(it != m_guild_skill_exp.end())
			{
				it->second += exp;
			}
			else
			{
				m_guild_skill_exp.insert(GUILD_SKILL_EXP::value_type(pc->m_guildInfo->guild()->index(), exp));
			}
		}
	}
}

int CParty::getGuildSkillExp( CPC* pc )
{
	if(pc == NULL)
		return 0;

	if(pc->m_assist.FindBySkillIndex(SKILL_GUILD_PARTY_EXP) != 0)
		return 0;

	if(pc->m_guildInfo == NULL)
		return 0;

	GUILD_SKILL_EXP::iterator it = m_guild_skill_exp.find(pc->m_guildInfo->guild()->index());

	if(it == m_guild_skill_exp.end())
		return 0;

	return it->second;
}

void CParty::updateCalcMember()
{
	CPC* member = NULL;
	for(int i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		if(m_listMember[i] == NULL)
			continue;

		member = m_listMember[i]->GetMemberPCPtr();

		if(member == NULL)
			continue;

		
		member->CalcStatus(true);
	}
}

void CParty::clearGuildSkillExp( CPC* pc )
{
	m_guild_skill_exp.clear();
}



