#include "stdhdrs.h"

#include "Log.h"
#include "Character.h"
#include "Server.h"
#include "CmdMsg.h"
#include "doFunc.h"
#include "SocketSystem.h"
#include "../ShareLib/packetType/ptype_old_do_exsocket.h"

void do_FuncExSocket(CPC* ch, CNetMsg::SP& msg)
{
	unsigned char subtype;
	RefMsg(msg) >> subtype;

	switch(subtype)
	{
	case MSG_EX_SOCKET_UPGRADE_JEWEL_REQ:
		do_ExSocketUpgradeJewelReq(ch, msg);
		break;
	case MSG_EX_SOCKET_UPGRADE_JEWEL_CHAOS_REQ:
		do_ExSocketUpgradeChaosJewelReq(ch, msg);
		break;
	case MSG_EX_SOCKET_MAKE_REQ:
		do_ExSocketMakeReq(ch, msg);
		break;
	case MSG_EX_SOCKET_COMBINE_JEWEL_REQ:
		do_ExSocketCombineJewelReq(ch, msg);
		break;
	case MSG_EX_SOCKET_CLEAN_JEWEL_REQ:
		do_ExSocketCleanJewel(ch, msg);
		break;
	case MSG_EX_SOCKET_CLEAN_ONE_JEWEL_REQ:
		do_ExSocketCleanOneJewel(ch, msg);
		break;
	}
}

void do_ExSocketUpgradeJewelReq(CPC* ch, CNetMsg::SP& msg)
{
	static const int nMaxJewelGrade = 3;

	int jewelindex = 0;

	RequestClient::doExSocketUpgradeJewel* packet = reinterpret_cast<RequestClient::doExSocketUpgradeJewel*>(msg->m_buf);

	{
		std::set<int> checkdup;
		int key = 0;

		key = makeCheckDupKey(packet->tab_1, packet->invenIndex_1);
		checkdup.insert(key);
		key = makeCheckDupKey(packet->tab_2, packet->invenIndex_2);
		checkdup.insert(key);
		key = makeCheckDupKey(packet->tab_3, packet->invenIndex_3);
		checkdup.insert(key);

		if (packet->itemCount > 0)
		{
			key = makeCheckDupKey(packet->tab_4, packet->invenIndex_4);
			if (checkdup.insert(key).second == false)
			{
				LOG_ERROR("HACKING? : dup index. charIndex[%d]", ch->m_index);
				ch->m_desc->Close("dup index");
				return;
			}
		}
	}

	CNPC* npc = (CNPC *)ch->m_pArea->FindCharInCell(ch, packet->npcIndex, MSG_CHAR_NPC);
	if (npc == NULL)
	{
		LOG_ERROR("HACKING? : not found npc. char_index[%d] npcIndex[%d]", ch->m_index, packet->npcIndex);
		ch->m_desc->Close("not found npc");
		return;
	}

	if (!npc->IsFlag(NPC_SHOPPER) )
	{
		LOG_ERROR("HACKING? : not exist npc. char_index[%d] npcIndex[%d]", ch->m_index, packet->npcIndex);
		ch->m_desc->Close("not exist npc");
		return ;
	}

	if (ch->m_inventory.isValidNormalInventory(packet->tab_1, packet->invenIndex_1) == false)
	{
		LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_1, packet->invenIndex_1);
		ch->m_desc->Close("invalid packet");
		return;
	}

	if (ch->m_inventory.isValidNormalInventory(packet->tab_2, packet->invenIndex_2) == false)
	{
		LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_2, packet->invenIndex_2);
		ch->m_desc->Close("invalid packet");
		return;
	}

	if (ch->m_inventory.isValidNormalInventory(packet->tab_3, packet->invenIndex_3) == false)
	{
		LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_3, packet->invenIndex_3);
		ch->m_desc->Close("invalid packet");
		return;
	}

	// ������ 1,2,3 Ÿ�� �˻�
	// 1. �ɼ� ����, 2. �ɼ� ����, 3. ���� ����
	CItem* item1 = ch->m_inventory.getItem(packet->tab_1, packet->invenIndex_1);
	if (item1 == NULL)
	{
		LOG_ERROR("HACKING? : not found item. charIndex[%d] tab[%d] invenIndex[%d]", ch->m_index, packet->tab_1, packet->invenIndex_1);
		ch->m_desc->Close("not found item");
		return;
	}
	CItem* item2 = ch->m_inventory.getItem(packet->tab_2, packet->invenIndex_2);
	CItem* item3 = ch->m_inventory.getItem(packet->tab_3, packet->invenIndex_3);
	CItem* item4 = NULL;

	//������ ������ �˻�
	if (packet->itemCount > 0)
	{
		if (ch->m_inventory.isValidNormalInventory(packet->tab_4, packet->invenIndex_4) == false)
		{
			LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
					  ch->m_index, packet->tab_4, packet->invenIndex_4);
			ch->m_desc->Close("invalid packet");
			return;
		}

		item4 = ch->m_inventory.getItem(packet->tab_4, packet->invenIndex_4);
		if (item4 == NULL)
		{
			LOG_ERROR("HACKING? : not found item. char_index[%d] tab[%d] invenIndex[%d]",
					  ch->m_index, packet->tab_4, packet->invenIndex_4);
			ch->m_desc->Close("not found item");
			return;
		}

		if(item4->getVIndex() != packet->virualIndex_4 || !item4->IsFuncType() || item4->Count() < packet->itemCount)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_ITEM_MISSMATCH);
			SEND_Q(rmsg, ch->m_desc);

			GAMELOG << init("JEWEL UPGRADE FAIL: ITEM MISSMATCH", ch)
					<< packet->virualIndex_1 << delim
					<< packet->virualIndex_2 << delim
					<< packet->virualIndex_3 << delim
					<< packet->virualIndex_4 << end;
			return ;
		}
	}

	//���� ������ �˻�
	if(!item1 || !item2 || !item3 ||
			item1->getVIndex() != packet->virualIndex_1 || item2->getVIndex() != packet->virualIndex_2 || item3->getVIndex() != packet->virualIndex_3 ||
			!item1->IsJewelType() || !item2->IsJewelType() || !item3->IsJewelType()  )
	{
		// �ùٸ� �������� �ƴմϴ�.
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_ITEM_MISSMATCH);
		SEND_Q(rmsg, ch->m_desc);

		GAMELOG << init("JEWEL UPGRADE FAIL: ITEM MISSMATCH", ch)
				<< packet->virualIndex_1 << delim
				<< packet->virualIndex_2 << delim
				<< packet->virualIndex_3 << delim
				<< packet->virualIndex_4 << end;
		return ;
	}

	//���� ���� üũ (�ߺ��� ������ �ö���� ����� ���� ó��)
	if(item1 == item2 && item1 == item3) 	//���� 3���� ��� ���� ���
	{
		if(item1->Count() < 3)
		{
			LOG_ERROR("HACKING? : invalid count. charIndex[%d]", ch->m_index);
			ch->m_desc->Close("invalid count");
			return;
		}
	}
	else if(item1 == item2 || item1 == item3) //2���� ���� ������ ���1
	{
		if(item1->Count() < 2)
		{
			LOG_ERROR("HACKING? : invalid count. charIndex[%d]", ch->m_index);
			ch->m_desc->Close("invalid count");
			return;
		}
	}
	else if(item2 == item3) //2���� ���� ������ ���2
	{
		if(item2->Count() < 2)
		{
			LOG_ERROR("HACKING? : invalid count. charIndex[%d]", ch->m_index);
			ch->m_desc->Close("invalid count");
			return;
		}
	}
	else if(item1->Count() < 0 && item2->Count() < 0 && item3->Count() < 0) //3���� ��� �ٸ� ������ ���
	{
		LOG_ERROR("HACKING? : invalid count. charIndex[%d]", ch->m_index);
		ch->m_desc->Close("invalid count");
		return;
	}

	//�� üũ
	int jewelGrade = (item1->m_itemProto->getItemNum0() + item2->m_itemProto->getItemNum0() + item3->m_itemProto->getItemNum0()) / 3 ;	//���� 3���� ��� ���
	CJewelData* jewelData = gserver->m_jewelDataList.FindGrade(jewelGrade);
	if (jewelData == NULL)
	{
		GAMELOG << init("JEWEL UPGRADE ERROR: ITEM MISSMATCH", ch)
				<< "jewelGrade " << jewelGrade << end;
		return;
	}

	GoldType_t needNas = jewelData->getNormalComposeMoney();

	if(ch->m_inventory.getMoney() < needNas)
	{
		//�� ����
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_NOMONEY);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	// ����� �˻�
	if(ch->m_inventory.getEmptyCount() < 1)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_INVEN_NOSPACE);
		SEND_Q(rmsg, ch->m_desc);

		if (packet->itemCount <= 0)
		{
			GAMELOG << init("JEWEL UPGRADE FAIL: NO SPACE", ch)
					<< item1->getDBIndex() << delim
					<< item2->getDBIndex() << delim
					<< item3->getDBIndex() << end;
		}
		else
		{
			GAMELOG << init("JEWEL UPGRADE FAIL: NO SPACE", ch)
					<< item1->getDBIndex() << delim
					<< item2->getDBIndex() << delim
					<< item3->getDBIndex() << delim
					<< item4->getDBIndex() << end;
		}
		return ;
	}

	ch->m_inventory.decreaseMoney(needNas);

	bool isChaos = false;

	{
		int makeChaosRand = jewelData->getComposeNormalToChaosProb() * gserver->m_jewelDataList.m_jewelData->getEventCreateChaosProb() / 100;
		int rand = GetRandom(1, 10000);
		if(rand < makeChaosRand)
			isChaos = true;		//ī���� ���� ������
	}

	{
		int rand = GetRandom(1, 10000);
		int maxrand;
		if (packet->itemCount > 0)
		{
			if(isChaos)
				maxrand = jewelData->getChaosComposeProb() * (100 + item4->m_itemProto->getItemNum3() * packet->itemCount) / 100 * gserver->m_jewelDataList.m_jewelData->getEventComposeProb() / 100;
			else
				maxrand = jewelData->getNormalComposeProb() * (100 + item4->m_itemProto->getItemNum3() * packet->itemCount) / 100 * gserver->m_jewelDataList.m_jewelData->getEventComposeProb() / 100;
		}
		else
		{
			if(isChaos)
				maxrand = jewelData->getChaosComposeProb() * gserver->m_jewelDataList.m_jewelData->getEventComposeProb() / 100;
			else
				maxrand = jewelData->getNormalComposeProb() * gserver->m_jewelDataList.m_jewelData->getEventComposeProb() / 100;
		}
		//isChaos = true;
		if(rand < maxrand)		//���� �ռ� ���� ��
		{
			//���� �ռ��� �����ϸ� + ��� ���� �����Ѵ�.
			int plus2, plus3;
			if(isChaos)
			{
				if (packet->itemCount > 0)
				{
					plus2 = jewelData->getChaosPlus2Prob() * (100 + (item4->m_itemProto->getItemNum3() * packet->itemCount)) / 100 * gserver->m_jewelDataList.m_jewelData->getEventUpgradeProb() / 100;
					plus3 = jewelData->getChaosPlus3Prob() * (100 + (item4->m_itemProto->getItemNum3() * packet->itemCount)) / 100 * gserver->m_jewelDataList.m_jewelData->getEventUpgradeProb() / 100;
				}
				else
				{
					plus2 = jewelData->getChaosPlus2Prob() * gserver->m_jewelDataList.m_jewelData->getEventUpgradeProb() / 100;
					plus3 = jewelData->getChaosPlus3Prob() * gserver->m_jewelDataList.m_jewelData->getEventUpgradeProb() / 100;
				}
			}
			else
			{
				if (packet->itemCount > 0)
				{
					plus2 = jewelData->getNormalPlus2Prob() * (100 + (item4->m_itemProto->getItemNum3() * packet->itemCount)) / 100 * gserver->m_jewelDataList.m_jewelData->getEventUpgradeProb() / 100;
					plus3 = jewelData->getNormalPlus3Prob() * (100 + (item4->m_itemProto->getItemNum3() * packet->itemCount)) / 100 * gserver->m_jewelDataList.m_jewelData->getEventUpgradeProb() / 100;
				}
				else
				{
					plus2 = jewelData->getNormalPlus2Prob() * gserver->m_jewelDataList.m_jewelData->getEventUpgradeProb() / 100;
					plus3 = jewelData->getNormalPlus3Prob() * gserver->m_jewelDataList.m_jewelData->getEventUpgradeProb() / 100;
				}
			}

			if(plus3 > 1000)	plus3 = 1000;
			if(plus2 > 4000)	plus2 = 4000;

			rand = GetRandom(1, 10000);



			if (rand < plus3)
			{
				//3 ���
				jewelGrade += 3;
				if(jewelGrade > JEWEL_MAX_LEVEL)
					jewelGrade = JEWEL_MAX_LEVEL;
			}
			else if(rand < plus2)
			{
				//2 ���
				jewelGrade += 2;
				if(jewelGrade > JEWEL_MAX_LEVEL)
					jewelGrade = JEWEL_MAX_LEVEL;
			}
			else
			{
				//1 ���
				jewelGrade += 1;
				if(jewelGrade > JEWEL_MAX_LEVEL)
					jewelGrade = JEWEL_MAX_LEVEL;
			}
		}
		else					//���� �ռ� ���� ��
		{
			//���� �ռ��� �����ϸ� - ��� ���� �����Ѵ�.
			int minus0, minus1, minus2, minus3;
			if(isChaos)
			{
				minus1 = jewelData->getChaosMinus1Prob();
				minus2 = jewelData->getChaosMinus2Prob();
				minus3 = jewelData->getChaosMinus3Prob();
				minus0 = 10000 - minus1 - minus2 - minus3;
			}
			else
			{
				minus1 = jewelData->getNormalMinus1Prob();
				minus2 = jewelData->getNormalMinus2Prob();
				minus3 = jewelData->getNormalMinus3Prob();
				minus0 = 10000 - minus1 - minus2 - minus3;
			}

			rand = GetRandom(1, 10000);

			if(rand < minus3)
			{
				//3 ����
				jewelGrade -= 3;
				if(jewelGrade < 1)
					jewelGrade = 1;
			}
			else if(rand < minus2)
			{
				jewelGrade -= 2;
				if(jewelGrade < 1)
					jewelGrade = 1;
			}
			else if(rand < minus0)
			{
				//����
			}
			else
			{
				jewelGrade -= 1;
				if(jewelGrade < 1)
					jewelGrade = 1;
			}
		}
		int rand_jewel;
		while(jewelindex <= 0)
		{
			if(isChaos)
			{
				rand_jewel = GetRandom(1, gserver->m_jewelDataList.m_nJewelItemKindCount_Chaos);
				jewelindex = gserver->m_jewelDataList.getRandomChaosJewel(jewelGrade, rand_jewel);
			}
			else
			{
				rand_jewel = GetRandom(1, gserver->m_jewelDataList.m_nJewelItemKindCount);
				jewelindex = gserver->m_jewelDataList.getRandomJewel(jewelGrade, rand_jewel);
			}
		}
	}

	// ���ο� ���� ����
	CItem* pJewel = gserver->m_itemProtoList.CreateItem(jewelindex, -1, 0, 0, 1);
	if(!pJewel)
	{
		LOG_INFO("JewelUpgrade System. Not Found Item... jewelGrade : %d, jewelindex : %d", jewelGrade, jewelindex);
		return ;
	}

	if (ch->m_inventory.addItem(pJewel))
	{
		GAMELOG << init("JEWEL UPGRADE SUCCESS", ch)
				<< item1->getDBIndex() << delim
				<< item2->getDBIndex() << delim
				<< item3->getDBIndex() << delim;

		if (packet->itemCount > 0)
		{
			GAMELOG << item4->getDBIndex() << delim
					<< packet->itemCount << delim;
		}

		GAMELOG << jewelindex << end;

		ch->m_inventory.decreaseItemCount(item1, 1);
		ch->m_inventory.decreaseItemCount(item2, 1);
		ch->m_inventory.decreaseItemCount(item3, 1);

		// ������ �����
		if (packet->itemCount > 0)
		{
			ch->m_inventory.decreaseItemCount(item4, packet->itemCount);
		}

		//MSG_EX_SOCKET_UPGRADE_JEWEL_REP
		CNetMsg::SP rmsg(new CNetMsg);
		SocketMsg(rmsg, MSG_EX_SOCKET_UPGRADE_JEWEL_REP);
		RefMsg(rmsg) << pJewel->getVIndex();

		SEND_Q(rmsg, ch->m_desc);
	}
	else
	{
		GAMELOG << init("JEWEL UPGRADE FAIL: INVENTORY", ch)
				<< item1->getDBIndex() << delim
				<< item2->getDBIndex() << delim
				<< item3->getDBIndex() << delim;

		if (packet->itemCount > 0)
		{
			GAMELOG << item4->getDBIndex() << delim
					<< packet->itemCount << delim;
		}
		GAMELOG << jewelindex << end;

		delete pJewel;

		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_INVEN_NOSPACE);
		SEND_Q(rmsg, ch->m_desc);
	}
}

void do_ExSocketUpgradeChaosJewelReq(CPC* ch, CNetMsg::SP& msg)
{
	static const int nMaxJewelGrade = 3;
	int jewelindex = 0;

	RequestClient::doExSocketUpgradeChaosJewel* packet = reinterpret_cast<RequestClient::doExSocketUpgradeChaosJewel*>(msg->m_buf);

	{
		std::set<int> checkdup;
		int key = 0;

		key = makeCheckDupKey(packet->tab_1, packet->invenIndex_1);
		checkdup.insert(key);
		key = makeCheckDupKey(packet->tab_2, packet->invenIndex_2);
		checkdup.insert(key);
		key = makeCheckDupKey(packet->tab_3, packet->invenIndex_3);
		checkdup.insert(key);

		if (packet->itemCount > 0)
		{
			key = makeCheckDupKey(packet->tab_4, packet->invenIndex_4);
			if (checkdup.insert(key).second == false)
			{
				LOG_ERROR("HACKING? : dup index. charIndex[%d]", ch->m_index);
				ch->m_desc->Close("dup index");
				return;
			}
		}
	}

	CNPC* npc = (CNPC *)ch->m_pArea->FindCharInCell(ch, packet->npcIndex, MSG_CHAR_NPC);
	if (npc == NULL)
	{
		LOG_ERROR("HACKING? : not found npc. char_index[%d] npcIndex[%d]", ch->m_index, packet->npcIndex);
		ch->m_desc->Close("not found npc");
		return;
	}

	if (!npc->IsFlag(NPC_SHOPPER) )
	{
		LOG_ERROR("HACKING? : not exist npc. char_index[%d] npcIndex[%d]", ch->m_index, packet->npcIndex);
		ch->m_desc->Close("not exist npc");
		return ;
	}

	if (ch->m_inventory.isValidNormalInventory(packet->tab_1, packet->invenIndex_1) == false)
	{
		LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_1, packet->invenIndex_1);
		ch->m_desc->Close("invalid packet");
		return;
	}

	if (ch->m_inventory.isValidNormalInventory(packet->tab_2, packet->invenIndex_2) == false)
	{
		LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_2, packet->invenIndex_2);
		ch->m_desc->Close("invalid packet");
		return;
	}

	if (ch->m_inventory.isValidNormalInventory(packet->tab_3, packet->invenIndex_3) == false)
	{
		LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_3, packet->invenIndex_3);
		ch->m_desc->Close("invalid packet");
		return;
	}

	// ������ 1,2,3 Ÿ�� �˻�
	// 1. �ɼ� ����, 2. �ɼ� ����, 3. ���� ����

	CItem* item1 = ch->m_inventory.getItem(packet->tab_1, packet->invenIndex_1);
	CItem* item2 = ch->m_inventory.getItem(packet->tab_2, packet->invenIndex_2);
	CItem* item3 = ch->m_inventory.getItem(packet->tab_3, packet->invenIndex_3);
	CItem* item4 = NULL;

	//������ ������ �˻�
	if (packet->itemCount > 0)
	{
		if (ch->m_inventory.isValidNormalInventory(packet->tab_4, packet->invenIndex_4) == false)
		{
			LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
					  ch->m_index, packet->tab_4, packet->invenIndex_4);
			ch->m_desc->Close("invalid packet");
			return;
		}

		item4 = ch->m_inventory.getItem(packet->tab_4, packet->invenIndex_4);
		if (item4 == NULL)
		{
			LOG_ERROR("HACKING? : not found item. char_index[%d] tab[%d] invenIndex[%d]",
					  ch->m_index, packet->tab_4, packet->invenIndex_4);
			ch->m_desc->Close("not found item");
			return;
		}

		if(item4->getVIndex() != packet->virualIndex_4 || !item4->IsFuncType() || item4->Count() < packet->itemCount)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_ITEM_MISSMATCH);
			SEND_Q(rmsg, ch->m_desc);

			GAMELOG << init("CHAOS JEWEL UPGRADE FAIL: ITEM MISSMATCH", ch)
					<< packet->virualIndex_1 << delim
					<< packet->virualIndex_2 << delim
					<< packet->virualIndex_3 << delim
					<< packet->virualIndex_4 << end;
			return ;
		}
	}

	//���� ������ �˻�
	if(!item1 || !item2 || !item3 ||
			item1->getVIndex() != packet->virualIndex_1 || item2->getVIndex() != packet->virualIndex_2 || item3->getVIndex() != packet->virualIndex_3 ||
			!item1->IsChaosJewelType() || !item2->IsChaosJewelType() || !item3->IsChaosJewelType()  )
	{
		// �ùٸ� �������� �ƴմϴ�.
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_ITEM_MISSMATCH);
		SEND_Q(rmsg, ch->m_desc);

		GAMELOG << init("CHAOS JEWEL UPGRADE FAIL: ITEM MISSMATCH", ch)
				<< packet->virualIndex_1 << delim
				<< packet->virualIndex_2 << delim
				<< packet->virualIndex_3 << delim
				<< packet->virualIndex_4 << end;
		return ;
	}

	//���� ���� üũ (�ߺ��� ������ �ö���� ����� ���� ó��)
	if(item1 == item2 && item1 == item3) 	//���� 3���� ��� ���� ���
	{
		if(item1->Count() < 3)
		{
			LOG_ERROR("HACKING? : invalid count. charIndex[%d]", ch->m_index);
			ch->m_desc->Close("invalid count");
			return;
		}
	}
	else if(item1 == item2 || item1 == item3) //2���� ���� ������ ���1
	{
		if(item1->Count() < 2)
		{
			LOG_ERROR("HACKING? : invalid count. charIndex[%d]", ch->m_index);
			ch->m_desc->Close("invalid count");
			return;
		}
	}
	else if(item2 == item3) //2���� ���� ������ ���2
	{
		if(item2->Count() < 2)
		{
			LOG_ERROR("HACKING? : invalid count. charIndex[%d]", ch->m_index);
			ch->m_desc->Close("invalid count");
			return;
		}
	}
	else if(item1->Count() < 0 && item2->Count() < 0 && item3->Count() < 0) //3���� ��� �ٸ� ������ ���
	{
		LOG_ERROR("HACKING? : invalid count. charIndex[%d]", ch->m_index);
		ch->m_desc->Close("invalid count");
		return;
	}

	// ����� �˻�
	if (ch->m_inventory.getEmptyCount() < 1)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_INVEN_NOSPACE);
		SEND_Q(rmsg, ch->m_desc);

		if (packet->itemCount <= 0)
		{
			GAMELOG << init("JEWEL UPGRADE FAIL: NO SPACE", ch)
					<< item1->getDBIndex() << delim
					<< item2->getDBIndex() << delim
					<< item3->getDBIndex() << end;
		}
		else
		{
			GAMELOG << init("JEWEL UPGRADE FAIL: NO SPACE", ch)
					<< item1->getDBIndex() << delim
					<< item2->getDBIndex() << delim
					<< item3->getDBIndex() << delim
					<< item4->getDBIndex() << end;
		}
		return ;
	}

	//�� üũ
	int jewelGrade = (item1->m_itemProto->getItemNum0() + item2->m_itemProto->getItemNum0() + item3->m_itemProto->getItemNum0()) / 3 ;	//���� 3���� ��� ���
	CJewelData* jewelData = gserver->m_jewelDataList.FindGrade(jewelGrade);
	if (jewelData == NULL)
	{
		GAMELOG << init("JEWEL UPGRADE ERROR: ITEM MISSMATCH", ch)
				<< "jewelGrade " << jewelGrade << end;
		return;
	}

	GoldType_t needNas = jewelData->getChaosComposeMoney();

	if(ch->m_inventory.getMoney() < needNas)
	{
		//�� ����
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_NOMONEY);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	ch->m_inventory.decreaseMoney(needNas);

	//Ȯ�� ����
	//TODO	: 1�ܰ� ���� ���� ���� ���� üũ
	//		: 2�ܰ� ����, ���н� ��� ����,���� ���� ����
	//		: 3�ܰ� ���� ���� ����

	{
		int rand = GetRandom(1, 10000);
		int maxrand;
		if (packet->itemCount > 0)
		{
			maxrand = jewelData->getChaosComposeProb() * (100 + item4->m_itemProto->getItemNum3() * packet->itemCount) / 100 * gserver->m_jewelDataList.m_jewelData->getEventComposeProb() / 100;
		}
		else
			maxrand = jewelData->getChaosComposeProb() * gserver->m_jewelDataList.m_jewelData->getEventComposeProb() / 100;

		if(rand < maxrand)		//���� �ռ� ���� ��
		{
			//���� �ռ��� �����ϸ� + ��� ���� �����Ѵ�.
			int plus2;
			int plus3;

			if (packet->itemCount > 0)
			{
				plus2 = jewelData->getChaosPlus2Prob() * (100 + (item4->m_itemProto->getItemNum3() * packet->itemCount)) / 100 * gserver->m_jewelDataList.m_jewelData->getEventUpgradeProb() / 100;
				plus3 = jewelData->getChaosPlus3Prob() * (100 + (item4->m_itemProto->getItemNum3() * packet->itemCount)) / 100 * gserver->m_jewelDataList.m_jewelData->getEventUpgradeProb() / 100;
			}
			else
			{
				plus2 = jewelData->getChaosPlus2Prob() * gserver->m_jewelDataList.m_jewelData->getEventUpgradeProb() / 100;
				plus3 = jewelData->getChaosPlus3Prob() * gserver->m_jewelDataList.m_jewelData->getEventUpgradeProb() / 100;
			}

			if(plus3 > 1000)	plus3 = 1000;
			if(plus2 > 4000)	plus2 = 4000;

			rand = GetRandom(1, 10000);

			if(rand < plus3)
			{
				//3 ���
				jewelGrade += 3;
				if(jewelGrade > JEWEL_MAX_LEVEL)
					jewelGrade = JEWEL_MAX_LEVEL;
			}
			else if(rand < plus2)
			{
				//2 ���
				jewelGrade += 2;
				if(jewelGrade > JEWEL_MAX_LEVEL)
					jewelGrade = JEWEL_MAX_LEVEL;
			}
			else
			{
				//1 ���
				jewelGrade += 1;
				if(jewelGrade > JEWEL_MAX_LEVEL)
					jewelGrade = JEWEL_MAX_LEVEL;
			}
		}
		else					//���� �ռ� ���� ��
		{
			//���� �ռ��� �����ϸ� - ��� ���� �����Ѵ�.
			int minus1 = jewelData->getChaosMinus1Prob();
			int minus2 = jewelData->getChaosMinus2Prob();
			int minus3 = jewelData->getChaosMinus3Prob();
			int minus0 = 10000 - minus1 - minus2 - minus3;

			rand = GetRandom(1, 10000);

			if(rand < minus3)
			{
				//3 ����
				jewelGrade -= 3;
				if(jewelGrade < 1)
					jewelGrade = 1;
			}
			else if(rand < minus2)
			{
				//2 ����
				jewelGrade -= 2;
				if(jewelGrade < 1)
					jewelGrade = 1;
			}
			else if(rand < minus0)
			{
			}
			else
			{
				//1 ����
				jewelGrade -= 1;
				if(jewelGrade < 1)
					jewelGrade = 1;
			}
		}

		int rand_jewel;
		while(jewelindex <= 0)
		{
			rand_jewel = GetRandom(1, gserver->m_jewelDataList.m_nJewelItemKindCount_Chaos);
			if(jewelGrade > JEWEL_MAX_LEVEL)
				jewelGrade = JEWEL_MAX_LEVEL;
			jewelindex = gserver->m_jewelDataList.getRandomChaosJewel(jewelGrade, rand_jewel);
		}
	}

	// ���ο� ���� ����
	CItem* pJewel = gserver->m_itemProtoList.CreateItem(jewelindex, -1, 0, 0, 1);
	if(!pJewel)
	{
		LOG_INFO("JewelUpgrade System. Not Found Item... jewelGrade : %d, jewelindex : %d", jewelGrade, jewelindex);
		return ;
	}

	if (ch->m_inventory.addItem(pJewel))
	{
		GAMELOG << init("JEWEL UPGRADE SUCCESS", ch)
				<< item1->getDBIndex() << delim
				<< item2->getDBIndex() << delim
				<< item3->getDBIndex() << delim;
		if (packet->itemCount > 0)
		{
			GAMELOG << item4->getDBIndex() << delim
					<< packet->itemCount << delim;
		}
		GAMELOG << jewelindex << end;

		ch->m_inventory.decreaseItemCount(item1, 1);
		ch->m_inventory.decreaseItemCount(item2, 1);
		ch->m_inventory.decreaseItemCount(item3, 1);

		// ������ �����
		if (packet->itemCount > 0)
		{
			ch->m_inventory.decreaseItemCount(item4, packet->itemCount);
		}

		//MSG_EX_SOCKET_UPGRADE_JEWEL_CHAOS_REP
		CNetMsg::SP rmsg(new CNetMsg);
		SocketMsg(rmsg, MSG_EX_SOCKET_UPGRADE_JEWEL_CHAOS_REP);
		RefMsg(rmsg) << pJewel->getVIndex();

		SEND_Q(rmsg, ch->m_desc);
	}
	else
	{
		GAMELOG << init("JEWEL UPGRADE FAIL: INVENTORY", ch)
				<< item1->getDBIndex() << delim
				<< item2->getDBIndex() << delim
				<< item3->getDBIndex() << delim;
		if (packet->itemCount > 0)
		{
			GAMELOG << item4->getDBIndex() << delim
					<< packet->itemCount << delim;
		}
		GAMELOG << jewelindex << end;

		delete pJewel;

		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_INVEN_NOSPACE);
		SEND_Q(rmsg, ch->m_desc);
	}
}

void do_ExSocketMakeReq(CPC* ch, CNetMsg::SP& msg)
{
	RequestClient::doExSocketMake* packet = reinterpret_cast<RequestClient::doExSocketMake*>(msg->m_buf);

	{
		std::set<int> checkdup;
		int key = 0;

		key = makeCheckDupKey(packet->tab_1, packet->invenIndex_1);
		if (checkdup.insert(key).second == false)
		{
			LOG_ERROR("HACKING? : dup index. charIndex[%d]", ch->m_index);
			ch->m_desc->Close("dup index");
			return;
		}

		key = makeCheckDupKey(packet->tab_2, packet->invenIndex_2);
		if (checkdup.insert(key).second == false)
		{
			LOG_ERROR("HACKING? : dup index. charIndex[%d]", ch->m_index);
			ch->m_desc->Close("dup index");
			return;
		}
	}

	CNPC* npc = (CNPC *)ch->m_pArea->FindCharInCell(ch, packet->npcIndex, MSG_CHAR_NPC);
	if (npc == NULL)
	{
		LOG_ERROR("HACKING? : not found npc. char_index[%d] npcIndex[%d]", ch->m_index, packet->npcIndex);
		ch->m_desc->Close("not found npc");
		return;
	}

	if (!npc->IsFlag(NPC_SHOPPER) )
	{
		LOG_ERROR("HACKING? : not exist npc. char_index[%d] npcIndex[%d]", ch->m_index, packet->npcIndex);
		ch->m_desc->Close("not exist npc");
		return ;
	}

	if (ch->m_inventory.isValidNormalInventory(packet->tab_1, packet->invenIndex_1) == false)
	{
		LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_1, packet->invenIndex_1);
		ch->m_desc->Close("invalid packet");
		return;
	}

	if (ch->m_inventory.isValidNormalInventory(packet->tab_2, packet->invenIndex_2) == false)
	{
		LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_2, packet->invenIndex_2);
		ch->m_desc->Close("invalid packet");
		return;
	}

	// ��� ���������� Ȯ��
	CItem* equipitem = ch->m_inventory.getItem(packet->tab_1, packet->invenIndex_1);
	if (equipitem == NULL)
	{
		LOG_ERROR("HACKING? : not found item. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_1, packet->invenIndex_1);
		ch->m_desc->Close("not found item");
		return;
	}

	if (equipitem->getVIndex() != packet->virualIndex_1)
	{
		// �ùٸ� �������� �ƴմϴ�
		LOG_ERROR("HACKING? : invalid virtual index. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_1, packet->invenIndex_1);
		ch->m_desc->Close("invalid virtual index");
		return ;
	}

	// ��ũ�� ����������
	CItem* scroll = ch->m_inventory.getItem(packet->tab_2, packet->invenIndex_2);
	if (scroll == NULL)
	{
		LOG_ERROR("HACKING? : not found item. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_2, packet->invenIndex_2);
		ch->m_desc->Close("not found item");
		return;
	}

	if (scroll->getVIndex() != packet->virualIndex_2)
	{
		// �ùٸ� �������� �ƴմϴ�.
		LOG_ERROR("HACKING? : invalid virtual index. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_2, packet->invenIndex_2);
		ch->m_desc->Close("invalid virtual index");
		return ;
	}

	if (scroll->isProcessScroll() == false)
	{
		LOG_ERROR("HACKING? : not scroll item. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_2, packet->invenIndex_2);
		ch->m_desc->Close("not scroll item");
		return ;
	}

	if(!equipitem->IsSocketItem())
	{
		LOG_ERROR("HACKING? : invalid socket item. char_index[%d] tab[%d] invenIndex[%d] itemIndex[%d]",
			ch->m_index, packet->tab_1, packet->invenIndex_1, equipitem->getDBIndex());
		ch->m_desc->Close("invalid socket item");
		return ;
	}

	// ���յ� ���¿����� �ȵ�
	if(equipitem->getFlag() & FLAG_ITEM_COMPOSITION)
		return ;

	// �������� ��� �Ұ�
	if(equipitem->getWearPos() != WEARING_NONE)
		return ;

	int nNeedMoney = (int)(equipitem->m_itemProto->getItemPrice() * 0.25);

	if(ch->m_inventory.getMoney() < nNeedMoney)
	{
		// ������
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_NOMONEY);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	if(equipitem->getFlag() & FLAG_ITEM_SOCKET)
	{
		LOG_INFO("HACKING - already create socket item. charIndex : %d", ch->m_index);
		ch->m_desc->Close("already create socket item");
		return;
	}

	if( equipitem->m_socketList.SetSocketCount(scroll->m_itemProto->getItemIndex()) )
	{
		{
			equipitem->setFlag(equipitem->getFlag() | FLAG_ITEM_SOCKET);

			ch->m_inventory.sendOneItemInfo(equipitem);
		}
		// ���� ���� ���� �޽���
		// ���� ���� ���� �޽���
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SocketMsg(rmsg, MSG_EX_SOCKET_MAKE_REP);
			RefMsg(rmsg) << equipitem->getVIndex() << equipitem->m_socketList.GetSocketCount();
			SEND_Q(rmsg, ch->m_desc);
		}

		GAMELOG << init("SOCKET MAKE SUCCESS", ch) << itemlog(equipitem) << end;
	}
	else
	{
		// ���� ���� ���� �����޽���
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_FAILED_MAKE);
		SEND_Q(rmsg, ch->m_desc);

		GAMELOG << init("SOCKET MAKE FAIL", ch) << itemlog(equipitem) << end;

		return;
	}

	// �κ��� ��ũ�� ������ �����
	ch->m_inventory.decreaseItemCount(scroll, 1);

	// ���� ���̱�
	if (nNeedMoney > 0)
	{
		ch->m_inventory.decreaseMoney(nNeedMoney);
	}
}

void do_ExSocketCombineJewelReq(CPC* ch, CNetMsg::SP& msg)
{
	RequestClient::doExSocketCombineJewel* packet = reinterpret_cast<RequestClient::doExSocketCombineJewel*>(msg->m_buf);

	{
		std::set<int> checkdup;
		int key = 0;

		key = makeCheckDupKey(packet->tab_1, packet->invenIndex_1);
		if (checkdup.insert(key).second == false)
		{
			LOG_ERROR("HACKING? : dup index. charIndex[%d]", ch->m_index);
			ch->m_desc->Close("dup index");
			return;
		}

		key = makeCheckDupKey(packet->tab_2, packet->invenIndex_2);
		if (checkdup.insert(key).second == false)
		{
			LOG_ERROR("HACKING? : dup index. charIndex[%d]", ch->m_index);
			ch->m_desc->Close("dup index");
			return;
		}

		key = makeCheckDupKey(packet->tab_3, packet->invenIndex_3);
		if (checkdup.insert(key).second == false)
		{
			LOG_ERROR("HACKING? : dup index. charIndex[%d]", ch->m_index);
			ch->m_desc->Close("dup index");
			return;
		}
	}

	CNPC* npc = TO_NPC(ch->m_pArea->FindCharInCell(ch, packet->npcIndex, MSG_CHAR_NPC, false));
	if (npc == NULL)
	{
		LOG_ERROR("HACKING? : not found npc. char_index[%d] npcIndex[%d]", ch->m_index, packet->npcIndex);
		ch->m_desc->Close("not found npc");
		return;
	}

	if (!npc->IsFlag(NPC_SHOPPER) )
	{
		LOG_ERROR("HACKING? : not exist npc. char_index[%d] npcIndex[%d]", ch->m_index, packet->npcIndex);
		ch->m_desc->Close("not exist npc");
		return ;
	}
	CItem *pProtect_item = NULL;

	if (packet->tab_3 != INVALID_SHORT_INDEX || packet->invenIndex_3 != INVALID_SHORT_INDEX)
	{
		if (ch->m_inventory.isValidNormalInventory(packet->tab_3, packet->invenIndex_3) == false)
		{
			LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
					  ch->m_index, packet->tab_3, packet->invenIndex_3);
			ch->m_desc->Close("invalid packet");
			return;
		}

		pProtect_item = ch->m_inventory.getItem(packet->tab_3, packet->invenIndex_3);
		if(pProtect_item == NULL)
		{
			LOG_ERROR("HACKING? : not found item. char_index[%d] tab[%d] invenIndex[%d]", ch->m_index, packet->tab_3, packet->invenIndex_3);
			ch->m_desc->Close("not found item");
			return ;
		}

		if (pProtect_item->getVIndex() != packet->virualIndex_3)
		{
			LOG_ERROR("HACKING? : invalid virtualIndex. char_index[%d] tab[%d] invenIndex[%d]", ch->m_index, packet->tab_3, packet->invenIndex_3);
			ch->m_desc->Close("invalid virtualIndex");
			return ;
		}

		if (pProtect_item->IsFuncType() == false)
		{
			LOG_ERROR("HACKING? : invalid func type. char_index[%d] tab[%d] invenIndex[%d]", ch->m_index, packet->tab_3, packet->invenIndex_3);
			ch->m_desc->Close("invalid func type");
			return ;
		}
	}

	if (ch->m_inventory.isValidNormalInventory(packet->tab_1, packet->invenIndex_1) == false)
	{
		LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_1, packet->invenIndex_1);
		ch->m_desc->Close("invalid packet");
		return;
	}

	if (ch->m_inventory.isValidNormalInventory(packet->tab_2, packet->invenIndex_2) == false)
	{
		LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_2, packet->invenIndex_2);
		ch->m_desc->Close("invalid packet");
		return;
	}

	CItem *pEq_item = ch->m_inventory.getItem(packet->tab_1, packet->invenIndex_1);
	if (pEq_item == NULL)
	{
		LOG_ERROR("HACKING? : not found item. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_1, packet->invenIndex_1);
		ch->m_desc->Close("not found item");
		return;
	}

	if (pEq_item->getVIndex() != packet->virualIndex_1)
	{
		LOG_ERROR("HACKING? : invalid virtual index. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_1, packet->invenIndex_1);
		ch->m_desc->Close("invalid virtual index");
		return;
	}

	if (pEq_item->IsSocketItem() == false)
	{
		LOG_ERROR("HACKING? : invalid socket item. char_index[%d] tab[%d] invenIndex[%d] itemIndex[%d]",
				  ch->m_index, packet->tab_1, packet->invenIndex_1, pEq_item->getDBIndex());
		ch->m_desc->Close("invalid weapon type");
		return;
	}

	if (pEq_item->m_socketList.GetSocketCount() == 0)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_FLAG_SOCKET);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	CItem *pJewel_item = ch->m_inventory.getItem(packet->tab_2, packet->invenIndex_2);
	if (pJewel_item == NULL)
	{
		LOG_ERROR("HACKING? : not found item. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_2, packet->invenIndex_2);
		ch->m_desc->Close("not found item");
		return;
	}

	LOG_INFO("SOCKET COMBINE START> charIndex : %d, jewelIndex : %d, jewelCount : %d", ch->m_index, pJewel_item->m_itemProto->getItemIndex(), pJewel_item->Count());

	if (pJewel_item->getVIndex() != packet->virualIndex_2)
	{
		LOG_ERROR("HACKING? : invalid virtual index. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_2, packet->invenIndex_2);
		ch->m_desc->Close("invalid virtual index");
		return;
	}

	if (pJewel_item->m_itemProto->getItemSet0() != JEWEL_COMPOSITE_POSITION_ALL)		//�������� ������ ��� a_set_0 �� ���� ��ġ�� ��Ÿ����.
	{
		bool bFailed = false;
		if (pEq_item->m_itemProto->getItemTypeIdx() == ITYPE_WEAPON)
		{
			if(!(pJewel_item->m_itemProto->getItemSet0() & JEWEL_COMPOSITE_POSITION_WEAPON))
				bFailed = true;
		}

		else if (pEq_item->m_itemProto->getItemTypeIdx() == ITYPE_WEAR)
		{
			if (pEq_item->m_itemProto->getItemSubTypeIdx() == IWEAR_HELMET		&& !(pJewel_item->m_itemProto->getItemSet0() & JEWEL_COMPOSITE_POSITION_HELMET))		bFailed = true;
			if (pEq_item->m_itemProto->getItemSubTypeIdx() == IWEAR_ARMOR		&& !(pJewel_item->m_itemProto->getItemSet0() & JEWEL_COMPOSITE_POSITION_ARMOR))			bFailed = true;
			if (pEq_item->m_itemProto->getItemSubTypeIdx() == IWEAR_PANTS		&& !(pJewel_item->m_itemProto->getItemSet0() & JEWEL_COMPOSITE_POSITION_PANTS))			bFailed = true;
			if (pEq_item->m_itemProto->getItemSubTypeIdx() == IWEAR_GLOVE		&& !(pJewel_item->m_itemProto->getItemSet0() & JEWEL_COMPOSITE_POSITION_GLOVES))		bFailed = true;
			if (pEq_item->m_itemProto->getItemSubTypeIdx() == IWEAR_SHOES		&& !(pJewel_item->m_itemProto->getItemSet0() & JEWEL_COMPOSITE_POSITION_SHOES))			bFailed = true;
			if (pEq_item->m_itemProto->getItemSubTypeIdx() == IWEAR_SHIELD		&& !(pJewel_item->m_itemProto->getItemSet0() & JEWEL_COMPOSITE_POSITION_SHIELD))		bFailed = true;
			if (pEq_item->m_itemProto->getItemSubTypeIdx() == IWEAR_BACKWING	&& !(pJewel_item->m_itemProto->getItemSet0() & JEWEL_COMPOSITE_POSITION_BACKWING))		bFailed = true;
		}
		else if( pEq_item->m_itemProto->getItemTypeIdx() == ITYPE_ACCESSORY)
		{
			switch(pEq_item->m_itemProto->getItemSubTypeIdx())
			{
			case IACCESSORY_CHARM:
			case IACCESSORY_MAGICSTONE:
			case IACCESSORY_LIGHTSTONE:
			case IACCESSORY_EARING:
			case IACCESSORY_RING:
			case IACCESSORY_NECKLACE:
				if( (pJewel_item->m_itemProto->getItemSet0() & JEWEL_COMPOSITE_POSITION_ACCESSORY) == false )
					bFailed = true;
				break;
			default:
				bFailed = true;
			}
		}
		else
			bFailed = true;

		if (bFailed)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_ITEM_MISSMATCH);
			SEND_Q(rmsg, ch->m_desc);
			LOG_INFO("SOCKET COMBINE MISSMATCH> charIndex : %d, jewelIndex : %d, jewelCount : %d", ch->m_index, pJewel_item->m_itemProto->getItemIndex(), pJewel_item->Count());
			return ;
		}
	}

	if( ch->m_inventory.getEmptyCount() < 1)										//���� ���нÿ� �κ��丮�� ���ο� �������� �־���� �� ���� �����Ƿ�
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SysFullInventoryMsg(rmsg, 0);
		SEND_Q(rmsg, ch->m_desc);
		LOG_INFO("SOCKET COMBINE INVENFULL> charIndex : %d, jewelIndex : %d, jewelCount : %d", ch->m_index, pJewel_item->m_itemProto->getItemIndex(), pJewel_item->Count());
		return ;
	}

	if (packet->pos == 0 && !pJewel_item->IsChaosJewelType())							//0�ڸ����� ī���� ������ ������ �� �ִ�.
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_ITEM_MISSMATCH);
		SEND_Q(rmsg, ch->m_desc);
		LOG_INFO("SOCKET COMBINE MISSMATCH> charIndex : %d, jewelIndex : %d, jewelCount : %d", ch->m_index, pJewel_item->m_itemProto->getItemIndex(), pJewel_item->Count());
		return ;
	}

	if (packet->pos > 0 && packet->pos <= MAX_SOCKET_COUNT && !pJewel_item->IsJewelType())		//1������ �ڸ����� �Ϲ� ������ ������ �� �ִ�.
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_ITEM_MISSMATCH);
		SEND_Q(rmsg, ch->m_desc);
		LOG_INFO("SOCKET COMBINE MISSMATCH> charIndex : %d, jewelIndex : %d, jewelCount : %d", ch->m_index, pJewel_item->m_itemProto->getItemIndex(), pJewel_item->Count());
		return ;
	}

	if (pEq_item->m_socketList.GetJewelAt(packet->pos) != 0)							//���� ��ġ�� �̹� ������ ������ �ȵȴ�.
		return ;

	for (int i = 0; i < MAX_SOCKET_COUNT; i++)									//���� �ɼ� �ߺ� üũ (�ߺ��Ǹ� �ȵȴ�)
	{
		COption *option = pEq_item->m_socketList.GetJewelOptionAt(i);
		if (option)
		{
			if (option->m_type == pJewel_item->m_itemProto->getItemRareIndex(0))
			{
				LOG_INFO("SOCKET COMBINE DUPLICATION OPTION > charIndex : %d, jewelIndex : %d, jewelCount : %d", ch->m_index, pJewel_item->m_itemProto->getItemIndex(), pJewel_item->Count());
				return ;
			}
		}
	}

	socket_config *instance = socket_config::Get_instance();
	if (!instance)
	{
		GAMELOG << init("ERROR : socket_config is NULL") << "do_ExSocketCombineJewelReq" << end;
		return ;
	}

	GoldType_t need_nas = 0;
	if (packet->pos == 0)	/* chaos jewel */
		need_nas = instance->Get_chaos_jewel_composite_cost();
	else			/* normal jewel */
		need_nas = instance->Get_normal_jewel_composite_cost();
	// TODO:NAS DECREASE
	if(ch->m_inventory.getMoney() < need_nas)
	{
		// ������
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_NOMONEY);
		SEND_Q(rmsg, ch->m_desc);
		LOG_INFO("SOCKET COMBINE NOT ENOUGH MONEY> charIndex : %d, money : %d, jewelIndex : %d, jewelCount : %d", ch->m_index, ch->m_inventory.getMoney(), pJewel_item->m_itemProto->getItemIndex(), pJewel_item->Count());
		return ;
	}

	ch->m_inventory.decreaseMoney(need_nas);

	int prob = GetRandom(1, 10000);
	int eventValue = gserver->m_jewelDataList.m_jewelData->getEventCombineProb();
	bool protectItem = true;

	int pJewel_item_for_log_dbindex = pJewel_item->m_itemProto->getItemIndex();
	LONGLONG pJewel_item_for_log_count = pJewel_item->Count();


	if (prob > pJewel_item->m_itemProto->getItemNum1() * eventValue / 100)		//���� ���� Ȯ�� (���н�)
	{
		// TODO:���� ������ ���� ���н�
		if(pProtect_item == NULL)
		{
			//���� �������� ���� ��쿡�� �������� �����ϰ� �ش� ��� �Ʒ��� �����ָӴ� �������� �߱��Ѵ�.
			//�ش� ���� �������� �Ѵܰ� �Ʒ��� �������� ����� �κ��� �־��ش�.
			protectItem = false;
			int grade = pJewel_item->m_itemProto->getItemNum0();
			int jewelIndex = 0;

			grade = grade - 1;

			if ( grade <= 0 )
				grade = 1;

			int rand = GetRandom(1, gserver->m_jewelDataList.m_nJewelItemKindCount);
			if(pJewel_item->IsChaosJewelType())
			{
				//ī���� ���� �ָӴϸ� ����
				int chaosJewelPocket = gserver->m_jewelDataList.FindChaosJewelPocketIndex(grade);

				CItem *chaosPocketItem = gserver->m_itemProtoList.CreateItem(chaosJewelPocket, WEARING_NONE, 0, 0, 1);
				if (chaosPocketItem == NULL)
				{
					LOG_ERROR("SOCKET COMBINE > can't create item. itemDBIndex is %d", chaosJewelPocket);
					return;
				}

				if (ch->m_inventory.addItem(chaosPocketItem) == false)
				{
					delete chaosPocketItem;

					CNetMsg::SP rmsg(new CNetMsg);
					SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_FAILED_MAKE);
					SEND_Q(rmsg, ch->m_desc);
					LOG_INFO("SOCKET COMBINE ADD INVENTORY CHAOSPOCKET FAIL> charIndex : %d, ChaosPocketItemIndex : %d, jewelIndex : %d, jewelCount : %d", ch->m_index, chaosPocketItem->m_itemProto->getItemIndex(), pJewel_item->m_itemProto->getItemIndex(), pJewel_item->Count());
					return ;
				}
				LOG_INFO("SOCKET COMBINE ADD INVENTORY CHAOSPOCKET SUCCESS> charIndex : %d, ChaosPocketItemIndex : %d, jewelIndex : %d, jewelCount : %d", ch->m_index, chaosPocketItem->m_itemProto->getItemIndex(), pJewel_item->m_itemProto->getItemIndex(), pJewel_item->Count());
			}
			else if(pJewel_item->IsJewelType())
			{
				//�Ϲ� ���� �ָӴϸ� ����
				int pocketIndex = gserver->m_jewelDataList.FindJewelPocketIndex(grade);
				CItem *pocketItem = gserver->m_itemProtoList.CreateItem(pocketIndex, WEARING_NONE, 0, 0, 1);
				if (pocketItem == NULL)
				{
					LOG_ERROR("SOCKET COMBINE > can't create item. itemDBIndex is %d", pocketIndex);
					return;
				}

				if (ch->m_inventory.addItem(pocketItem) == false)
				{
					delete pocketItem;

					CNetMsg::SP rmsg(new CNetMsg);
					SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_FAILED_MAKE);
					SEND_Q(rmsg, ch->m_desc);
					LOG_INFO("SOCKET COMBINE ADD INVENTORY POCKET FAIL> charIndex : %d, PocketItemIndex : %d, jewelIndex : %d, jewelCount : %d", ch->m_index, pocketItem->m_itemProto->getItemIndex(), pJewel_item->m_itemProto->getItemIndex(), pJewel_item->Count());
					return ;
				}
				LOG_INFO("SOCKET COMBINE ADD INVENTORY POCKET SUCCESS> charIndex : %d, PocketItemIndex : %d, jewelIndex : %d, jewelCount : %d", ch->m_index, pocketItem->m_itemProto->getItemIndex(), pJewel_item->m_itemProto->getItemIndex(), pJewel_item->Count());
			}

			ch->m_inventory.decreaseItemCount(pJewel_item, 1);
		}
		else
		{
			//��ȣ ��ũ�� ������ ����
			ch->m_inventory.decreaseItemCount(pProtect_item, 1);
		}

		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_FAILED_COMBINE);
		SEND_Q(rmsg, ch->m_desc);
		if(protectItem)
			LOG_INFO("SOCKET COMBINE FAIL> charIndex : %d, jewelIndex : %d, jewelCount : %d, isProtectItemUse = true", ch->m_index, pJewel_item_for_log_dbindex, pJewel_item_for_log_count);
		else
			LOG_INFO("SOCKET COMBINE FAIL> charIndex : %d, jewelIndex : %d, jewelCount : %d, isProtectItemUse = false", ch->m_index, pJewel_item_for_log_dbindex, pJewel_item_for_log_count);
		return ;
	}

	if (!pEq_item->m_socketList.SetJewelAt(packet->pos, pJewel_item->getDBIndex()))	//������ ����.
	{
		GAMELOG << init("SOCKET COMBINE", ch) << "FAIL:SET" << delim << packet->pos << delim << "JEWEL" << pJewel_item->getDBIndex() << end;
		LOG_INFO("SOCKET COMBINE FAIL> charIndex : %d, jewelIndex : %d, jewelCount : %d", ch->m_index, pJewel_item->m_itemProto->getItemIndex(), pJewel_item->Count());
		return ;
	}

	pEq_item->setFlag(pEq_item->getFlag() | FLAG_ITEM_SOCKET);

	if(pJewel_item->m_itemProto->getItemSet0() == 0 && pJewel_item->m_itemProto->getItemRareIndex(0) == OPTION_DOWN_LIMITLEVEL)		//���տ� �����Ͽ����� �ɼ��� 49���ɼ��̸�
	{
		pEq_item->setFlag(pEq_item->getFlag() | FLAG_ITEM_LEVELDOWN);
	}

	ch->m_inventory.decreaseItemCount(pJewel_item, 1);
	ch->m_inventory.sendOneItemInfo(pEq_item);

	//��ȣ ��ũ�� ������ ����
	if( pProtect_item != NULL)
	{
		ch->m_inventory.decreaseItemCount(pProtect_item, 1);
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		SocketMsg(rmsg, MSG_EX_SOCKET_COMBINE_JEWEL_REP);
		RefMsg(rmsg) << pEq_item->getVIndex();
		SEND_Q(rmsg, ch->m_desc);
	}

	if(protectItem)
		LOG_INFO("SOCKET COMBINE SUCCESS> charIndex : %d, jewelIndex : %d, jewelCount : %d, isProtectItemUse = true", ch->m_index, pJewel_item_for_log_dbindex, pJewel_item_for_log_count);
	else
		LOG_INFO("SOCKET COMBINE SUCCESS> charIndex : %d, jewelIndex : %d, jewelCount : %d, isProtectItemUse = false", ch->m_index, pJewel_item_for_log_dbindex, pJewel_item_for_log_count);
	GAMELOG << init("SOCKET COMBINE", ch) << "SUCCESS" << itemlog(pEq_item) << end;
}

void do_ExSocketCleanJewel(CPC* ch, CNetMsg::SP& msg)
{
	RequestClient::doExSocketCleanJewel* packet = reinterpret_cast<RequestClient::doExSocketCleanJewel*>(msg->m_buf);

	CNPC* npc = (CNPC *)ch->m_pArea->FindCharInCell(ch, packet->npcIndex, MSG_CHAR_NPC);
	if (npc == NULL)
	{
		LOG_ERROR("HACKING? : not found npc. char_index[%d] npcIndex[%d]", ch->m_index, packet->npcIndex);
		ch->m_desc->Close("not found npc");
		return;
	}

	if (!npc->IsFlag(NPC_SHOPPER) )
	{
		LOG_ERROR("HACKING? : not exist npc. char_index[%d] npcIndex[%d]", ch->m_index, packet->npcIndex);
		ch->m_desc->Close("not exist npc");
		return ;
	}

	if (ch->m_inventory.isValidNormalInventory(packet->tab, packet->invenIndex) == false)
	{
		LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab, packet->invenIndex);
		ch->m_desc->Close("invalid packet");
		return;
	}

	CItem* pEquipItem = ch->m_inventory.getItem(packet->tab, packet->invenIndex);
	if (pEquipItem == NULL)
	{
		LOG_ERROR("HACKING? : not found item. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab, packet->invenIndex);
		ch->m_desc->Close("not found item");
		return;
	}

	if (pEquipItem->getVIndex() != packet->virualIndex)
	{
		LOG_ERROR("HACKING? : invalid virtual index. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab, packet->invenIndex);
		ch->m_desc->Close("invalid virtual index");
		return;
	}

	if(!pEquipItem->IsSocketItem())
	{
		LOG_ERROR("HACKING? : invalid socket item. char_index[%d] tab[%d] invenIndex[%d] itemIndex[%d]",
			ch->m_index, packet->tab, packet->invenIndex, pEquipItem->getDBIndex());
		ch->m_desc->Close("invalid virtual index");
		return ;
	}

	// ���յ� ���¿����� �ȵ�
	if(pEquipItem->getFlag() & FLAG_ITEM_COMPOSITION)
		return ;

	// ������ ���� �ȵ�
	if(pEquipItem->getWearPos() != WEARING_NONE)
		return ;

	char socketcount = pEquipItem->m_socketList.GetSocketCount();
	if(socketcount < 1)
	{
		// ������ ������ ����.
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_NOSPACE);
		SEND_Q(rmsg, ch->m_desc);

		GAMELOG << init("SOCKET NOT EXIST", ch) << itemlog(pEquipItem) << end;
		return;
	}

	if(!pEquipItem->m_socketList.IsCombinedJewel())
	{
		// �������յ� ���� ����.
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_NOJEWEL);
		SEND_Q(rmsg, ch->m_desc);

		GAMELOG << init("JEWEL NOT EXIST", ch) << itemlog(pEquipItem) << end;
		return ;
	}
	socket_config *instance = socket_config::Get_instance();
	if (!instance)
	{
		GAMELOG << init("ERROR : socket_config is NULL") << "do_ExSocketCleanJewel" << end;
		return ;
	}
	// �� �˻�
	int needMoney = pEquipItem->m_socketList.GetCombinedCount() * pEquipItem->m_socketList.GetSocketCount() * instance->Get_jewel_clean_factor();
	if (pEquipItem->m_socketList.GetJewelAt(0) > 0)
		needMoney += instance->Get_chaos_jewel_clean_factor();

	if(ch->m_inventory.getMoney() < needMoney)
	{
		// ������
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_NOMONEY);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	std::vector<int> back_item;
	int temp_idx;
	for (int i = 0; i < MAX_SOCKET_COUNT; i++)
	{
		temp_idx = pEquipItem->m_socketList.IsSuccessDisable(i);
		if (temp_idx > 0)
			back_item.push_back(temp_idx);
	}

	pEquipItem->m_socketList.ClearAll();

	ch->m_inventory.decreaseMoney(needMoney);

	ch->m_inventory.sendOneItemInfo(pEquipItem);

	{
		CNetMsg::SP rmsg(new CNetMsg);
		SocketMsg(rmsg, MSG_EX_SOCKET_CLEAN_JEWEL_REP);
		RefMsg(rmsg) << pEquipItem->getVIndex();
		SEND_Q(rmsg, ch->m_desc);
	}

	for (std::vector<int>::iterator itr = back_item.begin(); itr != back_item.end(); itr++)
	{
		CItem *backitem = gserver->m_itemProtoList.CreateItem(*itr, -1, 0, 0, 1);
		if (backitem == NULL)
			continue;

		if (ch->m_inventory.addItem(backitem) == false)
		{
			// drop field
			backitem->m_preferenceIndex = ch->m_index;
			ch->m_pArea->DropItem(backitem, ch);
			CNetMsg::SP rmsg(new CNetMsg);
			ItemDropMsg(rmsg, ch, backitem);
			backitem->m_pArea->SendToCell(rmsg, GET_YLAYER(backitem), backitem->m_cellX, backitem->m_cellZ);
		}

		GAMELOG << init("SUCCESS SEPERATED JEWEL", ch) << itemlog(backitem) << end;
	}

	GAMELOG << init("JEWEL CLEAN SUCCESS", ch) << pEquipItem->getDBIndex() << needMoney << end;
}

void do_ExSocketCleanOneJewel(CPC* ch, CNetMsg::SP& msg)
{
	RequestClient::doExSocketCleanOneJewel* packet = reinterpret_cast<RequestClient::doExSocketCleanOneJewel*>(msg->m_buf);

	{
		std::set<int> checkdup;
		int key = 0;

		key = makeCheckDupKey(packet->tab_1, packet->invenIndex_1);
		if (checkdup.insert(key).second == false)
		{
			LOG_ERROR("HACKING? : dup index. charIndex[%d]", ch->m_index);
			ch->m_desc->Close("dup index");
			return;
		}

		key = makeCheckDupKey(packet->tab_2, packet->invenIndex_2);
		if (checkdup.insert(key).second == false)
		{
			LOG_ERROR("HACKING? : dup index. charIndex[%d]", ch->m_index);
			ch->m_desc->Close("dup index");
			return;
		}
	}

	CNPC* npc = (CNPC *)ch->m_pArea->FindCharInCell(ch, packet->npcIndex, MSG_CHAR_NPC);
	if (npc == NULL)
	{
		LOG_ERROR("HACKING? : not found npc. char_index[%d] npcIndex[%d]", ch->m_index, packet->npcIndex);
		ch->m_desc->Close("not found npc");
		return;
	}

	if (!npc->IsFlag(NPC_SHOPPER) )
	{
		LOG_ERROR("HACKING? : not exist npc. char_index[%d] npcIndex[%d]", ch->m_index, packet->npcIndex);
		ch->m_desc->Close("not exist npc");
		return ;
	}

	if (ch->m_inventory.isValidNormalInventory(packet->tab_1, packet->invenIndex_1) == false)
	{
		LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_1, packet->invenIndex_1);
		ch->m_desc->Close("invalid packet");
		return;
	}

	if (ch->m_inventory.isValidNormalInventory(packet->tab_2, packet->invenIndex_2) == false)
	{
		LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_2, packet->invenIndex_2);
		ch->m_desc->Close("invalid packet");
		return;
	}

	CItem* pEquipItem = ch->m_inventory.getItem(packet->tab_1, packet->invenIndex_1);
	if (pEquipItem == NULL)
	{
		LOG_ERROR("HACKING? : not found item. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_1, packet->invenIndex_1);
		ch->m_desc->Close("not found item");
		return;
	}

	if (pEquipItem->getVIndex() != packet->virualIndex_1)
	{
		LOG_ERROR("HACKING? : invalid virtual index. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_1, packet->invenIndex_1);
		ch->m_desc->Close("invalid virtual index");
		return;
	}

	if (pEquipItem->IsSocketItem() == false)
	{
		LOG_ERROR("HACKING? : invalid socket item. char_index[%d] tab[%d] invenIndex[%d] itemIndex[%d]",
				  ch->m_index, packet->tab_1, packet->invenIndex_1, pEquipItem->getDBIndex());
		ch->m_desc->Close("invalid weapon type");
		return;
	}

	CItem* pScrollItem = ch->m_inventory.getItem(packet->tab_2, packet->invenIndex_2);
	if (pScrollItem == NULL)
	{
		LOG_ERROR("HACKING? : not found item. char_index[%d] tab[%d] invenIndex[%d]",
				  ch->m_index, packet->tab_2, packet->invenIndex_2);
		ch->m_desc->Close("not found item");
		return;
	}

	if (pScrollItem->getVIndex() != packet->virualIndex_2)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_ITEM_MISSMATCH);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	socket_config *config = socket_config::Get_instance();
	if (!config)
	{
		GAMELOG << init("ERROR : socket_config is NULL") << "do_ExSocketCleanOneJewel" << end;
		return ;
	}

	if (!config->Is_seperate_scroll_item(pScrollItem->getDBIndex()))
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_ITEM_MISSMATCH);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	if (pEquipItem->m_socketList.GetJewelAt(packet->pos) < 1)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SocketErrorMsg(rmsg, MSG_EX_SOCKET_ERROR_ITEM_MISSMATCH);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	ch->m_inventory.decreaseItemCount(pScrollItem, 1);

	int jewel_db_index = pEquipItem->m_socketList.IsSuccessDisable(packet->pos);
	pEquipItem->m_socketList.ClearOne(packet->pos);

	ch->m_inventory.sendOneItemInfo(pEquipItem);

	if (jewel_db_index > 0)
	{
		CItem *backitem = gserver->m_itemProtoList.CreateItem(jewel_db_index, -1, 0, 0, 1);
		if (backitem)
		{
			if (ch->m_inventory.addItem(backitem) == false)
			{
				// drop field
				backitem->m_preferenceIndex = ch->m_index;
				ch->m_pArea->DropItem(backitem, ch);
				CNetMsg::SP rmsg(new CNetMsg);
				ItemDropMsg(rmsg, ch, backitem);
				backitem->m_pArea->SendToCell(rmsg, GET_YLAYER(backitem), backitem->m_cellX, backitem->m_cellZ);
			}

			GAMELOG << init("SUCCESS SEPERATED JEWEL", ch) << itemlog(backitem) << end;
		}
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		SocketMsg(rmsg, MSG_EX_SOCKET_CLEAN_JEWEL_REP);
		RefMsg(rmsg) << pEquipItem->getVIndex();
		SEND_Q(rmsg, ch->m_desc);
	}
}
