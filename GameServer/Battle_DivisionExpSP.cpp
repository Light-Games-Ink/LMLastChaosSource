#include "stdhdrs.h"

#include "Server.h"
#include "CmdMsg.h"
#include "Exp.h"
#include "Log.h"
#include "Battle.h"
#include "Party.h"

LONGLONG GetTeachSPBonus(CPC *ch, LONGLONG sp);

// ����ġ ���� ���� ��� ����Ʈ��
typedef struct __tagExpList
{
	CPC*					pc;		// ���޹��� ���
	struct __tagExpList*	next;	// ���� ��� ������
} EXP_LIST;

// ����ġ �г�Ƽ ���̺� [%] : 1���� ���� �ִ� 6����
static const int levelPenalty[6] = {95, 90, 75, 60, 45, 10};

// ��Ƽ ����ġ ���
void DivisionExpSPParty(CParty* party, CNPC* npc, CPC* pPreferencePC, LONGLONG nTotalDamage);

// ������ ����ġ ���
void DivisionExpSPExped(CExpedition* Exped, CNPC* npc, CPC* pPreferencePC, LONGLONG nTotalDamage);

// ����ġ ���� ���Ƽ ���� ����
int GetExpLevelPenalty(int nLevelDiff);
// SP ���� �г�Ƽ ���� ����
int GetSPLevelPenalty(int nLevelDiff);

// npc ����� ����ġ ���
// �۽��� �������� �������� ���� �� �̵� �߻��ÿ��� true�� ����
bool DivisionExpSP(CNPC* npc, CPC* pPreferencePC, LONGLONG nTotalDamage)
{
#ifdef PREMIUM_CHAR
	LONGLONG		nExpNPC;
	LONGLONG		nSPNPC;
	if(pPreferencePC != NULL)
	{
		nExpNPC = npc->m_proto->getExpForPremiumChar(pPreferencePC->m_premiumChar.getPremiumType());			// ���޵� ����ġ
		nSPNPC = npc->m_proto->getSkillPointForPremiumChar(pPreferencePC->m_premiumChar.getPremiumType());	// ���޵� SP
	}
	else
	{
		nExpNPC = npc->m_proto->getExp();			// ���޵� ����ġ
		nSPNPC = npc->m_proto->getSkillPoint();	// ���޵� SP
	}
#else
	LONGLONG		nExpNPC = npc->m_proto->getExp();			// ���޵� ����ġ
	LONGLONG		nSPNPC = npc->m_proto->getSkillPoint();	// ���޵� SP
#endif

	EXP_LIST*		expList = NULL;							// ����ġ ���� ��� ����Ʈ
	EXP_LIST*		expNode = NULL;							// ��� ����Ʈ ���
	CAttackChar*	pAttackChar = NULL;						// ������ ����Ʈ ������
	CAttackChar*	pAttackCharNext = npc->m_attackList;	// ������ ����Ʈ ������
	CPC*			pExpPC = NULL;							// ���޹��� ���

	if (nTotalDamage < 1)
		return false;

	// ������ �������� �� PC�� ������� ����
	while ((pAttackChar = pAttackCharNext))
	{
		pAttackCharNext = pAttackCharNext->m_next;

		if (!pAttackChar->ch)
			continue ;

		// ��ó�� �־�� ��� ����
		if (!CheckInNearCellExt(npc, pAttackChar->ch))
			continue ;

		// ������� �־�� ��
		if (pAttackChar->m_damage < 1)
			continue ;

		// ���� ��� PC�� ����
		switch (pAttackChar->ch->m_type)
		{
		case MSG_CHAR_PC:
			pExpPC = TO_PC(pAttackChar->ch);
			break;
		case MSG_CHAR_PET:
			pExpPC = TO_PC(TO_PET(pAttackChar->ch)->GetOwner());
			break;
		case MSG_CHAR_ELEMENTAL:
			pExpPC = TO_PC(TO_ELEMENTAL(pAttackChar->ch)->GetOwner());
			break;
		case MSG_CHAR_APET:
			pExpPC = TO_PC(TO_APET(pAttackChar->ch)->GetOwner());
			break;
		case MSG_CHAR_NPC:
			pExpPC = TO_PC(TO_NPC(pAttackChar->ch)->GetOwner());
			break;
		default:
			pExpPC = NULL;
			break;
		}

		// ���� ����� �־�� ��
		if (pExpPC == NULL)
			continue ;

		// ���� ����� ���� : ��/��ȯ��� ���� ������� PC�� ����
		if (pExpPC->m_nExpDamage + pAttackChar->m_damage > 1)
			pExpPC->m_nExpDamage += pAttackChar->m_damage;

		// ��� ����Ʈ�� �߰�
		expNode = new EXP_LIST;
		expNode->pc = pExpPC;
		expNode->next = expList;
		expList = expNode;
	}
	// -- ������ �������� ��� ����Ʈ �ۼ� ��

	// ��� ����Ʈ�� ���鼭 �ش� ������� ����ġ ����
	expNode = expList;
	while (expNode)
	{
		// ���� ����� ���� ����� �˻�
		if (expNode->pc->m_nExpDamage > 0)
		{
			// ��Ƽ ���ο� ���� ó�� ����
			if (expNode->pc->IsParty())
			{
				// ��Ƽ�� ��� ���� ó�� : expNode ���� ��常 ó��
				DivisionExpSPParty(expNode->pc->m_party, npc, pPreferencePC, nTotalDamage);
			}
			else if(expNode->pc->IsExped())
			{
				DivisionExpSPExped(expNode->pc->m_Exped, npc, pPreferencePC, nTotalDamage);
			}

			else
			{
				// ��Ƽ�� �ƴϸ� ����� ������ �°� ����

				// ������ ���� ���� ����
				int nLevelDiff = expNode->pc->m_level - npc->m_level;

				// ����ġ/SP �г�Ƽ ����
				LONGLONG nExpPenalty = GetExpLevelPenalty(nLevelDiff);
				LONGLONG nSPPenalty  = GetSPLevelPenalty(nLevelDiff);

				// ���� ����ġ ���� : (NPC�� * �г�Ƽ * �����) / (100 * ��ü �����)
				LONGLONG nGiveExp	= (nExpNPC	* nExpPenalty	* expNode->pc->m_nExpDamage) / (100 * nTotalDamage);
				LONGLONG nGiveSP	= (nSPNPC	* nSPPenalty	* expNode->pc->m_nExpDamage) / (100 * nTotalDamage);

				// ���� +- 10% ����
				nGiveExp = nGiveExp + (nGiveExp * GetRandom(-10, 10) / 100);
				nGiveSP  = nGiveSP  + (nGiveSP  * GetRandom(-10, 10) / 100);

				nGiveSP += GetTeachSPBonus(expNode->pc, nGiveSP );

				// EP = ( NPC����  * ��Ÿ(1) ) + ( NPC���� * ������ / NPC HP ) / 2
				char lastAttack = 0;
				if( pPreferencePC==expNode->pc )
					lastAttack = 1;
				// ������ ����
				int	recalcDamage  = (int) expNode->pc->m_nExpDamage ;
				if( recalcDamage > npc->m_maxHP )
					recalcDamage = npc->m_maxHP;

#ifdef PREMIUM_CHAR
				LONGLONG nGetEnergy;
				if(pPreferencePC != NULL)
				{
					nGetEnergy = (LONGLONG)npc->m_proto->getSkillPointForPremiumChar(pPreferencePC->m_premiumChar.getPremiumType()) * (LONGLONG)recalcDamage / (LONGLONG)npc->m_maxHP;
				}
				else
				{
					nGetEnergy = (LONGLONG)npc->m_proto->getSkillPoint() * (LONGLONG)recalcDamage / (LONGLONG)npc->m_maxHP;
				}
#else
				LONGLONG nGetEnergy = (LONGLONG)npc->m_proto->getSkillPoint() * (LONGLONG)recalcDamage / (LONGLONG)npc->m_maxHP;
#endif

				int ep = ( npc->m_proto->m_level * lastAttack ) + ( (int)nGetEnergy ) / 2 ;

				if(ABS(npc->m_level - expNode->pc->m_level) < 7)
				{
					expNode->pc->AddEP( ep );
				}
				expNode->pc->CalcStatus(true);

				int GuildExp = 0;
				if( expNode->pc->m_guildInfo && expNode->pc->m_guildInfo->guild() )
				{
					int guildindex = expNode->pc->m_guildInfo->guild()->index();
					CGuild* guild = gserver->m_guildlist.findguild( guildindex );

					if( expNode->pc->m_guildInfo->GetcontributeExp() != 0 )
					{
						GuildExp = nGiveExp * expNode->pc->m_guildInfo->GetcontributeExp() / 100;
						expNode->pc->m_GuildExp += GuildExp;
						nGiveExp = nGiveExp - GuildExp;  // ��� �⿩���� ���� ����ġ ��ȯ
						int GP = 0;
						if( expNode->pc->m_GuildExp >= 10000 )
						{
							GP = expNode->pc->m_GuildExp / 10000;
							expNode->pc->m_GuildExp = expNode->pc->m_GuildExp % 10000;
							guild->AddGuildPoint( GP );
							expNode->pc->m_guildInfo->AddCumulatePoint( GP );

							{
								CNetMsg::SP rmsg(new CNetMsg);
								HelperNewGuildPointUpdate( rmsg, expNode->pc->m_index, guild->index(), GP );
								SEND_Q(rmsg, gserver->m_helper);
							}

							{
								CNetMsg::SP rmsg(new CNetMsg);
								HelperSaveGuildMemberPointMsg( rmsg, guild->index(), expNode->pc->m_index, GP );
								SEND_Q( rmsg, gserver->m_helper );
							}
						}
					}
				}

				// ����
				if (nGiveExp < 1)	nGiveExp = 1;
				if (nGiveSP  < 1)	nGiveSP  = 1;

				// SP�� MAX_SKILLPOINT�� ���� �� ����
				if (nGiveSP > MAX_SKILLPOINT) nGiveSP = MAX_SKILLPOINT;

				// 9�� �̺�Ʈ : ����ڰ� �켱�����̸�
				bool bArtifact = false;
				if (expNode->pc == pPreferencePC)
				{
					// ����ġ 4��
					if (expNode->pc->m_assist.m_avAddition.hcSepExp)
					{
						expNode->pc->m_assist.CureByItemIndex(882);	// ����ġ
						nGiveExp = nGiveExp * 4;
						CNetMsg::SP rmsg(new CNetMsg);
						EventErrorMsg(rmsg, MSG_EVENT_ERROR_SEPTEMBER_EXP);
						SEND_Q(rmsg, expNode->pc->m_desc);

						//���� ������ �������� �ʵ��� ó��
						bArtifact = true;
					}
					// ���� ����ġ 4��
					else if (expNode->pc->m_assist.m_avAddition.hcSepExp_Cash)
					{
						expNode->pc->m_assist.CureByItemIndex(6094);	// ����ġ
						nGiveExp = nGiveExp * 4;
						CNetMsg::SP rmsg(new CNetMsg);
						EventErrorMsg(rmsg, MSG_EVENT_ERROR_SEPTEMBER_EXP);
						SEND_Q(rmsg, expNode->pc->m_desc);

						//���� ������ �������� �ʵ��� ó��
						bArtifact = true;
					}
					else if( expNode->pc->m_assist.m_avAddition.hcRandomExpUp > 0  && GetRandom(0,100) <= expNode->pc->m_assist.m_avAddition.hcRandomExpUp )
					{
						nGiveExp = nGiveExp * 4;

						CNetMsg::SP rmsg(new CNetMsg);
						EffectEtcMsg(rmsg, expNode->pc, MSG_EFFECT_ETC_RANDOM_EXP);
						expNode->pc->m_pArea->SendToCell(rmsg, expNode->pc, true);

						bArtifact = true;
					}

					// 9�� �̺�Ʈ : SP 4��
					if (expNode->pc->m_assist.m_avAddition.hcSepSP)
					{
						expNode->pc->m_assist.CureByItemIndex(883);	// ���õ�
						nGiveSP = nGiveSP * 4;
						CNetMsg::SP rmsg(new CNetMsg);
						EventErrorMsg(rmsg, MSG_EVENT_ERROR_SEPTEMBER_SP);
						SEND_Q(rmsg, expNode->pc->m_desc);
					}
					// ���� SP 4��
					else if (expNode->pc->m_assist.m_avAddition.hcSepSP_Cash)
					{
						expNode->pc->m_assist.CureByItemIndex(6095);	// ���õ�
						nGiveSP = nGiveSP * 4;
						CNetMsg::SP rmsg(new CNetMsg);
						EventErrorMsg(rmsg, MSG_EVENT_ERROR_SEPTEMBER_SP);
						SEND_Q(rmsg, expNode->pc->m_desc);
					}
					else if( expNode->pc->m_assist.m_avAddition.hcRandomSpUp > 0  && GetRandom(0,100) <= expNode->pc->m_assist.m_avAddition.hcRandomSpUp )
					{
						nGiveSP = nGiveSP * 4;

						CNetMsg::SP rmsg(new CNetMsg);
						EffectEtcMsg(rmsg, expNode->pc, MSG_EFFECT_ETC_RANDOM_SP);
						expNode->pc->m_pArea->SendToCell(rmsg, expNode->pc, true);
					}
				}

#ifdef NEW_ACCERY_ADD
				if( expNode->pc->m_AddProb )
				{
					nGiveExp = nGiveExp + ( nGiveExp * expNode->pc->m_AddProb / 100 );
					nGiveSP = nGiveSP + ( nGiveSP * expNode->pc->m_AddProb / 100 );
				}
#endif //NEW_ACCERY_ADD

				CAPet* apet = expNode->pc->GetAPet();
				if( apet )
				{
					LONGLONG petexp = (npc->m_level) / 5 * ( npc->m_proto->CheckFlag(NPC_MBOSS|NPC_BOSS)? 4:1 );
					apet->AddExpSP( petexp , 0 );

					//���� �������� ��� ���̰�, �븮���� ���� ��ϵǾ� ���� ���
					if( expNode->pc->m_petStashManager.getProxyPetItemvIndex() > -1 )
					{
						int petVIndex = expNode->pc->m_petStashManager.getProxyPetItemvIndex();
						int petPlus = expNode->pc->m_petStashManager.getProxyPetItemPlus();
						if(expNode->pc->GetAPet(petPlus))
						{
							CAPet* proxyPet = expNode->pc->m_petStashManager.getProxyAPet();
							if(proxyPet != NULL)
							{
								petexp /= 10;
								proxyPet->AddExpSP(petexp, 0);
							}
						}
						else if(expNode->pc->GetPet(petPlus))
						{
							CPet* proxyPet = expNode->pc->m_petStashManager.getProxyPet();
							if(proxyPet != NULL)
							{
								if(proxyPet->m_level < PET_MAX_LEVEL
									&& proxyPet->GetHungryPoint() > 0)
								{
									petexp /= 10;
									proxyPet->m_exp += petexp;
									if(proxyPet->m_exp > proxyPet->GetNeedExp())
									{
										proxyPet->m_level++;
										proxyPet->IncreaseAbilityPoint(1);
										proxyPet->LevelUp();
										proxyPet->m_exp = 0;
									}
								}
							}
						}
						expNode->pc->m_petStashManager.UpdatePetData(petPlus);
					}
				}

				// ����ġ ���� : �������� ���� ���̵��ÿ� true�� ��ȯ�ȴ�
				if (expNode->pc->AddExpSP(nGiveExp, (int)nGiveSP, true, false, false, bArtifact))
				{
					// ��� ����Ʈ �޸� ����
					while (expList)
					{
						expNode = expList;
						expList = expList->next;
						expNode->pc->m_nExpDamage = 0;
						delete expNode;
					}
					return true;
				}

				// ���� ���� PC �ʱ�ȭ
				expNode->pc->m_nExpDamage = 0;
			} // --- ��Ƽ �ƴ� ��� ó�� ��
		} // --- ���� ��� ó�� ��

		expNode = expNode->next;
	}

	// ��� ����Ʈ �޸� ����
	while (expList)
	{
		expNode = expList;
		expList = expList->next;
		expNode->pc->m_nExpDamage = 0;
		delete expNode;
	}

	return false;
}

// ��Ƽ�� �鿡�� ����ġ SP �й�	(��Ƽ, NPC, �켱��, ��ü ������)
void DivisionExpSPParty(CParty* party, CNPC* npc, CPC* pPreferencePC, LONGLONG nTotalDamage)
{
	// ��Ƽ ���ʽ� ���̺� [%] : 1�κ��� ����
#if defined (LC_USA) || defined(LC_BILA)
	static const int countBonus[MAX_PARTY_MEMBER] = { 0, 30, 40, 50, 60, 70, 75, 80};
#else
	static const int countBonus[MAX_PARTY_MEMBER] = { 15, 30, 40, 50, 60, 70, 70, 70};
#endif // #if defined (LC_USA) || defined (LC_BILA)

	EXP_LIST*			expParty = NULL;		// ��Ƽ���� ����ġ ���� ����Ʈ
	EXP_LIST*			expPartyNode = NULL;	// ��Ƽ�� ���
	EXP_LIST*			expNode = NULL;			// ��� ����Ʈ ������
	EXP_LIST*			expNodeNext = NULL;		// ��� ����Ʈ ������

	LONGLONG			nPartyDamage = 0;		// ��Ƽ �� �����
	int					nParty = 0;				// ��� ��Ƽ�� ��
	int					nPartyLevelTotal = 0;	// ��Ƽ�� ���� ��
	int					nPartyLevelMax = 0;		// ��Ƽ�� �ִ� ����
	int					nPartyLevel = 0;		// ��Ƽ ��� ����
	LONGLONG			nExpParty = 0;			// ��Ƽ ����ġ
	LONGLONG			nSPParty = 0;			// ��Ƽ SP
	LONGLONG			nExpPenalty;			// ����ġ �г�Ƽ
	LONGLONG			nSPPenalty;				// SP �г�Ƽ
	int					i;						// ��Ƽ ������
	int					nHavePetCount=0;			// ��Ƽ���� ������ �ִ� ������ �� ��

	if (nTotalDamage < 1)
		return ;

	if (party == NULL)
		return ;

	if (npc == NULL)
		return ;

	// ��Ƽ �� ����� ���ϱ�
	// ��Ƽ�� �� ���ϱ�
	// ��Ƽ�� �� �ش��� ����Ʈ �����
	for (i = 0; i < MAX_PARTY_MEMBER; i++)
	{
		CPC* pMember = party->GetNearMember(npc, i);
		if (pMember == NULL)
			continue;
		if ( DEAD(pMember) )
			continue;
		// ��ü ������� �߰�
		nPartyDamage += pMember->m_nExpDamage;
		// ��� ����Ʈ�� �߰�
		expPartyNode = new EXP_LIST;
		expPartyNode->pc = pMember;
		expPartyNode->next = expParty;
		expParty = expPartyNode;
		nParty++;
		// ��Ƽ�� ������
		nPartyLevelTotal += pMember->m_level;
		// ��Ƽ�� �ִ� ����
		if (nPartyLevelMax < pMember->m_level)
			nPartyLevelMax = pMember->m_level;

		// ���� ������ ���� ������ ī��Ʈ ����
		if( pMember->GetAPet() )
			nHavePetCount++;
	}

	// ��Ƽ ����ġ �й� ����
	// ��Ƽ ����� ����
	// ��� ��Ƽ���� 1�� �̻�
	// ��Ƽ �������� 1 �̻�
	if (nPartyDamage > 0 && nParty > 0 && nPartyLevelTotal > 0)
	{
		int basic_party_plus_exp;
		int basic_party_plus_sp;

		// bw : 060817 : ��Ƽ �ý��� ���� �κ�, �ο��� ���ʽ��� �Ѹ��϶� 0�̵��� ��������
		/*
		if( nParty == 1 )
		{
			basic_party_plus_exp = 0;
			basic_party_plus_sp = 0;
		}
		else
		*/
		{
			basic_party_plus_exp = PARTY_PLUS_EXP;
			basic_party_plus_sp = PARTY_PLUS_SP;
		}

		// ��Ƽ ��� ����
		nPartyLevel = (nPartyLevelMax - (nPartyLevelTotal / nParty) > 5) ? (nPartyLevelMax - 5) : (nPartyLevelTotal / nParty);

		// �������� ���ϱ�
		int nLevelDiff = nPartyLevel - npc->m_level;

		// ��Ƽ ���� �г�Ƽ ���ϱ�
		nExpPenalty = GetExpLevelPenalty(nLevelDiff);
		nSPPenalty  = GetSPLevelPenalty(nLevelDiff);

		// ��Ƽ	���ʽ� ���ϱ�
		LONGLONG nExpBonus	= 100 + basic_party_plus_exp + countBonus[nParty - 1];
		LONGLONG nSPBonus	= 100 + basic_party_plus_sp + 5 * (nParty - 1);

		// ��Ƽ�� ���� ����ġ/SP ���ϱ�
#ifdef PREMIUM_CHAR
		LONGLONG nExpNPC;
		if(pPreferencePC != NULL)
		{
			nExpNPC = npc-> m_proto->getExpForPremiumChar(pPreferencePC->m_premiumChar.getPremiumType());
		}
		else
		{
			nExpNPC = npc-> m_proto->getExp();
		}
#else
		LONGLONG nExpNPC = npc-> m_proto->getExp();
#endif

		//nExpParty		= nExpNPC * nExpBonus * nExpPenalty * nPartyDamage / (100 * 100 * nTotalDamage);
		nExpParty		= (LONGLONG)( ((double)nExpNPC / 100) * ((double)nExpBonus / 100) * nExpPenalty * ((double)nPartyDamage / nTotalDamage));
		if (nExpNPC >= 0 && nExpParty <= 0)
			nExpParty	= nExpNPC * nExpBonus / 100 * nExpPenalty / 100 * nPartyDamage / nTotalDamage;

#ifdef PREMIUM_CHAR
		int tSkillPoint;
		if(pPreferencePC != NULL)
		{
			tSkillPoint = npc->m_proto->getSkillPointForPremiumChar(pPreferencePC->m_premiumChar.getPremiumType());
		}
		else
		{
			tSkillPoint = npc->m_proto->getSkillPoint();
		}
#else
		int tSkillPoint = npc->m_proto->getSkillPoint();
#endif
		nSPParty		= tSkillPoint * nSPBonus  * nSPPenalty  * nPartyDamage / (100 * 100 * nTotalDamage);
		if (tSkillPoint > 0 && nSPParty <= 0)
			nSPParty	= tSkillPoint  * nSPBonus  / 100 * nSPPenalty  / 100 * nPartyDamage / nTotalDamage;

		// ���� ����
#if defined (LC_USA) || defined(LC_BILA)
		if (nParty >= 2 )
		{
			nExpParty = nExpParty + (nExpParty * GetRandom(-10, 10) / 100);
			nSPParty  = nSPParty  + (nSPParty *  GetRandom(-10, 10) / 100);
		}
#else
		nExpParty = nExpParty + (nExpParty * GetRandom(-10, 10) / 100);
		nSPParty  = nSPParty  + (nSPParty *  GetRandom(-10, 10) / 100);
#endif // #if defined (LC_USA) || defined (LC_BILA)
	}
	else
	{
		// ����� ����Ʈ �ʱ�ȭ
		while (expParty)
		{
			expPartyNode = expParty;
			expParty = expParty->next;
			expPartyNode->pc->m_nExpDamage = 0;
			delete expPartyNode;
		}
		return;
	}

	// ��Ƽ������ ����ġ/SP ����
	LONGLONG nGiveExp = 0;		// ���޵� ����ġ
	LONGLONG nGiveSP  = 0;		// ���޵� SP
	LONGLONG nExpPartyForGive = 0;			// ���޿� ����� ��Ƽ ����ġ : nExpParty or nExpParty_CA
	expNodeNext = expParty;
	while ((expNode = expNodeNext))
	{
		expNodeNext = expNodeNext->next;

		nExpPartyForGive = nExpParty;

		// ��Ƽ Ÿ�Կ� ���� ����ġ
		switch (party->GetPartyType(MSG_DIVITYPE_EXP))
		{
		case MSG_PARTY_TYPE_RANDOM:			// �յ�
		case MSG_PARTY_TYPE_FIRSTGET:		// �Լ� �켱
			//   ( (��Ƽ����ġ 75%) * (����� ����) / (��Ƽ�ѷ���) )
			// + ( (��Ƽ����ġ 25%) / (��Ƽ����) )
			nGiveExp = (nExpPartyForGive * 75 * expNode->pc->m_level / nPartyLevelTotal)
					   + (nExpPartyForGive * 25 / nParty);
			// ���� ���� ���� : �й�%
			nGiveExp /= 100;
			if (nExpPartyForGive > 0 && nGiveExp <= 0)
			{
				nGiveExp = (nExpPartyForGive * 75 / 100 * expNode->pc->m_level / nPartyLevelTotal)
						   + (nExpPartyForGive * 25 / 100 / nParty);
			}
			break;

		default:
		case MSG_PARTY_TYPE_BATTLE:			// ������
			//   ( (��Ƽ ����ġ 70%) * (����� �����) / (��Ƽ �����) )
			// + ( (��Ƽ ����ġ 30%) * (����� ����) / (��Ƽ ��ü����) )
			nGiveExp = (nExpPartyForGive * 70 * expNode->pc->m_nExpDamage / nPartyDamage)
					   + (nExpPartyForGive * 30 * expNode->pc->m_level / nPartyLevelTotal);
			// ���� ���� ���� : �й�%
			nGiveExp /= 100;
			if (nExpPartyForGive > 0 && nGiveExp <= 0)
			{
				nGiveExp = (nExpPartyForGive * 70 / 100 * expNode->pc->m_nExpDamage / nPartyDamage)
						   + (nExpPartyForGive * 30 / 100 * expNode->pc->m_level / nPartyLevelTotal);
			}
			break;
		}

		// ��Ƽ Ÿ�Կ� ���� SP
		switch (party->GetPartyType(MSG_DIVITYPE_SP))
		{
		case MSG_PARTY_TYPE_RANDOM:			// �յ�
		case MSG_PARTY_TYPE_BATTLE:			// ������
			// (��Ƽ SP) / (��Ƽ����)
			nGiveSP = nSPParty / nParty;
			break;

		default:
		case MSG_PARTY_TYPE_FIRSTGET:		// �Լ��켱
			// ĳ���� ���� �г�Ƽ
			nSPPenalty = GetSPLevelPenalty(expNode->pc->m_level - npc->m_level);

			//   ( (��ƼSP 20%) / (��Ƽ����) )
			// + ( (��ƼSP 60%) * (����ڴ����) / (��Ƽ�����) )
			// + ( (��ƼSP 20%) * (����ڷ���) / (��Ƽ�ѷ���) )
			nGiveSP = (nSPParty * 20 * nSPPenalty / nParty)
					  + (nSPParty * 60 * nSPPenalty * expNode->pc->m_nExpDamage / nPartyDamage)
					  + (nSPParty * 20 * nSPPenalty * expNode->pc->m_level / nPartyLevelTotal);
			// ���� ���� ���� : �й�%, �г�Ƽ
			nGiveSP /= 10000;
			if (nSPParty > 0 && nGiveSP <= 0)
			{
				nGiveSP = (nSPParty * 20 / 100 * nSPPenalty / 100 / nParty)
						  + (nSPParty * 60 / 100 * nSPPenalty / 100 * expNode->pc->m_nExpDamage / nPartyDamage)
						  + (nSPParty * 20 / 100 * nSPPenalty / 100 * expNode->pc->m_level / nPartyLevelTotal);
			}
			break;
		}

		nGiveSP += GetTeachSPBonus(expNode->pc, nGiveSP );

		// ��Ƽ ���� ���� �г�Ƽ : �ְ������� 15�� �� ������ 5%�� ����
		if (nPartyLevelMax - 15 > expNode->pc->m_level)
		{
			nGiveExp /= 20;
			nGiveSP  /= 20;
		}

		// ����
		if (nGiveExp < 1)	nGiveExp = 1;
		if (nGiveSP  < 1)	nGiveSP  = 1;

		// SP�� MAX_SKILLPOINT�� ���� �� ����
		if (nGiveSP > MAX_SKILLPOINT) nGiveSP = MAX_SKILLPOINT;

		// 9�� �̺�Ʈ : ����ڰ� �켱�����̸�
		bool bArtifact = false;

		if (expNode->pc == pPreferencePC)
		{
			// ����ġ 4��
			if (expNode->pc->m_assist.m_avAddition.hcSepExp)
			{
				expNode->pc->m_assist.CureByItemIndex(882);	// ����ġ
				nGiveExp = nGiveExp * 4;
				CNetMsg::SP rmsg(new CNetMsg);
				EventErrorMsg(rmsg, MSG_EVENT_ERROR_SEPTEMBER_EXP);
				SEND_Q(rmsg, expNode->pc->m_desc);

				bArtifact = true;
			}
			else if (expNode->pc->m_assist.m_avAddition.hcSepExp_Cash)	// ����ġ 4��
			{
				expNode->pc->m_assist.CureByItemIndex(6094);	// ����ġ
				nGiveExp = nGiveExp * 4;
				CNetMsg::SP rmsg(new CNetMsg);
				EventErrorMsg(rmsg, MSG_EVENT_ERROR_SEPTEMBER_EXP);
				SEND_Q(rmsg, expNode->pc->m_desc);

				bArtifact = true;
			}
			else if( expNode->pc->m_assist.m_avAddition.hcRandomExpUp > 0  && GetRandom(0,100) <= expNode->pc->m_assist.m_avAddition.hcRandomExpUp )
			{
				nGiveExp = nGiveExp * 4;

				CNetMsg::SP rmsg(new CNetMsg);
				EffectEtcMsg(rmsg, expNode->pc, MSG_EFFECT_ETC_RANDOM_EXP);
				expNode->pc->m_pArea->SendToCell(rmsg, expNode->pc, true);

				bArtifact = true;
			}

			// 9�� �̺�Ʈ : SP 4��
			if (expNode->pc->m_assist.m_avAddition.hcSepSP)
			{
				expNode->pc->m_assist.CureByItemIndex(883);	// ���õ�
				nGiveSP = nGiveSP * 4;
				CNetMsg::SP rmsg(new CNetMsg);
				EventErrorMsg(rmsg, MSG_EVENT_ERROR_SEPTEMBER_SP);
				SEND_Q(rmsg, expNode->pc->m_desc);
			}
			// ���� SP 4��
			else if (expNode->pc->m_assist.m_avAddition.hcSepSP_Cash)
			{
				expNode->pc->m_assist.CureByItemIndex(6095);	// ���õ�
				nGiveSP = nGiveSP * 4;
				CNetMsg::SP rmsg(new CNetMsg);
				EventErrorMsg(rmsg, MSG_EVENT_ERROR_SEPTEMBER_SP);
				SEND_Q(rmsg, expNode->pc->m_desc);
			}

			else if( expNode->pc->m_assist.m_avAddition.hcRandomSpUp > 0  && GetRandom(0,100) <= expNode->pc->m_assist.m_avAddition.hcRandomSpUp )
			{
				nGiveSP = nGiveSP * 4;

				CNetMsg::SP rmsg(new CNetMsg);
				EffectEtcMsg(rmsg, expNode->pc, MSG_EFFECT_ETC_RANDOM_SP);
				expNode->pc->m_pArea->SendToCell(rmsg, expNode->pc, true);
			}
		}

#ifdef	NEW_ACCERY_ADD
		if( expNode->pc->m_AddProb )
		{
			nGiveExp = nGiveExp + ( nGiveExp * expNode->pc->m_AddProb / 100 );
			nGiveSP = nGiveSP + ( nGiveSP * expNode->pc->m_AddProb / 100 );
		}
#endif // NEW_ACCERY_ADD

		int GuildExp = 0;
		if( expNode->pc->m_guildInfo && expNode->pc->m_guildInfo->guild() )
		{
			CGuild* guild = expNode->pc->m_guildInfo->guild();
			if( expNode->pc->m_guildInfo->GetcontributeExp() != 0 )
			{
				GuildExp = nGiveExp * expNode->pc->m_guildInfo->GetcontributeExp() / 100;
				expNode->pc->m_GuildExp += GuildExp;
				nGiveExp = nGiveExp - GuildExp;
				int GP = 0;
				if( expNode->pc->m_GuildExp >= 10000 )
				{
					GP = expNode->pc->m_GuildExp / 10000;
					expNode->pc->m_GuildExp = expNode->pc->m_GuildExp % 10000;
					guild->AddGuildPoint( GP );
					expNode->pc->m_guildInfo->AddCumulatePoint( GP );

					{
						CNetMsg::SP rmsg(new CNetMsg);
						HelperNewGuildPointUpdate( rmsg, expNode->pc->m_index, guild->index(), GP );
						SEND_Q(rmsg, gserver->m_helper);
					}

					{
						CNetMsg::SP rmsg(new CNetMsg);
						HelperSaveGuildMemberPointMsg( rmsg, guild->index(), expNode->pc->m_index, GP );
						SEND_Q( rmsg, gserver->m_helper );
					}
				}

			}
		}

		// EP = ( NPC����  * ��Ƽ���� ) + ( NPC���� * ������ / NPC HP ) / 2
		// ������ ����

		int	recalcDamage  = (int) expNode->pc->m_nExpDamage ;
		if( recalcDamage > npc->m_maxHP )
			recalcDamage = npc->m_maxHP;

#ifdef PREMIUM_CHAR
		LONGLONG nGetEnergy ;
		if(pPreferencePC != NULL)
		{
			nGetEnergy = (LONGLONG)npc->m_proto->getSkillPointForPremiumChar(pPreferencePC->m_premiumChar.getPremiumType()) * (LONGLONG)recalcDamage / (LONGLONG)npc->m_maxHP;
		}
		else
		{
			nGetEnergy = (LONGLONG)npc->m_proto->getSkillPoint() * (LONGLONG)recalcDamage / (LONGLONG)npc->m_maxHP;
		}
#else
		LONGLONG nGetEnergy = (LONGLONG)npc->m_proto->getSkillPoint() * (LONGLONG)recalcDamage / (LONGLONG)npc->m_maxHP;
#endif

		int ep = ( npc->m_proto->m_level * nParty ) + ( (int)nGetEnergy ) / 2 ;
		if(ABS(npc->m_level - expNode->pc->m_level) < 7)
		{
			expNode->pc->AddEP( ep );
		}
		expNode->pc->CalcStatus(true);

		//�����Ƽ �нú� ����
		int member_count = party->getGuildMemberCountInParty(expNode->pc);
		if(member_count >= 2)
		{
			if(expNode->pc->m_avPassiveAddition.guild_party_exp > 0)
			{
				nGiveExp += (expNode->pc->m_avPassiveAddition.guild_party_exp * member_count);
			}
			if(expNode->pc->m_avPassiveRate.guild_party_exp > 0)
			{
				nGiveExp += nGiveExp * (expNode->pc->m_avPassiveRate.guild_party_exp - 100) * member_count / SKILL_RATE_UNIT;
			}

			if(expNode->pc->m_avPassiveAddition.guild_party_sp > 0)
			{
				nGiveSP += expNode->pc->m_avPassiveAddition.guild_party_sp * member_count;
			}
			if(expNode->pc->m_avPassiveRate.guild_party_sp > 0)
			{
				nGiveSP += nGiveSP * (expNode->pc->m_avPassiveRate.guild_party_sp - 100) * member_count / SKILL_RATE_UNIT;
			}
		}

		
		// ����ġ ����
		expNode->pc->AddExpSP(nGiveExp, (int)nGiveSP, true, false, false, bArtifact);
		if(expNode->pc->m_party != NULL)
			expNode->pc->m_party->calcGuildSkillExp(expNode->pc, nGiveExp);

		CAPet* apet = expNode->pc->GetAPet();
		if( apet )
		{
			if( nHavePetCount == 0 )
				nHavePetCount = 1;

			LONGLONG petexp = (npc->m_level) / 5 / nHavePetCount * ( npc->m_proto->CheckFlag(NPC_MBOSS|NPC_BOSS)? 4:1 );
			apet->AddExpSP( petexp , 0 );

			if( expNode->pc->m_petStashManager.getProxyPetItemvIndex() > -1 )
			{
				int petVIndex = expNode->pc->m_petStashManager.getProxyPetItemvIndex();
				int petPlus = expNode->pc->m_petStashManager.getProxyPetItemPlus();
				if(expNode->pc->GetAPet(petPlus))
				{
					CAPet* proxyPet = expNode->pc->m_petStashManager.getProxyAPet();
					if(proxyPet != NULL)
					{
						petexp /= 10;
						proxyPet->AddExpSP(petexp, 0);
					}
				}
				else if(expNode->pc->GetPet(petPlus))
				{
					CPet* proxyPet = expNode->pc->m_petStashManager.getProxyPet();
					if(proxyPet != NULL)
					{
						if(proxyPet->m_level < PET_MAX_LEVEL)
						{
							petexp /= 10;
							proxyPet->m_exp += petexp;
							if(proxyPet->m_exp > proxyPet->GetNeedExp())
							{
								proxyPet->m_level++;
								proxyPet->IncreaseAbilityPoint(1);
								proxyPet->LevelUp();
								proxyPet->m_exp = 0;
							}
						}
					}
				}
				expNode->pc->m_petStashManager.UpdatePetData(petPlus);
			}
		}

		// ���� ���� PC �ʱ�ȭ
		expNode->pc->m_nExpDamage = 0;
	} // -- ��Ƽ�� �й� ��ƾ ��

	// ����� ����Ʈ �ʱ�ȭ
	while (expParty)
	{
		expPartyNode = expParty;
		int guild_skill_exp = 0;

		if(expPartyNode->pc->m_party != NULL)
		{
			guild_skill_exp = expPartyNode->pc->m_party->getGuildSkillExp(expPartyNode->pc);
		}

		if(guild_skill_exp != 0)
		{
			expPartyNode->pc->AddExpSP_Direct(guild_skill_exp, 0);

			{
				// Exp, SP �޽��� ����
				CNetMsg::SP rmsg(new CNetMsg);
				ExpSPMsg(rmsg, guild_skill_exp, 0);
				SEND_Q(rmsg, expParty->pc->m_desc);
			}
		}

		expParty = expParty->next;
		expPartyNode->pc->m_nExpDamage = 0;
		delete expPartyNode;
	}

	party->m_guild_skill_exp.clear();
}

// ������� �鿡�� ����ġ SP �й�	(��Ƽ, NPC, �켱��, ��ü ������)
void DivisionExpSPExped(CExpedition* Exped, CNPC* npc, CPC* pPreferencePC, LONGLONG nTotalDamage)
{
	// ��Ƽ ���ʽ� ���̺� [%] : 1�κ��� ����
#if defined (LC_USA) || defined(LC_BILA)
	static const int countBonus[MAX_PARTY_MEMBER] = { 0, 30, 40, 50, 60, 70, 75, 80};
#else
	static const int countBonus[MAX_PARTY_MEMBER] = { 15, 30, 40, 50, 60, 70, 70, 70};
#endif

	EXP_LIST*			expExped = NULL;		// �������� ����ġ ���� ����Ʈ
	EXP_LIST*			expExpedNode = NULL;	// ������� ���
	EXP_LIST*			expNode = NULL;			// ��� ����Ʈ ������
	EXP_LIST*			expNodeNext = NULL;		// ��� ����Ʈ ������

	LONGLONG			nExpedDamage = 0;		// ������ �� �����
	int					nExped = 0;				// ��� ������� ��
	int					nExpedLevelTotal = 0;	// ������� ���� ��
	int					nExpedLevelMax = 0;		// ������� �ִ� ����
	int					nExpedLevel = 0;		// ������ ��� ����
	LONGLONG			nExpExped = 0;			// ������ ����ġ
	LONGLONG			nSPExped = 0;			// ������ SP
	LONGLONG			nExpPenalty;			// ����ġ �г�Ƽ
	LONGLONG			nSPPenalty;				// SP �г�Ƽ
	int					i;						// ������ ������
	int					j;						// ������ ������
	int					nHavePetCount=0;			// ��������� ������ �ִ� ������ �� ��

	if (nTotalDamage < 1)
		return ;

	if (Exped == NULL)
		return ;

	if (npc == NULL)
		return ;

	// ������ �� ����� ���ϱ�
	// ������� �� ���ϱ�
	// ������� �� �ش��� ����Ʈ �����
	for (i = 0; i < MAX_EXPED_GROUP; i++)
	{
		for (j = 0; j < MAX_EXPED_GMEMBER; j++)
		{
			CPC* pMember = Exped->GetNearMember(npc, i,j);
			if (pMember == NULL)
				continue;
			if ( DEAD(pMember) )
				continue;
			// ��ü ������� �߰�
			nExpedDamage += pMember->m_nExpDamage;
			// ��� ����Ʈ�� �߰�
			expExpedNode = new EXP_LIST;
			expExpedNode->pc = pMember;
			expExpedNode->next = expExped;
			expExped = expExpedNode;
			nExped++;
			// ������� ������
			nExpedLevelTotal += pMember->m_level;
			// ������� �ִ� ����
			if (nExpedLevelMax < pMember->m_level)
				nExpedLevelMax = pMember->m_level;

			// ���� ������ ���� ������ ī��Ʈ ����
			if( pMember->GetAPet() )
				nHavePetCount++;
		}
	}

	// ������ ����ġ �й� ����
	// ������ ����� ����
	// ������ ��Ƽ���� 1�� �̻�
	// ������ �������� 1 �̻�
	if (nExpedDamage > 0 && nExped > 0 && nExpedLevelTotal > 0)
	{
		int basic_Exped_plus_exp;
		int basic_Exped_plus_sp;

		// bw : 060817 : ������ �ý��� ���� �κ�, �ο��� ���ʽ��� �Ѹ��϶� 0�̵��� ��������
		/*
		if( nParty == 1 )
		{
			basic_party_plus_exp = 0;
			basic_party_plus_sp = 0;
		}
		else
		*/
		{
			basic_Exped_plus_exp = PARTY_PLUS_EXP;
			basic_Exped_plus_sp = PARTY_PLUS_SP;
		}

		// ������ ��� ����
		nExpedLevel = (nExpedLevelMax - (nExpedLevelTotal / nExped) > 5) ? (nExpedLevelMax - 5) : (nExpedLevelTotal / nExped);

		// �������� ���ϱ�
		int nLevelDiff = nExpedLevel - npc->m_level;

		// ������ ���� �г�Ƽ ���ϱ�
		nExpPenalty = GetExpLevelPenalty(nLevelDiff);
		nSPPenalty  = GetSPLevelPenalty(nLevelDiff);

		// ������ ���ʽ� ���ϱ�
		// 8�� ������ ��Ƽ�� ���ʽ� ���� �� �̻��̸� 8��� ����
		int tempExpBonus = 0;
		if(nExped > 8)
			tempExpBonus = countBonus[7];
		else
			tempExpBonus = countBonus[nExped - 1];

		LONGLONG nExpBonus	= 100 + basic_Exped_plus_exp + tempExpBonus;

		int tempSPBonus = 0;
		if(nExped > 8)
			tempSPBonus = 35;
		else
			tempSPBonus = 5 * (nExped - 1);

		LONGLONG nSPBonus	= 100 + basic_Exped_plus_sp + tempSPBonus;

		// �����밡 ���� ����ġ/SP ���ϱ�
#ifdef PREMIUM_CHAR
		LONGLONG nExpNPC;
		if(pPreferencePC != NULL)
		{
			nExpNPC = npc-> m_proto->getExpForPremiumChar(pPreferencePC->m_premiumChar.getPremiumType());
		}
		else
		{
			nExpNPC = npc-> m_proto->getExp();
		}
#else
		LONGLONG nExpNPC = npc-> m_proto->getExp();
#endif

		//nExpExped		= nExpNPC * nExpBonus * nExpPenalty * nExpedDamage / (100 * 100 * nTotalDamage);
		nExpExped = LONGLONG(nExpNPC * ( (double)nExpBonus / 100)  * ( (double)nExpPenalty / 100 ) * ( (double)nExpedDamage / (double)nTotalDamage ) );
		if (nExpNPC >= 0 && nExpExped <= 0)
			nExpExped	= nExpNPC * nSPBonus * nExpPenalty / 100 * nExpedDamage / nTotalDamage;

// [2010-10-18 derek] ��ĭ ������� ���� ����ġ�� 1�� ���ͼ� Ȯ�� ���
// �ؿ� �������� ��� �־ ���� ����. �� �ɾ�����? �����Ѱͱ����� ����̳��µ� �� �ߴ��� ��� �ʳ�.
// ���� ��û�̿�����?
#ifdef PREMIUM_CHAR
		int tSkillPoint;
		if(pPreferencePC != NULL)
		{
			tSkillPoint = npc->m_proto->getSkillPointForPremiumChar(pPreferencePC->m_premiumChar.getPremiumType());
		}
		else
		{
			tSkillPoint = npc->m_proto->getSkillPoint();
		}
#else
		int tSkillPoint = npc->m_proto->getSkillPoint();
#endif
		nSPExped		=  tSkillPoint * nSPBonus  * nSPPenalty  * nExpedDamage / (100 * 100 * nTotalDamage);
		if (tSkillPoint > 0 && nSPExped <= 0)
			nSPExped	= tSkillPoint * nSPBonus  * nSPPenalty  / 100 * nExpedDamage / nTotalDamage;

		// ���� ����
#if defined (LC_USA) || defined(LC_BILA)
		if (nExped >= 2 )
		{
			nExpExped = nExpExped + (nExpExped * GetRandom(-10, 10) / 100);
			nSPExped  = nSPExped  + (nSPExped *  GetRandom(-10, 10) / 100);
		}
#else
		nExpExped = nExpExped + (nExpExped * GetRandom(-10, 10) / 100);
		nSPExped  = nSPExped  + (nSPExped *  GetRandom(-10, 10) / 100);
#endif // #if defined (LC_USA) || defined (LC_BILA)
	}
	else
	{
		// ����� ����Ʈ �ʱ�ȭ
		while (expExped)
		{
			expExpedNode = expExped;
			expExped = expExped->next;
			expExpedNode->pc->m_nExpDamage = 0;
			delete expExpedNode;
		}
		return;
	}

	// �����뿡�� ����ġ/SP ����
	LONGLONG nGiveExp = 0;		// ���޵� ����ġ
	LONGLONG nGiveSP  = 0;		// ���޵� SP
	LONGLONG nExpExpedForGive = 0;			// ���޿� ����� ��Ƽ ����ġ : nExpParty or nExpParty_CA
	expNodeNext = expExped;
	while ((expNode = expNodeNext))
	{
		expNodeNext = expNodeNext->next;
		nExpExpedForGive = nExpExped;

		// ������ Ÿ�Կ� ���� ����ġ
		switch (Exped->GetExpedType(MSG_DIVITYPE_EXP))
		{
		case MSG_EXPED_TYPE_RANDOM:			// �յ�
		case MSG_EXPED_TYPE_FIRSTGET:		// �Լ� �켱
			//   ( (���������ġ 75%) * (����� ����) / (��Ƽ�ѷ���) )
			// + ( (���������ġ 25%) / (���������) )
			nGiveExp = (nExpExpedForGive * 75 * expNode->pc->m_level / nExpedLevelTotal)
					   + (nExpExpedForGive * 25 / nExped);
			// ���� ���� ���� : �й�%
			nGiveExp /= 100;
			if (nExpExpedForGive > 0 && nGiveExp <= 0)
			{
				nGiveExp = (nExpExpedForGive * 75 / 100 * expNode->pc->m_level / nExpedLevelTotal)
						   + (nExpExpedForGive * 25 / 100 / nExped);
			}
			break;

		default:
		case MSG_EXPED_TYPE_BATTLE:			// ������
			//   ( (������ ����ġ 70%) * (����� �����) / (������ �����) )
			// + ( (������ ����ġ 30%) * (����� ����) / (������ ��ü����) )
			nGiveExp = (nExpExpedForGive * 70 * expNode->pc->m_nExpDamage / nExpedDamage)
					   + (nExpExpedForGive * 30 * expNode->pc->m_level / nExpedLevelTotal);
			// ���� ���� ���� : �й�%
			nGiveExp /= 100;
			if (nExpExpedForGive > 0 && nGiveExp <= 0)
			{
				nGiveExp = (nExpExpedForGive * 70 / 100 * expNode->pc->m_nExpDamage / nExpedDamage)
						   + (nExpExpedForGive * 30 / 100 * expNode->pc->m_level / nExpedLevelTotal);
			}
			break;
		}

		// ������ Ÿ�Կ� ���� SP
		switch (Exped->GetExpedType(MSG_DIVITYPE_SP))
		{
		case MSG_EXPED_TYPE_RANDOM:			// �յ�
		case MSG_EXPED_TYPE_BATTLE:			// ������
			// (������ SP) / (���������)
			nGiveSP = nSPExped / nExped;
			break;

		default:
		case MSG_EXPED_TYPE_FIRSTGET:		// �Լ��켱
			// ĳ���� ���� �г�Ƽ
			nSPPenalty = GetSPLevelPenalty(expNode->pc->m_level - npc->m_level);

			//   ( (������SP 20%) / (���������) )
			// + ( (������SP 60%) * (����ڴ����) / (����������) )
			// + ( (������SP 20%) * (����ڷ���) / (�������ѷ���) )
			nGiveSP = (nSPExped * 20 * nSPPenalty / nExped)
					  + (nSPExped * 60 * nSPPenalty * expNode->pc->m_nExpDamage / nExpedDamage)
					  + (nSPExped * 20 * nSPPenalty * expNode->pc->m_level / nExpedLevelTotal);
			// ���� ���� ���� : �й�%, �г�Ƽ
			nGiveSP /= 10000;
			if (nSPExped > 0 && nGiveSP <= 0)
			{
				nGiveSP = (nSPExped * 20 / 100 * nSPPenalty / 100 / nExped)
						  + (nSPExped * 60 / 100 * nSPPenalty / 100 * expNode->pc->m_nExpDamage / nExpedDamage)
						  + (nSPExped * 20 / 100 * nSPPenalty / 100 * expNode->pc->m_level / nExpedLevelTotal);
			}
			break;
		}
		nGiveSP += GetTeachSPBonus(expNode->pc, nGiveSP );

		// ������ ���� ���� �г�Ƽ : �ְ������� 15�� �� ������ 5%�� ����
		if (nExpedLevelMax - 15 > expNode->pc->m_level)
		{
			nGiveExp /= 20;
			nGiveSP  /= 20;
		}

		// ����
		if (nGiveExp < 1)	nGiveExp = 1;
		if (nGiveSP  < 1)	nGiveSP  = 1;

		// SP�� MAX_SKILLPOINT�� ���� �� ����
		if (nGiveSP > MAX_SKILLPOINT) nGiveSP = MAX_SKILLPOINT;

		// 9�� �̺�Ʈ : ����ڰ� �켱�����̸�
		bool bArtifact = false;
		if (expNode->pc == pPreferencePC)
		{
			// ����ġ 4��
			if (expNode->pc->m_assist.m_avAddition.hcSepExp)
			{
				expNode->pc->m_assist.CureByItemIndex(882);	// ����ġ
				nGiveExp = nGiveExp * 4;
				CNetMsg::SP rmsg(new CNetMsg);
				EventErrorMsg(rmsg, MSG_EVENT_ERROR_SEPTEMBER_EXP);
				SEND_Q(rmsg, expNode->pc->m_desc);

				bArtifact = true;
			}
			// ���� ����ġ 4��
			else if (expNode->pc->m_assist.m_avAddition.hcSepExp_Cash)
			{
				expNode->pc->m_assist.CureByItemIndex(6094);	// ����ġ
				nGiveExp = nGiveExp * 4;
				CNetMsg::SP rmsg(new CNetMsg);
				EventErrorMsg(rmsg, MSG_EVENT_ERROR_SEPTEMBER_EXP);
				SEND_Q(rmsg, expNode->pc->m_desc);

				bArtifact = true;
			}
			else if( expNode->pc->m_assist.m_avAddition.hcRandomExpUp > 0  && GetRandom(0,100) <= expNode->pc->m_assist.m_avAddition.hcRandomExpUp )
			{
				nGiveExp = nGiveExp * 4;

				CNetMsg::SP rmsg(new CNetMsg);
				EffectEtcMsg(rmsg, expNode->pc, MSG_EFFECT_ETC_RANDOM_EXP);
				expNode->pc->m_pArea->SendToCell(rmsg, expNode->pc, true);

				bArtifact = true;
			}

			// 9�� �̺�Ʈ : SP 4��
			if (expNode->pc->m_assist.m_avAddition.hcSepSP)
			{
				expNode->pc->m_assist.CureByItemIndex(883);	// ���õ�
				nGiveSP = nGiveSP * 4;
				CNetMsg::SP rmsg(new CNetMsg);
				EventErrorMsg(rmsg, MSG_EVENT_ERROR_SEPTEMBER_SP);
				SEND_Q(rmsg, expNode->pc->m_desc);
			}
			// ���� SP 4��
			else if (expNode->pc->m_assist.m_avAddition.hcSepSP_Cash)
			{
				expNode->pc->m_assist.CureByItemIndex(6095);	// ���õ�
				nGiveSP = nGiveSP * 4;
				CNetMsg::SP rmsg(new CNetMsg);
				EventErrorMsg(rmsg, MSG_EVENT_ERROR_SEPTEMBER_SP);
				SEND_Q(rmsg, expNode->pc->m_desc);
			}
			else if( expNode->pc->m_assist.m_avAddition.hcRandomSpUp > 0  && GetRandom(0,100) <= expNode->pc->m_assist.m_avAddition.hcRandomSpUp )
			{
				nGiveSP = nGiveSP * 4;

				CNetMsg::SP rmsg(new CNetMsg);
				EffectEtcMsg(rmsg, expNode->pc, MSG_EFFECT_ETC_RANDOM_SP);
				expNode->pc->m_pArea->SendToCell(rmsg, expNode->pc, true);
			}
		}

#ifdef	NEW_ACCERY_ADD
		if( expNode->pc->m_AddProb )
		{
			nGiveExp = nGiveExp + ( nGiveExp * expNode->pc->m_AddProb / 100 );
			nGiveSP = nGiveSP + ( nGiveSP * expNode->pc->m_AddProb / 100 );
		}
#endif // NEW_ACCERY_ADD

		int GuildExp = 0;
		if( expNode->pc->m_guildInfo && expNode->pc->m_guildInfo->guild() )
		{
			CGuild* guild = expNode->pc->m_guildInfo->guild();
			if( expNode->pc->m_guildInfo->GetcontributeExp() != 0 )
			{
				GuildExp = nGiveExp * expNode->pc->m_guildInfo->GetcontributeExp() / 100;
				expNode->pc->m_GuildExp += GuildExp;
				nGiveExp = nGiveExp - GuildExp;
				int GP = 0;
				if( expNode->pc->m_GuildExp >= 10000 )
				{
					GP = expNode->pc->m_GuildExp / 10000;
					expNode->pc->m_GuildExp = expNode->pc->m_GuildExp % 10000;
					guild->AddGuildPoint( GP );
					expNode->pc->m_guildInfo->AddCumulatePoint( GP );

					{
						CNetMsg::SP rmsg(new CNetMsg);
						HelperNewGuildPointUpdate( rmsg, expNode->pc->m_index, guild->index(), GP );
						SEND_Q(rmsg, gserver->m_helper);
					}

					{
						CNetMsg::SP rmsg(new CNetMsg);
						HelperSaveGuildMemberPointMsg( rmsg, guild->index(), expNode->pc->m_index, GP );
						SEND_Q( rmsg, gserver->m_helper );
					}
				}

			}
		}

		// EP = ( NPC����  * �������ο� ) + ( NPC���� * ������ / NPC HP ) / 2
		// ������ ����
		int	recalcDamage  = (int) expNode->pc->m_nExpDamage ;
		if( recalcDamage > npc->m_maxHP )
			recalcDamage = npc->m_maxHP;

#ifdef PREMIUM_CHAR
		LONGLONG nGetEnergy;
		if(pPreferencePC != NULL)
		{
			nGetEnergy = (LONGLONG)npc->m_proto->getSkillPointForPremiumChar(pPreferencePC->m_premiumChar.getPremiumType()) * (LONGLONG)recalcDamage / (LONGLONG)npc->m_maxHP;
		}
		else
		{
			nGetEnergy = (LONGLONG)npc->m_proto->getSkillPoint() * (LONGLONG)recalcDamage / (LONGLONG)npc->m_maxHP;
		}
#else
		LONGLONG nGetEnergy = (LONGLONG)npc->m_proto->getSkillPoint() * (LONGLONG)recalcDamage / (LONGLONG)npc->m_maxHP;
#endif
		int ep = ( npc->m_proto->m_level * nExped ) + ( (int)nGetEnergy ) / 2 ;
		if(ABS(npc->m_level - expNode->pc->m_level) < 7)
		{
			expNode->pc->AddEP( ep );
		}
		expNode->pc->CalcStatus(true);

		// ����ġ ����
		expNode->pc->AddExpSP(nGiveExp, (int)nGiveSP, true, false, false, bArtifact);

		CAPet* apet = expNode->pc->GetAPet();
		if( apet )
		{
			if( nHavePetCount == 0 )
				nHavePetCount = 1;

			LONGLONG petexp = (npc->m_level) / 5 / nHavePetCount * ( npc->m_proto->CheckFlag(NPC_MBOSS|NPC_BOSS)? 4:1 );
			apet->AddExpSP( petexp , 0 );

			if( expNode->pc->m_petStashManager.getProxyPetItemvIndex() > -1 )
			{
				int petVIndex = expNode->pc->m_petStashManager.getProxyPetItemvIndex();
				int petPlus = expNode->pc->m_petStashManager.getProxyPetItemPlus();
				if(expNode->pc->GetAPet(petPlus))
				{
					CAPet* proxyPet = expNode->pc->m_petStashManager.getProxyAPet();
					if(proxyPet != NULL)
					{
						petexp /= 10;
						proxyPet->AddExpSP(petexp, 0);
					}
				}
				else if(expNode->pc->GetPet(petPlus))
				{
					CPet* proxyPet = expNode->pc->m_petStashManager.getProxyPet();
					if(proxyPet != NULL)
					{
						if(proxyPet->m_level < PET_MAX_LEVEL)
						{
							petexp /= 10;
							proxyPet->m_exp += petexp;
							if(proxyPet->m_exp > proxyPet->GetNeedExp())
							{
								proxyPet->m_level++;
								proxyPet->IncreaseAbilityPoint(1);
								proxyPet->LevelUp();
								proxyPet->m_exp = 0;
							}
						}
					}
				}
				expNode->pc->m_petStashManager.UpdatePetData(petPlus);
			}
		}

		// ���� ���� PC �ʱ�ȭ
		expNode->pc->m_nExpDamage = 0;
	} // -- ������� �й� ��ƾ ��

	// ����� ����Ʈ �ʱ�ȭ
	while (expExped)
	{
		expExpedNode = expExped;
		expExped = expExped->next;
		expExpedNode->pc->m_nExpDamage = 0;
		delete expExpedNode;
	}
}

int GetExpLevelPenalty(int nLevelDiff)
{
	int nExpPenalty = 100;
	if (nLevelDiff > 0)
	{
		// ����ġ �г�Ƽ ó�� : �ִ� 6����
		if (nLevelDiff > 6)
			nExpPenalty = levelPenalty[5];
		else
			nExpPenalty = levelPenalty[nLevelDiff - 1];
	}
	return nExpPenalty;
}

int GetSPLevelPenalty(int nLevelDiff)
{
	int nSPPenalty = 100;
	if (nLevelDiff > 0)
	{
		// SP �г�Ƽ ó�� : MAX_PENALTY_SP_LEVEL���� ������ ������ 1
		if (nLevelDiff > MAX_PENALTY_SP_LEVEL)
			nSPPenalty = 0;
		else
			nSPPenalty = 100 - (nLevelDiff * DOWN_LEVEL_SP);
	}

	return nSPPenalty;
}

LONGLONG GetTeachSPBonus(CPC *ch, LONGLONG sp)
{
	if( ch->m_teachType == MSG_TEACH_STUDENT_TYPE )
	{
		if(ch->m_teachIdx[0] != -1 ) // �İ����� ������ �־�� �Ѵ� ���ʽ� ������ ���� �� �ֶ�.
		{
			CPC * tpc = PCManager::instance()->getPlayerByCharIndex(ch->m_teachIdx[0]);
			if( tpc != NULL)
			{
				// �İ��ο��� ���ʽ� SP 2% ����
				LONGLONG tpcGiveSP = sp * TEACH_SP_TEACHER / 100;
				tpc->AddExpSP(0, (int)tpcGiveSP, false);

				// �߽������� ���ʽ� sp 5% ����
				return (sp * TEACH_SP_STUDENT / 100);
			}
		}
	}
	return 0;
}
