#include "stdhdrs.h"

#include "Log.h"
#include "Character.h"
#include "Server.h"
#include "CmdMsg.h"
#include "doFunc.h"
#include "Battle.h"
#include "WarCastle.h"
#include "../ShareLib/packetType/ptype_old_do_item.h"

int GetGradeProductItem(int itemdbindex)
{
	int grade = -1;
	switch (itemdbindex)
	{
	case 156:	// ���� ��			E
	case 161:	// E��� ����		E
	case 199:	// ũ���� �����	E
		grade = 0;
		break;

	case 160:	// D��� ����		D
	case 155:	// ���� ����		D
	case 198:	// ũ���� �ٱ�		D
		grade = 1;
		break;

	case 159:	// C��� ����		C
	case 154:	// ���� ����		C
	case 197:	// ũ���� �Ķ���	C
		grade = 2;
		break;

	case 158:	// B��� ����		B
	case 153:	// ���� ��Ʈ		B
	case 163:	// ũ���� ����		B
		grade = 3;
		break;

	case 152:	// ���� ������		A
	case 157:	// A��� ����		A
	case 162:	// ũ���� ��		A
		grade = 4;
		break;
	}

	return grade;
}

//////////////////////////////
// ���� ����
void do_SelectProduce(CPC* ch, CNetMsg::SP& msg)
{
	if (DEAD(ch))
		return ;

	if (ch->IsSetPlayerState(PLAYER_STATE_CHANGE))
		return;

	int npcindex;
	int itemdbindex;

	msg->MoveFirst();

	RefMsg(msg) >> npcindex
				>> itemdbindex;

	// ��ó�� NPC ã��
	CNPC* npc = TO_NPC(ch->m_pArea->FindCharInCell(ch, npcindex, MSG_CHAR_NPC, false));
	if (!npc)
		return ;

	// NPC�� a_product0 ~ 4���� ã��
	int i;
	for (i = 0; i < MAX_PRODUCTS_FROM_NPC; i++)
	{
		if (npc->m_proto->m_product[i] == itemdbindex)
			break;
	}

	if (i == MAX_PRODUCTS_FROM_NPC)
		return ;

	// �ӵ� �˻�
	ch->m_hackProduceCount--;
	ch->m_ProduceClientTime += gserver->m_pulse - ch->m_lastProducePulse + PULSE_HACK_ATTACK_THRESHOLD;

	if (ch->m_hackProduceCount < 1)
	{
		if( ch->m_ProduceClientTime < PULSE_PRODUCE_DELAY * PULSE_HACK_ATTACK_COUNT)
		{
			GAMELOG << init("HACK PRODUCE SPEED", ch)
					<< (gserver->m_pulse - ch->m_lastProducePulse)
					<< end;
			ch->m_desc->IncreaseHackCount(0);
			return;
		}
		ch->m_hackProduceCount = PULSE_HACK_ATTACK_COUNT;	// ����޼��� �� �� ����
		ch->m_ProduceClientTime = 0; // Ŭ���̾�Ʈ�� ���� ����ð�
	}

	ch->m_lastProducePulse = gserver->m_pulse;

	// �Ÿ� �˻�
	if (GetDistance(ch, npc) > npc->m_proto->m_attackArea + 0.5)
	{
		GAMELOG << init("HACK PRODUCE DIST", ch)
				<< GetDistance(ch, npc)
				<< end;
		// ch->m_desc->IncreaseHackCount(6);
		return ;
	}

	// ������ DB �ε��� �˻�
	int npcflag = -1;			// NPC ����
	int needitemtype = -1;		// ���� ����
	int needskilltype = -1;		// ��ų ����
	switch (itemdbindex)
	{
	case 152:	// ���� ������
	case 153:	// ���� ��Ʈ
	case 154:	// ���� ����
	case 155:	// ���� ����
	case 156:	// ���� ��
		if (!npc->m_proto->CheckFlag(NPC_MINERAL))
			return ;
		npcflag = NPC_MINERAL;
		needitemtype = IWEAPON_MINING;
		needskilltype = SSKILL_MINING;
		break;

	case 157:	// A��� ����
	case 158:	// B��� ����
	case 159:	// C��� ����
	case 160:	// D��� ����
	case 161:	// E��� ����
		if (!npc->m_proto->CheckFlag(NPC_ENERGY))
			return ;
		npcflag = NPC_ENERGY;
		needitemtype = IWEAPON_CHARGE;
		needskilltype = SSKILL_CHARGE;
		break;

	case 162:	// ũ���� ��
	case 163:	// ũ���� ����
	case 197:	// ũ���� �Ķ���
	case 198:	// ũ���� �ٱ�
	case 199:	// ũ���� �����
		if (!npc->m_proto->CheckFlag(NPC_CROPS))
			return ;
		npcflag = NPC_CROPS;
		needitemtype = IWEAPON_GATHERING;
		needskilltype = SSKILL_GATHERING;
		break;

	default:
		return ;
	}

	// ��� ���ϱ�
	int grade = GetGradeProductItem(itemdbindex);
	if (grade == -1)
		return ;

	// ���� �˻�
	if (!ch->m_wearInventory.wearItemInfo[WEARING_WEAPON])
	{
		// �ƿ� ������ġ�� �ϰ͵� ����
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_PRODUCE_NOTOOL);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	CItem* tool = ch->m_wearInventory.wearItemInfo[WEARING_WEAPON];

	// ���� ���� Ÿ�� �˻�
	bool bProduceTool = false;
	switch (tool->m_itemProto->getItemTypeIdx())
	{
	case ITYPE_WEAPON:
		switch (tool->m_itemProto->getItemSubTypeIdx())
		{
		case IWEAPON_MINING:
		case IWEAPON_GATHERING:
		case IWEAPON_CHARGE:
			bProduceTool = true;
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}
	if (!bProduceTool)
	{
		// ���� ������ �ƴ�
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_PRODUCE_WEAR_TOOL);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}
	if (tool->m_itemProto->getItemSubTypeIdx() != needitemtype)
	{
		// �ش� ���� ������ �ƴ�
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_PRODUCE_MATCH_TOOL);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	// �ν��� �ɼ� üũ
	int bBoost  = 0;
	if(tool->getFlag() & FLAG_ITEM_BOOSTER_ADDED)
		bBoost = 100;
	else if(tool->getFlag() & FLAG_ITEM_SILVERBOOSTER_ADDED)
		bBoost = 140;
	else if(tool->getFlag() & FLAG_ITEM_GOLDBOOSTER_ADDED)
		bBoost = 200;
	else if(tool->getFlag() & FLAG_ITEM_PLATINUMBOOSTER_ADDED)
		bBoost = 300;

	const int produceTax = 30;		// ����� �ʿ� ����

	// �ν��ʹ� �ݾ� �˻� ����, ������������
	if (!bBoost && npc->GetMapAttr() & MATT_PRODUCT_PRIVATE)
	{
		if (ch->m_inventory.getMoney() < produceTax)
		{
			// �� ��� �Ұ���
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_PRODUCE_NO_MONEY);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}
	}

// [101216: selo] ���� ������ ��� ����
	if( !ch->CheckInvenForProduceNoramlPrize(itemdbindex, grade, bBoost) )
	{
		return;
	}

	// �ν��� ������ �������� �� ��������
	// ���� ���� ������ �˻�
	if (!bBoost && tool->m_itemProto->getItemMaxUse() != -1 &&  tool->getUsed() <= 0)
	{
		// ���� �� �Ǿ��� ��
		ch->m_wearInventory.RemoveItem((int)tool->getWearPos());

		{
			// �������� �����
			CNetMsg::SP rmsg(new CNetMsg);
			WearingMsg(rmsg, ch, WEARING_WEAPON, -1, 0);
			ch->m_pArea->SendToCell(rmsg, ch, false);
		}

		{
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_PRODUCE_MAXUSE_TOOL);
			SEND_Q(rmsg, ch->m_desc);
		}
		return;
	}

	// ���� ���� �ο� üũ
	int nTarget = 0;
	CAttackChar* target = npc->m_attackList;
	bool bProduce = false;		// �̹��� ���� �޽������� ������ �������̴�

	while (target)
	{
		nTarget++;

		if (target->ch == ch)
			bProduce = true;

		target = target->m_next;
	}

	// ����޽��� ���������� �̹� �������̶�� �ο�üũ ���Ѵ�.
	if (!bProduce)
	{
		// ���� ���� �ο� �ʰ�
		if (nTarget >= MAX_ACCEPTABLE)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_PRODUCE_MAX_ACCEPTABLE);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}
	}

	// �������� Quest �� ���� npc�� �������̸� UpdateData
	CQuest* pQuest;
	CQuest* pQuestNext = ch->m_questList.GetNextQuest(NULL, QUEST_STATE_RUN);
	while ((pQuest = pQuestNext))
	{
		pQuestNext = ch->m_questList.GetNextQuest(pQuestNext, QUEST_STATE_RUN);
		// ���� ����Ʈ
		switch (pQuest->GetQuestType0())
		{
		case QTYPE_KIND_COLLECTION:
		case QTYPE_KIND_MINING_EXPERIENCE:
		case QTYPE_KIND_GATHERING_EXPERIENCE:
		case QTYPE_KIND_CHARGE_EXPERIENCE:
			pQuest->QuestUpdateData(ch, npc);
			break;
		default:
			break;
		}
	}

	// ��ų �˻�
	// �ε��� ū ���� �� ���� ������� �Ͽ� ���� ������ ����
	CSSkillNode* node = ch->m_sSkillList.m_head;
	CSSkill* sskill = NULL;

	// �ν��� ������ ��ų �˻� ���Ѵ�
	// �ν��� ������ �������� �� ��������
	if (!bBoost)
	{
		while (node)
		{
			if (node->m_sskill->m_proto->m_type == needskilltype)
			{
				if (sskill == NULL || sskill->m_level <= node->m_sskill->m_level)
					sskill = node->m_sskill;
			}
			node = node->m_next;
		}
		if (sskill == NULL || sskill->m_level < 0 || sskill->m_level >= SSKILL_MAX_LEVEL)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_PRODUCE_NO_SSKILL);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}

		// ������ �̿� Ƚ�� ����
		if (tool->m_itemProto->getItemMaxUse() > 0)
		{
			tool->setUsed(tool->getUsed() - 1);

			CNetMsg::SP rmsg(new CNetMsg);
			UpdateClient::makeUpdateWearItemUsed(rmsg, tool->getWearPos(), tool->getVIndex(), tool->getUsed());
			SEND_Q(rmsg, ch->m_desc);
		}
		else
		{
			// ���� �� �Ǿ��� ��
			// �����
			ch->m_wearInventory.RemoveItem((int)tool->getWearPos());
			{
				// �������� �����
				CNetMsg::SP rmsg(new CNetMsg);
				WearingMsg(rmsg, ch, WEARING_WEAPON, -1, 0);
				ch->m_pArea->SendToCell(rmsg, ch, false);
			}

			{
				CNetMsg::SP rmsg(new CNetMsg);
				SysMsg(rmsg, MSG_SYS_PRODUCE_MAXUSE_TOOL);
				SEND_Q(rmsg, ch->m_desc);
			}

			return;
		}
	}

	// npc ü�� ����
	npc->m_hp -= tool->m_itemProto->getItemNum0();

	if (npc->m_hp <= 0)
		npc->m_hp = 0;

	// ���ø���Ʈ�� �߰�
	AddAttackList(ch, npc, 0);

	// ���� ����Ʈ���� ������ ����
	target = npc->m_attackList;

	while (target)
	{
		if (target->ch == ch)
		{
			target->m_damage += tool->m_itemProto->getItemNum0();
			// ������ ���� �޽����� �Ź� �������ش�.
			target->m_targetPulse = gserver->m_pulse;
			break;
		}
		target = target->m_next;
	}

	// 5 ���� �� ����
	if (target->m_damage >= 5)
	{
		CItem* item = NULL;

		// �ν����ΰ�?
		if (bBoost)
		{
			// �ν��� ������
			// �ν��� ������ �г�Ƽ ���� �̺�Ʈ�� ����

			// ����� ����
			target->m_damage = 0;

			// ��޺� ȹ�� ����
			LONGLONG countOfGrade[5] = {150, 86, 55, 38, 30};

			// TODO : �ӽ� ����
			item = gserver->m_itemProtoList.CreateItem(itemdbindex, -1, 0, 0, countOfGrade[grade] / 2 * bBoost / 100 );
			if (item)
			{
				// ���� �� �Ǿ��� ��
				// �����
				ch->m_wearInventory.RemoveItem((int)tool->getWearPos());
				{
					// �������� �����
					CNetMsg::SP rmsg(new CNetMsg);
					WearingMsg(rmsg, ch, WEARING_WEAPON, -1, 0);
					ch->m_pArea->SendToCell(rmsg, ch, false);
				}

			}

			npc->m_hp = 0;
		}
		else
		{
			// �Ϲ� ������

			// ����� ���̱�
			target->m_damage -= 5;

			// ���� sskill���� Ư�� ����� ���� Ȯ�� ����
			int prob = 0;
			switch (grade)
			{
			case 0:
				prob = sskill->m_proto->m_num0[sskill->m_level - 1];
				break;	// E
			case 1:
				prob = sskill->m_proto->m_num1[sskill->m_level - 1];
				break;	// D
			case 2:
				prob = sskill->m_proto->m_num2[sskill->m_level - 1];
				break;	// C
			case 3:
				prob = sskill->m_proto->m_num3[sskill->m_level - 1];
				break;	// B
			case 4:
				prob = sskill->m_proto->m_num4[sskill->m_level - 1];
				break;	// A
			}

			// ������ ���� ���Ƽ
			if (npc->m_proto->CheckFlag(NPC_ETERNAL))
				prob = prob * ETERNAL_PENALTY_PROB / 100;

			if (npc->GetMapAttr() & MATT_PRODUCT_PUBLIC)
			{
				// ������ ��ε�
				prob = prob * PUBLIC_PENALTY_PROB / 100;
			}

			prob = prob * gserver->m_bDoubleProducePercent / 100;

			// ���� ����
			if (GetRandom(1, 10000) <= prob)
			{
				LONGLONG count = 1;
				// ���� �̺�Ʈ
				if (gserver->m_bDoubleEvent)
					count = gserver->m_bDoubleProduceNum;

				item = gserver->m_itemProtoList.CreateItem(itemdbindex, -1, 0, 0, count);

				if (npc->GetMapAttr() & MATT_PRODUCT_PRIVATE)
				{
					if (ch->m_inventory.getMoney() >= produceTax)
					{
						ch->m_inventory.decreaseMoney(produceTax);
					}
					gserver->AddTaxProduceCastle(produceTax / 3);
				}
			}
		}

		if (item && item->m_itemProto->getItemTypeIdx() == ITYPE_ETC && item->m_itemProto->getItemSubTypeIdx() == IETC_PRODUCT)
		{
			if (ch->m_inventory.addItem(item))
			{
				// Item LOG
				GAMELOG << init("ITEM_PICK_PRODUCE_SEL", ch)
						<< itemlog(item)
						<< end;

				switch (item->m_itemProto->getItemIndex())
				{
				case 152:
					STATISTICS(152, item->Count());
					break;
				case 153:
					STATISTICS(153, item->Count());
					break;
				case 154:
					STATISTICS(154, item->Count());
					break;
				case 155:
					STATISTICS(155, item->Count());
					break;
				case 156:
					STATISTICS(156, item->Count());
					break;
				case 157:
					STATISTICS(157, item->Count());
					break;
				case 158:
					STATISTICS(158, item->Count());
					break;
				case 159:
					STATISTICS(159, item->Count());
					break;
				case 160:
					STATISTICS(160, item->Count());
					break;
				case 161:
					STATISTICS(161, item->Count());
					break;
				case 162:
					STATISTICS(162, item->Count());
					break;
				case 163:
					STATISTICS(163, item->Count());
					break;
				case 197:
					STATISTICS(197, item->Count());
					break;
				case 198:
					STATISTICS(198, item->Count());
					break;
				case 199:
					STATISTICS(199, item->Count());
					break;
				}
			}
			else
			{
				// �����丮 ������ �� ���� �� Drop
				item = npc->m_pArea->DropItem(item, npc);
				if (!item)
					return;

				item->m_preferenceIndex = ch->m_index;
				{
					CNetMsg::SP rmsg(new CNetMsg);
					ItemDropMsg(rmsg, npc, item);
					npc->m_pArea->SendToCell(rmsg, npc, true);
				}
			}

			{
				// ���� ������ �ش��ϴ� ������ ���� ����Ʈ �޼��� ����
				CNetMsg::SP rmsg(new CNetMsg);
				switch (npcflag)
				{
				case NPC_MINERAL:
					EffectEtcMsg(rmsg, ch, MSG_EFFECT_ETC_PRODUCE_MINING);
					break;
				case NPC_ENERGY:
					EffectEtcMsg(rmsg, ch, MSG_EFFECT_ETC_PRODUCE_GATHERING);
					break;
				case NPC_CROPS:
					EffectEtcMsg(rmsg, ch, MSG_EFFECT_ETC_PRODUCE_CHARGE);
					break;
				}
				ch->m_pArea->SendToCell(rmsg, ch, true);
			}
		}
	}

	// ����
	if (npc->m_proto->CheckFlag(NPC_ETERNAL))
	{
		npc->m_hp = ch->m_maxHP;
	}

	{
		// ����Ʈ �޽��� ����
		CNetMsg::SP rmsg(new CNetMsg);
		switch (npcflag)
		{
		case NPC_MINERAL:
			EffectProduceMsg(rmsg, ch, npc, MSG_PRODUCE_MINING);
			break;
		case NPC_ENERGY:
			EffectProduceMsg(rmsg, ch, npc, MSG_PRODUCE_CHARGE);
			break;
		case NPC_CROPS:
			EffectProduceMsg(rmsg, ch, npc, MSG_PRODUCE_GATHERING);
			break;
		}
		ch->m_pArea->SendToCell(rmsg, ch, true);
	}

	// �׾�����
	if (DEAD(npc))
	{
		// ����
		if (npc->m_proto->CheckFlag(NPC_ETERNAL))
		{
			npc->m_hp = ch->m_maxHP;
		}

		// ������
		else
		{
			// ������ ���
			for (int loopcount = 0; loopcount < MAX_NPC_DROPITEM_LOOP; loopcount++)
			{
				// 1. ����� �� �ִ� �� �������� ������ ����
				// 2. �� �������� ���Ȯ���� ��ӿ��� ����
				// 3. ���
				if (npc->m_proto->m_itemCount > 0)
				{
					CItem* dropItem = NULL;
					int tableindex = GetRandom(0, MAX_NPC_DROPITEM - 1);

					if (npc->m_proto->m_item[tableindex] != -1 && GetRandom(1, 10000) <= npc->m_proto->m_itemPercent[tableindex])
						dropItem = npc->m_pArea->DropItem(npc->m_proto->m_item[tableindex], npc, 0, 0, 1, true);

					if (dropItem)
					{
						// Drop Msg ������
						dropItem->m_preferenceIndex = -1;
						CNetMsg::SP rmsg(new CNetMsg);
						ItemDropMsg(rmsg, npc, dropItem);
						npc->m_pArea->SendToCell(rmsg, npc);
					}
				}
			}

			// ���ø���Ʈ���� ����
			DelAttackList(npc);
			
			// npc ����
			npc->SendDisappearAllInCell( false );
			ch->m_pArea->CharFromCell(npc, true);
			ch->m_pArea->DelNPC(npc);
		}
	}
}

//////////////
// ������ ����
void do_RandomProduce(CPC* ch, CNetMsg::SP& msg)
{
	if (DEAD(ch))
		return ;

	if (ch->IsSetPlayerState(PLAYER_STATE_CHANGE))
		return;

	int npcindex;

	msg->MoveFirst();

	RefMsg(msg) >> npcindex;

	// ��ó�� NPC ã��
	CNPC* npc = TO_NPC(ch->m_pArea->FindCharInCell(ch, npcindex, MSG_CHAR_NPC, false));
	if (!npc)
		return ;

	// �ӵ� �˻�
	ch->m_hackProduceCount--;
	ch->m_ProduceClientTime += gserver->m_pulse - ch->m_lastProducePulse + PULSE_HACK_ATTACK_THRESHOLD;

	if (ch->m_hackProduceCount < 1)
	{
		if( ch->m_ProduceClientTime < PULSE_PRODUCE_DELAY * PULSE_HACK_ATTACK_COUNT)
		{
			GAMELOG << init("HACK PRODUCE SPEED", ch)
					<< (gserver->m_pulse - ch->m_lastProducePulse)
					<< end;

			ch->m_desc->IncreaseHackCount(0);
			return;
		}
		ch->m_hackProduceCount = PULSE_HACK_ATTACK_COUNT;	// ����޼��� �� �� ����
		ch->m_ProduceClientTime = 0; // Ŭ���̾�Ʈ�� ���� ����ð�
	}

	ch->m_lastProducePulse = gserver->m_pulse;

	// �Ÿ� �˻�
	if (GetDistance(ch, npc) > npc->m_proto->m_attackArea + 0.5)
	{
		GAMELOG << init("HACK PRODUCE DIST", ch)
				<< GetDistance(ch, npc)
				<< end;
		// ch->m_desc->IncreaseHackCount(6);
		return ;
	}

	// ������ DB �ε��� �˻�
	int npcflag = -1;			// NPC ����
	int needitemtype = -1;		// ���� ����
	int needskilltype = -1;		// ��ų ����

	if (npc->m_proto->CheckFlag(NPC_MINERAL))
	{
		npcflag = NPC_MINERAL;
		needitemtype = IWEAPON_MINING;
		needskilltype = SSKILL_MINING;
	}
	else if (npc->m_proto->CheckFlag(NPC_CROPS))
	{
		npcflag = NPC_CROPS;
		needitemtype = IWEAPON_GATHERING;
		needskilltype = SSKILL_GATHERING;
	}
	else if (npc->m_proto->CheckFlag(NPC_ENERGY))
	{
		npcflag = NPC_ENERGY;
		needitemtype = IWEAPON_CHARGE;
		needskilltype = SSKILL_CHARGE;
	}
	else
		return ;

	// ���� �˻�
	if (!ch->m_wearInventory.wearItemInfo[WEARING_WEAPON])
	{
		// �ƿ� ������ġ�� �ϰ͵� ����
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_PRODUCE_NOTOOL);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	CItem* tool = ch->m_wearInventory.wearItemInfo[WEARING_WEAPON];

	// ���� ���� Ÿ�� �˻�
	bool bProduceTool = false;
	switch (tool->m_itemProto->getItemTypeIdx())
	{
	case ITYPE_WEAPON:
		switch (tool->m_itemProto->getItemSubTypeIdx())
		{
		case IWEAPON_MINING:
		case IWEAPON_GATHERING:
		case IWEAPON_CHARGE:
			bProduceTool = true;
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}
	if (!bProduceTool)
	{
		// ���� ������ �ƴ�
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_PRODUCE_WEAR_TOOL);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}
	if (tool->m_itemProto->getItemSubTypeIdx() != needitemtype)
	{
		// �ش� ���� ������ �ƴ�
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_PRODUCE_MATCH_TOOL);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	// �ν��� ������ ��� �Ұ�
	if (tool->getFlag() & FLAG_ITEM_BOOSTER_ADDED
			|| tool->getFlag() & FLAG_ITEM_SILVERBOOSTER_ADDED
			|| tool->getFlag() & FLAG_ITEM_GOLDBOOSTER_ADDED
			|| tool->getFlag() & FLAG_ITEM_PLATINUMBOOSTER_ADDED
	   )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_PRODUCE_CANNT_BOOST);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	const int produceTax = 30;		// ����� �ʿ� ����

	// �ν��ʹ� �ݾ� �˻� ����, ������������
	if (npc->GetMapAttr() & MATT_PRODUCT_PRIVATE)
	{
		if (ch->m_inventory.getMoney() < produceTax)
		{
			// �� ��� �Ұ���
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_PRODUCE_NO_MONEY);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}
	}

// [101216: selo] ���� ������ ��� ����
	if( !ch->CheckInvenForProduceRandomPrize() )
	{
		return;
	}

	// ���� ���� ������ �˻�
	if (tool->m_itemProto->getItemMaxUse() != -1 &&  tool->getUsed() <= 0)
	{
		// ���� �� �Ǿ��� ��
		// �����
		ch->m_wearInventory.RemoveItem((int)tool->getWearPos());

		{
			// �������� �����
			CNetMsg::SP rmsg(new CNetMsg);
			WearingMsg(rmsg, ch, WEARING_WEAPON, -1, 0);
			ch->m_pArea->SendToCell(rmsg, ch, false);
		}

		{
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_PRODUCE_MAXUSE_TOOL);
			SEND_Q(rmsg, ch->m_desc);
		}

		return;
	}

	// ���� ���� �ο� üũ
	int nTarget = 0;
	CAttackChar* target = npc->m_attackList;
	bool bProduce = false;		// �̹��� ���� �޽������� ������ �������̴�

	while (target)
	{
		nTarget++;

		if (target->ch == ch)
			bProduce = true;

		target = target->m_next;
	}

	// ����޽��� ���������� �̹� �������̶�� �ο�üũ ���Ѵ�.
	if (!bProduce)
	{
		// ���� ���� �ο� �ʰ�
		if (nTarget >= MAX_ACCEPTABLE)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_PRODUCE_MAX_ACCEPTABLE);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}
	}

	// �������� Quest �� ���� npc�� �������̸� UpdateData
	CQuest* pQuest;
	CQuest* pQuestNext = ch->m_questList.GetNextQuest(NULL, QUEST_STATE_RUN);
	while ((pQuest = pQuestNext))
	{
		pQuestNext = ch->m_questList.GetNextQuest(pQuestNext, QUEST_STATE_RUN);
		// ���� ����Ʈ
		switch (pQuest->GetQuestType0())
		{
		case QTYPE_KIND_COLLECTION:
		case QTYPE_KIND_MINING_EXPERIENCE:
		case QTYPE_KIND_GATHERING_EXPERIENCE:
		case QTYPE_KIND_CHARGE_EXPERIENCE:
			pQuest->QuestUpdateData(ch, npc);
			break;
		default:
			break;
		}
	}

	// ��ų �˻�
	// �ε��� ū ���� �� ���� ������� �Ͽ� ���� ������ ����
	CSSkillNode* node = ch->m_sSkillList.m_head;
	CSSkill* sskill = NULL;

	while (node)
	{
		if (node->m_sskill->m_proto->m_type == needskilltype)
		{
			if (sskill == NULL || sskill->m_level <= node->m_sskill->m_level)
				sskill = node->m_sskill;
		}
		node = node->m_next;
	}
	if (sskill == NULL || sskill->m_level < 0 || sskill->m_level >= SSKILL_MAX_LEVEL)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_PRODUCE_NO_SSKILL);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	// ������ �̿� Ƚ�� ����
	if (tool->m_itemProto->getItemMaxUse() > 0)
	{
		tool->setUsed(tool->getUsed() - 1);

		CNetMsg::SP rmsg(new CNetMsg);
		UpdateClient::makeUpdateWearItemUsed(rmsg, tool->getWearPos(), tool->getVIndex(), tool->getUsed());
		SEND_Q(rmsg, ch->m_desc);
	}
	else
	{
		// ���� �� �Ǿ��� ��
		ch->m_wearInventory.RemoveItem((int)tool->getWearPos());
		{
			// �������� �����
			CNetMsg::SP rmsg(new CNetMsg);
			WearingMsg(rmsg, ch, WEARING_WEAPON, -1, 0);
			ch->m_pArea->SendToCell(rmsg, ch, false);
		}

		{
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_PRODUCE_MAXUSE_TOOL);
			SEND_Q(rmsg, ch->m_desc);
		}

		return;
	}

	// npc ü�� ����
	npc->m_hp -= tool->m_itemProto->getItemNum0();
	if (npc->m_hp <= 0)
		npc->m_hp = 0;

	// ���ø���Ʈ�� �߰�
	AddAttackList(ch, npc, 0);

	// ���� ����Ʈ���� ������ ����
	target = npc->m_attackList;

	while (target)
	{
		if (target->ch == ch)
		{
			target->m_damage += tool->m_itemProto->getItemNum0();
			// ������ ���� �޽����� �Ź� �������ش�.
			target->m_targetPulse = gserver->m_pulse;
			break;
		}
		target = target->m_next;
	}

	// 5 ���� �� ����
	if (target->m_damage >= 5)
	{
		CItem* item = NULL;

		// ����� ���̱�
		target->m_damage -= 5;

		// ��޺� Ȯ�� ���ϱ�
		int probProduct[MAX_PRODUCTS_FROM_NPC];
		int itemIndexProduct[MAX_PRODUCTS_FROM_NPC];
		memset(probProduct, 0, sizeof(int) * MAX_PRODUCTS_FROM_NPC);
		memset(itemIndexProduct, 0, sizeof(int) * MAX_PRODUCTS_FROM_NPC);
		for (int i = 0; i < MAX_PRODUCTS_FROM_NPC; i++)
		{
			if (npc->m_proto->m_product[i] != -1)
			{
				itemIndexProduct[i] = npc->m_proto->m_product[i];

				// ��� ���ϱ�
				int grade = GetGradeProductItem(itemIndexProduct[i]);
				if (grade != -1)
				{
					// ��ų���� Ȯ�� ���ϱ�
					switch (grade)
					{
					case 0:
						probProduct[i] = sskill->m_proto->m_num5[sskill->m_level - 1];
						break;	// E
					case 1:
						probProduct[i] = sskill->m_proto->m_num6[sskill->m_level - 1];
						break;	// D
					case 2:
						probProduct[i] = sskill->m_proto->m_num7[sskill->m_level - 1];
						break;	// C
					case 3:
						probProduct[i] = sskill->m_proto->m_num8[sskill->m_level - 1];
						break;	// B
					case 4:
						probProduct[i] = sskill->m_proto->m_num9[sskill->m_level - 1];
						break;	// A
					}

					// �г�Ƽ ���� : ������ ����
					if (npc->m_proto->CheckFlag(NPC_ETERNAL))
						probProduct[i] = probProduct[i] * ETERNAL_PENALTY_PROB / 100;

					// �г�Ƽ ���� : ������
					if (npc->GetMapAttr() & MATT_PRODUCT_PUBLIC)
						probProduct[i] = probProduct[i] * PUBLIC_PENALTY_PROB / 100;
				}
			}
		}

		// ���� Ȯ�� ���ϱ�
		for (int i = 1; i < MAX_PRODUCTS_FROM_NPC; i++)
		{
			probProduct[i] += probProduct[i - 1];
		}

		// ���� ����
		int prob = GetRandom(1, 10000);
		prob = prob * gserver->m_bDoubleProducePercent / 100;

		for (int i = 0; i < MAX_PRODUCTS_FROM_NPC; i++)
		{
			if (prob <= probProduct[i])
			{
				LONGLONG count = 1;

				// ���� �̺�Ʈ
				if (gserver->m_bDoubleEvent)
					count = gserver->m_bDoubleProduceNum;

				item = gserver->m_itemProtoList.CreateItem(itemIndexProduct[i], -1, 0, 0, count);

				if (npc->GetMapAttr() & MATT_PRODUCT_PRIVATE)
				{
					if (ch->m_inventory.getMoney() >= produceTax)
					{
						ch->m_inventory.decreaseMoney(produceTax);
					}
					gserver->AddTaxProduceCastle(produceTax / 3);
				}
				break ;
			}
		}

		if (item && item->m_itemProto->getItemTypeIdx() == ITYPE_ETC && item->m_itemProto->getItemSubTypeIdx() == IETC_PRODUCT)
		{
			if (ch->m_inventory.addItem(item))
			{
				// Item LOG
				GAMELOG << init("ITEM_PICK_PRODUCE_RANDOM", ch)
						<< itemlog(item)
						<< end;

				switch (item->m_itemProto->getItemIndex())
				{
				case 152:
					STATISTICS(152, item->Count());
					break;
				case 153:
					STATISTICS(153, item->Count());
					break;
				case 154:
					STATISTICS(154, item->Count());
					break;
				case 155:
					STATISTICS(155, item->Count());
					break;
				case 156:
					STATISTICS(156, item->Count());
					break;
				case 157:
					STATISTICS(157, item->Count());
					break;
				case 158:
					STATISTICS(158, item->Count());
					break;
				case 159:
					STATISTICS(159, item->Count());
					break;
				case 160:
					STATISTICS(160, item->Count());
					break;
				case 161:
					STATISTICS(161, item->Count());
					break;
				case 162:
					STATISTICS(162, item->Count());
					break;
				case 163:
					STATISTICS(163, item->Count());
					break;
				case 197:
					STATISTICS(197, item->Count());
					break;
				case 198:
					STATISTICS(198, item->Count());
					break;
				case 199:
					STATISTICS(199, item->Count());
					break;
				}
			}
			else
			{
				// �����丮 ������ �� ���� �� Drop
				item = npc->m_pArea->DropItem(item, npc);
				if (!item)
					return;

				item->m_preferenceIndex = ch->m_index;

				{
					CNetMsg::SP rmsg(new CNetMsg);
					ItemDropMsg(rmsg, npc, item);
					npc->m_pArea->SendToCell(rmsg, npc, true);
				}
			}

			{
				// ���� ������ �ش��ϴ� ������ ���� ����Ʈ �޼��� ����
				CNetMsg::SP rmsg(new CNetMsg);
				switch (npcflag)
				{
				case NPC_MINERAL:
					EffectEtcMsg(rmsg, ch, MSG_EFFECT_ETC_PRODUCE_MINING);
					break;
				case NPC_ENERGY:
					EffectEtcMsg(rmsg, ch, MSG_EFFECT_ETC_PRODUCE_GATHERING);
					break;
				case NPC_CROPS:
					EffectEtcMsg(rmsg, ch, MSG_EFFECT_ETC_PRODUCE_CHARGE);
					break;
				}
				ch->m_pArea->SendToCell(rmsg, ch, true);
			}
		}
		else
		{
			if (item)
				delete item;
		}
	}

	// ����
	if (npc->m_proto->CheckFlag(NPC_ETERNAL))
	{
		npc->m_hp = ch->m_maxHP;
	}

	{
		// ����Ʈ �޽��� ����
		CNetMsg::SP rmsg(new CNetMsg);
		switch (npcflag)
		{
		case NPC_MINERAL:
			EffectProduceMsg(rmsg, ch, npc, MSG_PRODUCE_MINING);
			break;
		case NPC_ENERGY:
			EffectProduceMsg(rmsg, ch, npc, MSG_PRODUCE_CHARGE);
			break;
		case NPC_CROPS:
			EffectProduceMsg(rmsg, ch, npc, MSG_PRODUCE_GATHERING);
			break;
		}
		ch->m_pArea->SendToCell(rmsg, ch, true);
	}

	// �׾�����
	if (DEAD(npc))
	{
		if (npc->m_proto->CheckFlag(NPC_ETERNAL)) // ����
		{
			npc->m_hp = ch->m_maxHP;
		}
		else // ������
		{
			// ������ ���
			for (int loopcount = 0; loopcount < MAX_NPC_DROPITEM_LOOP; loopcount++)
			{
				// 1. ����� �� �ִ� �� �������� ������ ����
				// 2. �� �������� ���Ȯ���� ��ӿ��� ����
				// 3. ���
				if (npc->m_proto->m_itemCount > 0)
				{
					CItem* dropItem = NULL;
					int tableindex = GetRandom(0, MAX_NPC_DROPITEM - 1);

					if (npc->m_proto->m_item[tableindex] != -1 && GetRandom(1, 10000) <= npc->m_proto->m_itemPercent[tableindex])
						dropItem = npc->m_pArea->DropItem(npc->m_proto->m_item[tableindex], npc, 0, 0, 1, true);

					if (dropItem)
					{
						// Drop Msg ������
						dropItem->m_preferenceIndex = -1;
						CNetMsg::SP rmsg(new CNetMsg);
						ItemDropMsg(rmsg, npc, dropItem);
						npc->m_pArea->SendToCell(rmsg, npc);
					}
				}
			}

			// ���ø���Ʈ���� ����
			DelAttackList(npc);

			// npc ����
			ch->m_pArea->CharFromCell(npc, true);
			ch->m_pArea->DelNPC(npc);
		}
	}
}

