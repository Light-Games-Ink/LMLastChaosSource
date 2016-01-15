#include "stdhdrs.h"

#include "Server.h"
#include "Log.h"
#include "Character.h"
#include "CmdMsg.h"
#include "doFunc.h"
#include "Battle.h"
#include "../ShareLib/packetType/ptype_old_do_exapet.h"


void do_ExApetSkillLean( CPC* ch, CNetMsg::SP& msg );
void do_ExApetSkillListReq( CPC* ch, CNetMsg::SP& msg);
void do_ExApetRebirth( CPC* ch, CNetMsg::SP& msg);
void do_ExApetItemWear( CPC* ch, CNetMsg::SP& msg);
void do_ExApetStatUp( CPC* ch, CNetMsg::SP& msg);
void do_ExApetInfo( CPC* ch, CNetMsg::SP& msg );

void do_ExApetAIListReq(CPC* ch, CNetMsg::SP& msg);
void do_ExApetAIOnOffReq(CPC* ch, CNetMsg::SP& msg);
void do_ExApetAIUpdateReq(CPC* ch, CNetMsg::SP& msg);

void do_ExApetEvolution(CPC* ch, CNetMsg::SP& msg);

void do_ExApetMount( CPC* ch, CNetMsg::SP& msg );
void do_ExApetExpInfo( CPC* ch, CNetMsg::SP& msg );
void do_ExApetExpUse( CPC* ch, CNetMsg::SP& msg );

// ������ �� ����
void do_AttackPet(CPC* ch, CNetMsg::SP& msg )
{
	unsigned char subtype;

	RefMsg(msg) >> subtype;

	switch( subtype )
	{
	case MSG_SUB_REBIRTH :
		do_ExApetRebirth( ch, msg );
		break;
	case MSG_SUB_SKILLLEAN :
		do_ExApetSkillLean( ch, msg );
		break;
	case MSG_SUB_SKILLLIST:
		do_ExApetSkillListReq( ch, msg );
		break;
	case MSG_SUB_ITEM_WEAR:
		do_ExApetItemWear( ch, msg );
		break;
	case MSG_SUB_STATUP :
		do_ExApetStatUp( ch, msg );
		break;
	case MSG_SUB_APET_INFO :
		do_ExApetInfo(ch, msg);
		break;
	case MSG_SUB_AI_LIST_REQ:
		do_ExApetAIListReq(ch, msg);
		break;
	case MSG_SUB_AI_UPDATE_REQ:
		do_ExApetAIUpdateReq(ch, msg);
		break;
	case MSG_SUB_AI_ONOFF_REQ:
		do_ExApetAIOnOffReq(ch, msg);
		break;
	case MSG_SUB_EVOLUTION:
		do_ExApetEvolution(ch,msg);
		break;
	case MSG_SUB_MOUNT_REQ:
		do_ExApetMount( ch, msg );
		break;
	case MSG_SUB_EXPINFO:
		do_ExApetExpInfo( ch, msg );
		break;
	case MSG_SUB_EXPUSE:
		do_ExApetExpUse( ch, msg );
		break;
	default :
		break;
	}
}

void do_ExApetStatUp( CPC* ch, CNetMsg::SP& msg)
{
	char	stat_code;
	RefMsg(msg) >>	stat_code;

	CAPet* apet = ch->GetAPet();
	if( !apet )
		return;

	if( apet->m_nRemainStat < 1 )
		return;

	// 0: str, 1:con, 2: dex, 3:Int
	switch( stat_code )
	{
	case 0:	// str
		apet->m_nPlusStr++;
		break;
	case 1: // con
		apet->m_nPlusCon++;
		break;
	case 2: // dex
		apet->m_nPlusDex++;
		break;
	case 3:	// Int
		apet->m_nPlusInt++;
		break;
	default :
		return;
	}

	apet->m_nRemainStat--;

	apet->CalcStatus(false);

	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExAPetStatusMsg( rmsg, apet );
		SEND_Q( rmsg, ch->m_desc );
	}
}

void do_ExApetRebirth( CPC* ch, CNetMsg::SP& msg)
{
	RequestClient::doExApetRebirth* packet = reinterpret_cast<RequestClient::doExApetRebirth*>(msg->m_buf);

	bool npc_check_flag = true;

#ifdef PREMIUM_CHAR
	//XX - �����̾�ĳ���� : �������� �� ��Ȱ
	if (ch->m_premiumChar.isActive())
	{
		npc_check_flag = false;
	}
	else
	{
		npc_check_flag = true;
	}
#endif

	if (npc_check_flag)
	{
		CNPC* pNpc = ch->m_pArea->FindProtoNPCInCell(ch, packet->npcIndex, false, 2);
		if (pNpc == NULL)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			ExAPetFuntionMsg( rmsg, MSG_SUB_ERROR_NOT_EXIST_NPC, NULL , 1 );		// ���� �ƴմϴ�.
			SEND_Q( rmsg, ch->m_desc );
			return;
		}
	}
	
	if (ch->m_inventory.isValidNormalInventory(packet->tab, packet->invenIndex) == false)
	{
		LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab, packet->invenIndex);
		ch->m_desc->Close("invalid packet");
		return;
	}

	CItem *petItem = ch->m_inventory.getItem(packet->tab, packet->invenIndex);
	if (petItem == NULL)
	{
		LOG_ERROR("HACKING? : not found item. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab, packet->invenIndex);
		ch->m_desc->Close("not found item");
		return;
	}

	if (petItem->IsAPet() == false)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExAPetFuntionMsg( rmsg, MSG_SUB_REBIRTH, NULL , 1 );		// ���� �ƴմϴ�.
		SEND_Q( rmsg, ch->m_desc );
		return;
	}

	int petIndex = petItem->getPlus();
	CAPet* apet = ch->GetAPet(petIndex);
	if( apet )
	{
		// ���� ����
		if( !(petItem->getFlag() & FLAG_ITEM_SEALED) )
		{
			CNetMsg::SP rmsg(new CNetMsg);
			ExAPetFuntionMsg( rmsg, MSG_SUB_REBIRTH, NULL , 3 );		// ���ε��� ���� ������
			SEND_Q( rmsg, ch->m_desc );
			return;
		}
		// �ʿ� ���� ó��  //////////////////////////////////////////////////////
		int nNeedMoney = (apet->m_level-1)*((apet->m_level*apet->m_level*8)+20000);
		if( nNeedMoney != 0 )		// �ʿ� �ݾ��� 0 �̸�  �׳����ֱ�
		{
			if( ch->m_inventory.getMoney() < nNeedMoney )
			{
				// ���� ������
				CNetMsg::SP rmsg(new CNetMsg);
				ExAPetFuntionMsg( rmsg, MSG_SUB_REBIRTH, NULL , 2 );		// ���� �����մϴ�.
				SEND_Q( rmsg, ch->m_desc );
				return;
			}
			else
			{
				ch->m_inventory.decreaseMoney(nNeedMoney);
			}
		}
		//////////////////////////////////////////////////////////////////////////

		/// ȸ�� /////////////////////////////////////////////////////////////////
		apet->m_hp = apet->m_maxHP * 40 / 100;					// HP 40% ȸ��
		petItem->setFlag(petItem->getFlag() & ~FLAG_ITEM_SEALED);

		{
			CNetMsg::SP rmsg(new CNetMsg);
			UpdateClient::makeUpdateItemFlag(rmsg, petItem->tab(), petItem->getInvenIndex(), petItem->getFlag());
			SEND_Q(rmsg, ch->m_desc);
		}

		{
			CNetMsg::SP rmsg(new CNetMsg);
			ExAPetFuntionMsg( rmsg, MSG_SUB_REBIRTH, NULL , 0 );		// ���� ���� ����
			SEND_Q( rmsg, ch->m_desc );
		}
	}
}

void do_ExApetSkillListReq( CPC* ch, CNetMsg::SP& msg )
{
	CAPet* apet = ch->GetAPet();
	if( apet )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExAPetFuntionMsg( rmsg, MSG_SUB_SKILLLIST, apet , 0 );
		SEND_Q( rmsg, ch->m_desc );
	}
	else
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExAPetFuntionMsg( rmsg, MSG_SUB_SKILLLIST, apet , 1 );		// FAIL
		SEND_Q( rmsg, ch->m_desc );
	}
}

void do_ExApetSkillLean( CPC* ch, CNetMsg::SP& msg )
{
	int skillindex = -1;

#ifdef NPC_CHECK
	int npcIndex;
	CNPC *npc;
	RefMsg(msg) >> npcIndex
				>> skillindex;
	npc = ch->m_pArea->FindProtoNPCInCell(ch, npcIndex, false, 2);
	if(!npc)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExAPetFuntionMsg( rmsg, MSG_SUB_ERROR_NOT_EXIST_NPC, NULL , 1 );		// FAIL
		SEND_Q(rmsg, ch->m_desc);
		GAMELOG << init("ExApetSkill Learn FAIL: NOT EXIST NPC (Dangerous User)", ch) << end;
		return ;
	}
	if( !npc->IsFlag(NPC_EVENT) )
	{
		GAMELOG << init("ExApetSkill Learn FAIL: NOT EXIST NPC (Dangerous User)", ch) << end;
		return ;
	}
#else
	RefMsg(msg) >> skillindex;
#endif

	CAPet* apet = ch->GetAPet();
	if (apet == NULL)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExAPetFuntionMsg( rmsg, MSG_SUB_SKILLLEAN, NULL , 1 );		// FAIL
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	const CSkillProto* skillProto = gserver->m_skillProtoList.Find(skillindex);
	if (skillProto == NULL)
		return ;

	// ���� �ش� ��ų�� ������ �ִ��� �˻�
	CSkill* curSkill = apet->FindSkill(skillindex);
	int nextlevel = (curSkill == NULL) ? 1 : curSkill->m_level + 1;

	const CSkillLevelProto* skillLevelProto = skillProto->Level(nextlevel);
	if (skillLevelProto == NULL)
		return ;
	if (skillProto->m_maxLevel < nextlevel)
		return ;

	// ���� �˻� : ����
	if ( ! skillProto->IsSameJob(apet) )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExAPetFuntionMsg( rmsg, MSG_SUB_SKILLLEAN, NULL , 1 );		// FAIL
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	// ���� �˻� : ����
	if (skillLevelProto->m_learnLevel > apet->m_level)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExAPetFuntionMsg( rmsg, MSG_SUB_SKILLLEAN, NULL , 2 );		// ������ ���� ����
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	// ���� �˻� : ����Ʈ : ��� ����Ʈ�� SP�� �޸� ������ 1�� ����
	if (skillLevelProto->m_learnSP > apet->m_nSP )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExAPetFuntionMsg( rmsg, MSG_SUB_SKILLLEAN, NULL , 3 );		// ��ų����Ʈ ����
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	// ���� �˻� : ������
	item_search_t vec[MAX_SKILL_LEARN_ITEM];
	for (int i = 0; i < MAX_SKILL_LEARN_ITEM; i++)
	{
		if (skillLevelProto->m_learnItemIndex[i] < 0)
			continue ;

		int sc = ch->m_inventory.searchItemByCondition(skillLevelProto->m_learnItemIndex[i], 0, 0, vec[i]);
		if (sc == 0 || sc < skillLevelProto->m_learnItemCount[i])
		{
			CNetMsg::SP rmsg(new CNetMsg);
			ExAPetFuntionMsg( rmsg, MSG_SUB_SKILLLEAN, NULL , 4 );		// �ʿ��� ������ ����
			SEND_Q(rmsg, ch->m_desc);
			return ;
		}
	}

	// ��ų �߰�
	if (curSkill == NULL)
	{
		// �ű� �߰�
		curSkill = gserver->m_skillProtoList.Create(skillindex);
		if (!curSkill)
			return ;

		apet->AddSkill(curSkill);
	}
	else
	{
		// ���� ��ų ���� ��
		curSkill->m_level++;
	}

	{
		// ����Ʈ �Ҹ�
		apet->m_nSP -= skillLevelProto->m_learnSP;
		CNetMsg::SP rmsg(new CNetMsg);
		ExAPetStatusMsg(rmsg, apet);
		SEND_Q(rmsg, ch->m_desc);
	}

	// ������ �Ҹ�
	for (int i = 0; i < MAX_SKILL_LEARN_ITEM; i++)
	{
		ch->m_inventory.deleteItem(vec[i], skillLevelProto->m_learnItemCount[i]);
	}
	apet->CalcStatus(true);

	{
		// ��� �˸���
		CNetMsg::SP rmsg(new CNetMsg);
		ExAPetFuntionMsg( rmsg, MSG_SUB_SKILLLEAN, NULL , 0 );		// ���� ����
		RefMsg(rmsg) << skillindex << (char)nextlevel;						// �����ø� ��ų�ε��� , ��ų���� �߰�
		SEND_Q(rmsg, ch->m_desc);
	}

	{
		// ��ų ����Ʈ ����
		CNetMsg::SP rmsg(new CNetMsg);
		ExAPetFuntionMsg( rmsg, MSG_SUB_SKILLLIST, apet , 0 );		// ���� ����
		SEND_Q(rmsg, ch->m_desc);
	}

	// ���� �α�
	// TODO : petlog
	GAMELOG << init("ATTACK PET SKILL LEARN", ch)
			<< "ATTACK_PET" << delim
			<< apet->m_index << delim
			<< apet->m_name << delim
			<< "SKILL" << delim
			<< curSkill->m_proto->m_index << delim
			<< curSkill->m_level
			<< end;
}

void do_ExApetItemWear( CPC* ch, CNetMsg::SP& msg )
{
	CAPet* apet = ch->GetAPet();
	if( apet == NULL )
	{
		// ���� �����Ͻð� �������� �����ÿ�
		return;
	}

	RequestClient::doExApetItemWear* packet = reinterpret_cast<RequestClient::doExApetItemWear*>(msg->m_buf);

	if (packet->wearPos < 0 || packet->wearPos > APET_WEARPOINT )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_CANNOT_WEAR);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	CItem* item = ch->m_inventory.getItem(packet->tab, packet->invenIndex);

	// �׳� ����
	if (packet->tab == INVALID_TAB && packet->invenIndex == INVALID_INDEX)
	{
		packet->itemIdx = -1;
		item = NULL;
	}

	//HUMAN_PET_TYPE (1)	, BEAST_PET_TYPE (2)
	// grade 0 : NONE, 1 : HUMAN , 2 : BEAST
	if( item && apet->m_pProto && apet->m_pProto->m_Type != item->m_itemProto->getItemGrade()  )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_CANNOT_WEAR);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	// ���� ��ġ�� �ùٸ��� �˻�
	if (item && item->m_itemProto->getItemWearing() != packet->wearPos)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_CANNOT_WEAR);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	// �Դ� �������� ���յ� �������̸� �Ұ���
	if (item && !item->CanWear())
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_CANNOT_WEAR);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	if(  item && item->IsRareItem() && apet->m_level < item->m_itemProto->GetItemProtoLevel() )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_CANNOT_WEAR);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	CItem * item2 = apet->m_wearing[(int)packet->wearPos];		// ���� ������

	// �Ѵ� NULL �̸� ����
	if (!item && !item2)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_CANNOT_WEAR);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	if (!item)
	{
		// �����̸� ������ ��ġ�� �ùٸ��� ���޵Ǿ� ��
		if (item2 != NULL && item2->m_itemProto->getItemWearing() != packet->wearPos)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_CANNOT_WEAR);
			SEND_Q(rmsg, ch->m_desc);
			return ;
		}
	}

	if (packet->itemIdx == -1)
	{
		// ����
		if (!item2)
		{
			// ���ڸ��� ���ڸ� ������� ����
			return ;
		}

		// ����ڵ� : ���� �������µ� �� ������ �Ϲ� ����
		if (item2->getWearPos() == WEARING_NONE)
			return;

		// ���� �� �κ� �� �������� ĳ���� �κ��� Item �־� �ش�
		// item2 �� �κ��� �߰�
		int oldWearPos = item2->getWearPos();
		item2->unWearPos();
		if (ch->m_inventory.addItem(item2))
		{
			apet->m_wearing[(int)packet->wearPos] = NULL;
		}
		else
		{
			// �κ��� ���� ����
			item2->setWearPos(oldWearPos);

			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_FULL_INVENTORY);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}

		{
			CNetMsg::SP rmsg(new CNetMsg);
			ItemAPetWearMsg( rmsg, packet->wearPos, NULL, item2);
			SEND_Q(rmsg, ch->m_desc);
		}
	}
	else
	{
		// �Ա�
		if (!item)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_CANNOT_WEAR);
			SEND_Q(rmsg, ch->m_desc);
			return ;
		}

		if (!(item->m_itemProto->getItemJobFlag() & (1 << JOB_APET)))
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_CANNOT_WEAR);
			SEND_Q(rmsg, ch->m_desc);
			return ;
		}

		// ����ڵ� : ���� ������ �ִ��� �ƴϸ� ����
		if (item->getWearPos() != WEARING_NONE)
			return;

		if (item2)
		{
			// ���粨 �����
			// ĳ���� �κ��� �ֱ�
			int oldWearPos = item2->getWearPos();
			item2->unWearPos();
			if (ch->m_inventory.addItem(item2))
			{
				apet->m_wearing[(int)packet->wearPos] = NULL;
			}
			else
			{
				// �κ��� ���� ����
				item2->setWearPos(oldWearPos);

				CNetMsg::SP rmsg(new CNetMsg);
				SysMsg(rmsg, MSG_SYS_FULL_INVENTORY);
				SEND_Q(rmsg, ch->m_desc);
				return;
			}
		}

		{
			// �κ����� ���� ���κ� �� �ְ� //////////////////////////////////////////
			apet->m_wearing[(int)packet->wearPos] = item;
			item->setWearPos(packet->wearPos);

			ch->m_inventory.eraseNotFree(item);
		}

		{
			CNetMsg::SP rmsg(new CNetMsg);
			AddItemExAPetMsg(rmsg, item);
			SEND_Q(rmsg, ch->m_desc);
		}

		{
			CNetMsg::SP rmsg(new CNetMsg);
			ItemAPetWearMsg(rmsg, packet->wearPos, item, item2);
			SEND_Q(rmsg, ch->m_desc);
		}
	}

	apet->CalcStatus(false);

	{
		// ������ ���� ���� �˸�
		CNetMsg::SP rmsg(new CNetMsg);
		ItemAPetWearingMsg( rmsg, apet );
		ch->m_pArea->SendToCell(rmsg, ch, false);
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExAPetStatusMsg( rmsg, apet );
		ch->m_pArea->SendToCell(rmsg, ch, true);
	}
}

void do_ExApetInfo( CPC* ch, CNetMsg::SP& msg )
{
	int pet_index;
	int char_index;

	RefMsg(msg) >> char_index
				>> pet_index;

	CPC* tpc = PCManager::instance()->getPlayerByCharIndex( char_index );
	if( tpc == NULL )
		return;

	CAPet* apet = tpc->GetAPet( pet_index );
	if( apet )
	{
		apet->CalcStatus(false);

		CNetMsg::SP rmsg(new CNetMsg);
		ExApetSellInfo( rmsg, apet );
		SEND_Q( rmsg, ch->m_desc );
	}
}

void do_ExApetAIListReq( CPC* ch, CNetMsg::SP& msg )
{
	CAPet * pAPet = ch->GetAPet();
	if (!pAPet)
		return;

	{
		CNetMsg::SP rmsg(new CNetMsg);
		APetAIListMsg(rmsg, pAPet);
		SEND_Q(rmsg, ch->m_desc);
	}
}

void do_ExApetAIOnOffReq(CPC* ch, CNetMsg::SP& msg)
{
	char cEnable;
	unsigned char ucError = MSG_APET_ERROR_OK;

	RefMsg(msg) >> cEnable;

	CAPet * pAPet = ch->GetAPet();
	if (pAPet)
		pAPet->m_cAI = cEnable;
	else
		ucError = MSG_APET_ERROR;

	{
		CNetMsg::SP rmsg(new CNetMsg);
		APetAIOnOffMsg(rmsg, pAPet, ucError);
		SEND_Q(rmsg, ch->m_desc);
	}
}

void do_ExApetAIUpdateReq(CPC* ch, CNetMsg::SP& msg)
{
	CAPet * pAPet = ch->GetAPet();
	if (!pAPet)
		return;

	int nSlot = 0, nAICount = 0;
	int nAIindex[APET_MAX_AI] = {0,};
	unsigned char ucError = MSG_APET_ERROR_OK;
	APET_AI_DATA tAI[APET_MAX_AI];
	memset(tAI, -1, sizeof(APET_AI_DATA) * APET_MAX_AI);

	RefMsg(msg) >> nSlot >> nAICount;

	if (nAICount > pAPet->m_nAISlot || nAICount > APET_MAX_AI)
		return;

	int i = 0;
	for (i = 0; i < nAICount; i++)
	{
		RefMsg(msg) >> nAIindex[i];
		RefMsg(msg) >> tAI[i].nItemNum
					>> tAI[i].nSkillType
					>> tAI[i].nSkillIdx
					>> tAI[i].cUse;
	}

	memset(pAPet->m_tAIData, -1, sizeof(APET_AI_DATA) * APET_MAX_AI);
	for (i = 0; i < APET_MAX_AI; i++)
	{
		pAPet->m_tAIData[i].cUse = 0;
		pAPet->m_tAIData[i].pItem = NULL;
	}

	int nIndex = 0;
	for (i = 0; i < nAICount; i++)
	{
		if ((nAIindex[i] < 0 || nAIindex[i] > APET_MAX_AI))
		{
			ucError = MSG_APET_ERR_WRONG_AI;
			GAMELOG << init("APET_AI_UPDATE", ch) << delim << "ai count : " << delim << nSlot << end;
			break;
		}

		CItem * pItem = ch->m_inventory.FindByDBIndex(tAI[i].nItemNum);
		if (pItem == NULL || (pItem && (pItem->m_itemProto->getItemTypeIdx() != ITYPE_ETC || pItem->m_itemProto->getItemSubTypeIdx() != IETC_PET_AI)))
		{
			ucError = MSG_APET_ERR_NOT_HAVE_ITEM;
			GAMELOG << init("APET_AI_UPDATE ITEM", ch) << delim << "ai count : " << delim << nSlot << delim;

			if (pItem)
			{
				GAMELOG << "item index : " << pItem->m_itemProto->getItemIndex() << delim
						<< "skill index : " << tAI[i].nSkillIdx << end;
			}
			break;
		}

		if (tAI[i].nSkillType == 0 && !pAPet->FindSkill(tAI[i].nSkillIdx))
		{
			ucError = MSG_APET_ERR_NOT_HAVE_ITEM;
			GAMELOG << init("APET_AI_UPDATE SKILL", ch) << delim << "ai count : " << delim << nSlot << delim
					<< "item index : " << pItem->m_itemProto->getItemIndex() << delim
					<< "skill index : " << tAI[i].nSkillIdx << end;
			break;
		}

		nIndex = nAIindex[i];
		pAPet->m_tAIData[nIndex].pItem		= pItem;
		pAPet->m_tAIData[nIndex].nItemNum	= pItem->m_itemProto->getItemIndex();
		pAPet->m_tAIData[nIndex].nSkillIdx	= tAI[i].nSkillIdx;
		pAPet->m_tAIData[nIndex].nSkillType	= tAI[i].nSkillType;
		pAPet->m_tAIData[nIndex].cUse		= tAI[i].cUse;
	}
	pAPet->m_nAICount = nAICount;

	{
		CNetMsg::SP rmsg(new CNetMsg);
		rmsg->Init(MSG_EXTEND);
		RefMsg(rmsg) << MSG_EX_ATTACK_PET
					 << (unsigned char)MSG_SUB_AI_UPDATE_REP
					 << ucError;
		SEND_Q(rmsg, ch->m_desc);
	}

	pAPet->m_nAICount = nAICount;

	{
		CNetMsg::SP rmsg(new CNetMsg);
		APetAIListMsg(rmsg, pAPet);
		SEND_Q(rmsg, ch->m_desc);
	}
}

void do_ExApetEvolution(CPC* ch, CNetMsg::SP& msg)
{
	int		nEvPetIndex = 0 ;

#ifdef NPC_CHECK
	int npcIndex;
	CNPC *npc;
	RefMsg(msg) >> npcIndex
				>> row >> col;
	npc = ch->m_pArea->FindProtoNPCInCell(ch, npcIndex, false, 2);
	if(!npc)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExAPetFuntionMsg( rmsg, MSG_SUB_ERROR_NOT_EXIST_NPC, NULL , 2 );		// ������ �Ұ���
		SEND_Q( rmsg, ch->m_desc );
		GAMELOG << init("ExApet Evolution FAIL: NOT EXIST NPC (Dangerous User)", ch) << end;
		return ;
	}
	if( !npc->IsFlag(NPC_QUEST) && !npc->IsFlag(NPC_EVENT) )
	{
		GAMELOG << init("ExApet Evolution FAIL: NOT EXIST NPC (Dangerous User)", ch) << end;
		return ;
	}
#endif

	RequestClient::doExApetEvolution* packet = reinterpret_cast<RequestClient::doExApetEvolution*>(msg->m_buf);

	if (ch->m_inventory.isValidNormalInventory(packet->tab, packet->invenIndex) == false)
	{
		LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab, packet->invenIndex);
		ch->m_desc->Close("invalid packet");
		return;
	}

	CItem *petItem = ch->m_inventory.getItem(packet->tab, packet->invenIndex);
	if (petItem == NULL)
	{
		LOG_ERROR("HACKING? : not found item. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab, packet->invenIndex);
		ch->m_desc->Close("not found item");
		return;
	}

	if (petItem->IsAPet() == false)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExAPetFuntionMsg( rmsg, MSG_SUB_EVOLUTION, NULL , 4 );		// �� ����
		SEND_Q( rmsg, ch->m_desc );
		return;
	}

	int petIndex = petItem->getPlus();
	CAPet* apet = ch->GetAPet(petIndex);
	if( apet && apet->m_pProto )
	{
		if( petItem->getWearPos() > 0 )
		{
			CNetMsg::SP rmsg(new CNetMsg);
			ExAPetFuntionMsg( rmsg, MSG_SUB_EVOLUTION, NULL , 2 );		// ������ �Ұ���
			SEND_Q( rmsg, ch->m_desc );
			return;
		}

		if( apet->m_pProto->m_listEvInfo.empty() == false )
		{
			std::vector<APET_EV_INFO>::iterator it = apet->m_pProto->m_listEvInfo.begin();
			std::vector<APET_EV_INFO>::iterator endit = apet->m_pProto->m_listEvInfo.end();
			for(; it != endit; ++it)
			{
				APET_EV_INFO& data = *(it);

				int comp[2] , nDivid = apet->m_level -1;
				if( apet->m_level < 2 )
				{
					nDivid = 1;
				}

				if( data.nLevel <= apet->m_level &&
						data.nFaith <= apet->m_pProto->m_nMaxFaith - (int)( apet->m_nAccFaith / nDivid  ) &&
						data.nStm <= apet->m_pProto->m_nMaxStm - (int) ( apet->m_nAccStemina / nDivid ) )
				{
					for( int idx = 0; idx < 2 ; idx++ )
					{
						switch( data.nStatType[idx] )
						{
						case APET_EV_NONE:
							comp[idx]=0;
							break;
						case APET_EV_STR:
							comp[idx]=apet->m_nPlusStr;
							break;
						case APET_EV_INT:
							comp[idx]=apet->m_nPlusInt;
							break;
						case APET_EV_DEX:
							comp[idx]=apet->m_nPlusDex;
							break;
						case APET_EV_CON:
							comp[idx]=apet->m_nPlusCon;
							break;
						}
					}

					if( comp[0] >= comp[1] )
					{
						nEvPetIndex = data.nEvPetIndex;
						break;
					}
				}
			}
		}

		if( DEAD(apet) || nEvPetIndex == 0 || ( petItem->getFlag() & FLAG_ITEM_SEALED ) )
		{
			CNetMsg::SP rmsg(new CNetMsg);
			ExAPetFuntionMsg( rmsg, MSG_SUB_EVOLUTION, NULL , 4 );		// �� ����
			SEND_Q( rmsg, ch->m_desc );
			return;
		}

		CAPet* pEvoPet = gserver->m_pApetlist.Create( ch, apet->m_index, nEvPetIndex );
		if( pEvoPet )
		{
			CItem *newPetItem = gserver->m_itemProtoList.CreateItem( pEvoPet->m_pProto->m_nItemIndex, -1, apet->m_index, 0, 1 );
			if (ch->m_inventory.addItem(newPetItem))
			{
				ch->m_inventory.decreaseItemCount(petItem, 1);

				pEvoPet->Evolution( apet );

				GAMELOG << init("APET EVOLUTION",ch) << apet->m_pProto->Index() <<
						delim << apet->m_pProto->m_strName <<
						" => " << pEvoPet->m_pProto->Index() <<
						delim << pEvoPet->m_pProto->m_strName <<
						end;

				REMOVE_FROM_BILIST(apet, ch->m_pApetlist, m_pPrevPet, m_pNextPet);
				delete apet;
				apet = NULL;
				ADD_TO_BILIST(pEvoPet, ch->m_pApetlist, m_pPrevPet, m_pNextPet);

				{
					CNetMsg::SP rmsg(new CNetMsg);
					ExAPetFuntionMsg( rmsg, MSG_SUB_EVOLUTION, NULL , 0 );		// ����
					SEND_Q( rmsg, ch->m_desc );
				}

				{
					CNetMsg::SP rmsg(new CNetMsg);
					pEvoPet->CalcStatus(false);									// �� ���� ����
					ExApetSellInfo( rmsg, pEvoPet );
					SEND_Q( rmsg, ch->m_desc );
				}
				return;
			}
			else
			{
				delete newPetItem;
				delete pEvoPet;

				CNetMsg::SP rmsg(new CNetMsg);
				ExAPetFuntionMsg( rmsg, MSG_SUB_EVOLUTION, NULL , 3 );		// �κ� ����
				SEND_Q( rmsg, ch->m_desc );
				return;
			}
		}
		else
		{
			CNetMsg::SP rmsg(new CNetMsg);
			ExAPetFuntionMsg( rmsg, MSG_SUB_EVOLUTION, NULL , 4 );		// �� ����
			SEND_Q( rmsg, ch->m_desc );
			return;
		}
	}
}

void do_ExApetMount(CPC *ch, CNetMsg::SP& msg )
{
	char type = 0;
	RefMsg(msg) >> type;
	if(ch)
	{
		if(ch->m_evocationIndex != EVOCATION_NONE)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_CANT_MOUNT_EVOCATION);
			SEND_Q(rmsg, ch->m_desc);
			return ;
		}
	}

	CAPet * pAPet = ch->GetAPet();
	if (pAPet)
	{
		if( !pAPet->IsSummon() )
		{
			return;
		}

		// Ż�� ���� ����
		if (!ch->m_pZone->m_bCanMountPet)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_PET_CANT_MOUNT);
			SEND_Q(rmsg, ch->m_desc);
			return ;
		}

		// ������ Ż���� �غ�
		int nMountCount = pAPet->m_pProto->m_nMount[ (unsigned int)pAPet->m_cTransSate ];
		if( nMountCount > 0 )
		{
			if( type == 1 )		// Ÿ��
			{
				if( pAPet->IsMount() )
				{
					CNetMsg::SP rmsg(new CNetMsg);
					ExAPetFuntionMsg( rmsg, MSG_SUB_MOUNT_REP, pAPet , 1 );
					SEND_Q( rmsg, ch->m_desc );
					// �̹� Ÿ���ִ�
					return;
				}

				pAPet->Mount( true );

				{
					CNetMsg::SP rmsg(new CNetMsg);
					ExAPetFuntionMsg( rmsg, MSG_SUB_MOUNT_REP, pAPet , 0 );
					//ch->m_pArea->SendToCell(rmsg, ch, true);
					SEND_Q(rmsg, ch->m_desc);
				}
			}
			else				// ������
			{
				pAPet->Mount( false );

				CNetMsg::SP rmsg(new CNetMsg);
				ExAPetFuntionMsg( rmsg, MSG_SUB_MOUNT_REP, pAPet , 0 );
				//ch->m_pArea->SendToCell(rmsg, ch, true);
				SEND_Q(rmsg, ch->m_desc);
			}

			{
				CNetMsg::SP rmsg(new CNetMsg);
				APetAIOnOffMsg(rmsg, pAPet, MSG_APET_ERROR_OK);
				SEND_Q(rmsg, pAPet->GetOwner()->m_desc);
			}

			{
				CNetMsg::SP rmsg(new CNetMsg);
				APetAIListMsg(rmsg, pAPet);
				SEND_Q(rmsg, pAPet->GetOwner()->m_desc);
			}
		}
		else
		{
			// ����Ʈ �Ұ����� �� or ����
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_PET_CANT_MOUNT_ETC);
			SEND_Q(rmsg, ch->m_desc);
			return ;
		}
	}
}

void do_ExApetExpInfo( CPC* ch, CNetMsg::SP& msg )
{
}

void do_ExApetExpUse( CPC* ch, CNetMsg::SP& msg )
{
	CAPet* apet = ch->GetAPet();
	if( !apet )
		return;

	const APET_EXP_INFO* myExpInfo = apet->m_pProto->GetExpInfo();

	if (myExpInfo == NULL)
		return;

	char	type;
	int		index;
	unsigned char errorCode = 0;
	LONGLONG exp = apet->GetAccExp();

	RefMsg(msg) >> type
				>> index;

	switch ( type )
	{
	case MSG_CHAR_APET:		// �꿡��
		{
			CAPet*	targetAPet = ch->GetAPet(index);
			if( !targetAPet )
				errorCode = 1 ;					// 1.����� �ùٸ��� �ʽ��ϴ�.
			else if ( targetAPet == apet )		// �������� �꿣 ����Ҽ� �����ϴ�.
				errorCode = 2 ;
			else
			{
				if(apet->GetCooltime() > 0)
					errorCode = 2;
				else if( false == apet->UseAccExp(targetAPet, true) )
					errorCode = 1;
			}
		}
		break;
	case MSG_CHAR_PC:		// PC����
		{
			CPC* targetPC = PCManager::instance()->getPlayerByCharIndex(index);
			if( !targetPC )
				errorCode = 1;
			else
			{
				if(apet->GetCooltime() > 0)
					errorCode = 2;
				else
					apet->UseAccExp(targetPC);
			}
		}
		break;
	default:
		{
			errorCode = 3;
		}
		break;
	}

	{
		// ����?
		CNetMsg::SP rmsg(new CNetMsg);
		APetUseMsg( rmsg , errorCode );
		SEND_Q( rmsg, ch->m_desc );
	}
	GAMELOG << init("ApetExpUse") << delim
			<< ch->GetName() << delim
			<< ch->m_index << delim
			<< apet->m_index << delim;
	if(type == MSG_CHAR_APET)
	{
		CAPet*	targetAPet = ch->GetAPet(index);
		GAMELOG << "To pet" << delim
				<< targetAPet->m_index << delim;
	}
	if(type == MSG_CHAR_PC)
	{
		GAMELOG << "To char" << delim;
	}
	GAMELOG << exp << end;
	GAMELOG << init("ApetExpUse") << delim
			<< ch->GetName() << delim
			<< ch->m_index << delim
			<< apet->m_index << delim;
	if(type == MSG_CHAR_APET)
	{
		CAPet*	targetAPet = ch->GetAPet(index);
		GAMELOG << "To pet" << delim
				<< targetAPet->m_index << delim;
	}
	if(type == MSG_CHAR_PC)
	{
		GAMELOG << "To char" << delim;
	}
	GAMELOG << exp << end;

	// errorCode�� �����ϰ� , EXP �꿡�� ��Ÿ�� ����
	if ( errorCode == 0 && ch->m_pApetlist )
	{
		CAPet* pAPet;
		CAPet* pAPetNext = ch->m_pApetlist;
		while ((pAPet = pAPetNext))
		{
			pAPetNext = pAPetNext->m_pNextPet;
			if( pAPet == apet )			// ����ѻ������ �ڽ��� ��Ÿ�� ����
			{
				pAPet->SetCooltime( myExpInfo->nCooltime );
			}
			else
			{
				// �ٸ� ����ġ �꿡�� �ڽ��� ��Ÿ�ӷ��� ����
				if( pAPet->IsProtoFlag( APET_FLAG_EXP ) )
				{
					const APET_EXP_INFO* expInfo = pAPet->m_pProto->GetExpInfo();
					if( expInfo )
					{
						pAPet->SetCooltime( myExpInfo->nCooltime * expInfo->nCooltimeRate / 100 );

						CNetMsg::SP rmsg(new CNetMsg);
						ExApetSellInfo( rmsg, pAPet );
						SEND_Q(rmsg, ch->m_desc );
					}
				}
			}
		}
		ch->SendStatus();
	}
}

