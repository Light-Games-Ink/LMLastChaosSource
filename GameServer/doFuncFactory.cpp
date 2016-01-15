#include "stdhdrs.h"

#include "Log.h"
#include "Character.h"
#include "Server.h"
#include "CmdMsg.h"
#include "doFunc.h"


void do_Factory(CPC* ch, CNetMsg::SP& msg)
{
//	if (DEAD(ch) || ch->IsSetPlayerState(PLAYER_STATE_SITDOWN))
//		return ;

	msg->MoveFirst();

	unsigned char subtype;
	RefMsg(msg) >> subtype;

	switch (subtype)
	{
	case MSG_FACTORY_LEARN:
		do_FactoryLearn(ch, msg);
		break;
	case MSG_FACTORY_ITEM_LIST:
		do_FactoryList(ch, msg);
		break;
	case MSG_FACTORY_ITEM_MAKE_REQ:
		do_FactoryMakeReq(ch, msg);
		break;
	case MSG_FACTORY_ITEM_MAKE:
		do_FactoryMake(ch, msg);
		break;
	}
}

void do_FactoryLearn(CPC* ch, CNetMsg::SP& msg, bool GMCommand)
{
	try
	{
		msg->MoveFirst();
		unsigned char subtype;
		int nIndex;

#ifdef NPC_CHECK
		int npcIndex;
		CNPC *npc;
		RefMsg(msg) >> subtype
					>> npcIndex
					>> nIndex;
		if(!GMCommand)
		{
			npc = ch->m_pArea->FindProtoNPCInCell(ch, npcIndex, false, 2);
			if(!npc)
			{
				GAMELOG << init("FactoryLearn FAIL: NOT EXIST NPC (Dangerous User)", ch) << end;
				return ;
			}
		}
#else
		RefMsg(msg) >> subtype
					>> nIndex;
#endif

		CFactoryProto * pFactory = gserver->m_factoryItemProtoList.Find(nIndex);
		if (!pFactory)
			throw MSG_FACTORY_ERROR_SYSTEM;

		// ���� ��� ����
		if (!ch->m_sealSkillList.Find(FACTORY_UNION_REG_SKILL))
			throw MSG_FACTORY_ERROR_SYSTEM;

		// ��ų üũ
		if (!ch->m_sealSkillList.Find(pFactory->GetSealType()))
			throw MSG_FACTORY_ERROR_SYSTEM;

		// �̹� ������� �� ��� �ʿ����.
		if (ch->m_listFactory.Find(pFactory))
			throw MSG_FACTORY_ERROR_ALREADY_REG;

		// ����
		if ( ch->m_inventory.getMoney() < pFactory->GetNas())
			throw MSG_FACTORY_ERROR_NAS;

		// ���õ�
		if (ch->GetSealExp(pFactory->GetSealType()) < pFactory->GetNeedExp())
			throw MSG_FACTORY_ERROR_EXP;

		ch->m_inventory.decreaseMoney(pFactory->GetNas());

		// �߰�
		ch->m_listFactory.Add(pFactory);
		GAMELOG << init("FACTORY ITEM LEARN", ch) << pFactory->GetIndex() << end;

		{
			CNetMsg::SP rmsg(new CNetMsg);
			FactoryLearnMsg(rmsg, pFactory);
			SEND_Q(rmsg, ch->m_desc);
		}
	}
	catch (MSG_FACTORY_ERROR_TYPE learnerr)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		FactoryErrorMsg(rmsg, learnerr);
		SEND_Q(rmsg, ch->m_desc);
	}
}

void do_FactoryList(CPC* ch, CNetMsg::SP& msg)
{
	unsigned char subtype;
	char cUI;
	int nSkillIdx;

	msg->MoveFirst();

	RefMsg(msg) >> subtype
				>> cUI              // Ŭ���̾�Ʈ���� UI������ ����
				>> nSkillIdx;

	{
		CNetMsg::SP rmsg(new CNetMsg);
		rmsg->Init(MSG_FACTORY);
		RefMsg(rmsg) << (unsigned char)MSG_FACTORY_ITEM_LIST
					 << cUI
					 << nSkillIdx;

		FactoryListMsg(rmsg, ch, nSkillIdx);	// �� �Լ� ���ʿ��� rmsg ��Ŷ�� ���� ������ �Է��Ѵ�.
		SEND_Q(rmsg, ch->m_desc);
	}
}

void do_FactoryMakeReq(CPC* ch, CNetMsg::SP& msg)
{
	unsigned char subtype;
	int nIndex;

	msg->MoveFirst();

	RefMsg(msg) >> subtype
				>> nIndex;

	try
	{
		CFactoryProto * pFactory = gserver->m_factoryItemProtoList.Find(nIndex);
		if (!pFactory)
			throw MSG_FACTORY_ERROR_SYSTEM;

		// ��ų üũ
		if (!ch->m_sealSkillList.Find(pFactory->GetSealType()))
			throw MSG_FACTORY_ERROR_SKILL;

		// ��� üũ
		for (int i = 0; i < MAX_FACTORY_ITEM_STUFF; i++)
		{
			if (!pFactory->GetStuff(i))
				continue;

			if( pFactory->GetStuff(i)->nItemIdx < 1 )
				continue;

			CItem* item = ch->m_inventory.FindByDBIndex(pFactory->GetStuff(i)->nItemIdx, 0, 0);
			if (item == NULL)
				throw MSG_FACTORY_ERROR_ITEM;
		}

		// �κ��丮�� ��ĭ�� �ִ���
		if (ch->m_inventory.getEmptyCount() < 1)
			throw MSG_FACTORY_ERROR_INVEN;

// [110125: selo] ���� ������ ��� ����
		if( !ch->CheckInvenForPrize(pFactory->GetItemIdx(),0,0,1) )
		{
			return;
		}

		{
			CNetMsg::SP rmsg(new CNetMsg);
			rmsg->Init(MSG_FACTORY);
			RefMsg(rmsg) << (unsigned char)MSG_FACTORY_ITEM_MAKE_REP
						 << (unsigned char)MSG_FACTORY_ERROR_MAKE_OK;
			SEND_Q(rmsg, ch->m_desc);
		}
	}
	catch (MSG_FACTORY_ERROR_TYPE learnerr)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		FactoryErrorMsg(rmsg, learnerr);
		SEND_Q(rmsg, ch->m_desc);
	}
}

void do_FactoryMake(CPC* ch, CNetMsg::SP& msg)
{
	unsigned char subtype;
	int nIndex;

	msg->MoveFirst();

	RefMsg(msg) >> subtype
				>> nIndex;

	try
	{
		CFactoryProto * pFactory = gserver->m_factoryItemProtoList.Find(nIndex);
		if (!pFactory)
			throw MSG_FACTORY_ERROR_SYSTEM;

		// ��ų üũ
		if (!ch->m_sealSkillList.Find(pFactory->GetSealType()))
			throw MSG_FACTORY_ERROR_SKILL;

		// ��� üũ
		item_search_t vec[MAX_FACTORY_ITEM_STUFF];
		for (int i = 0; i < MAX_FACTORY_ITEM_STUFF; i++)
		{
			if (!pFactory->GetStuff(i))
				continue ;

			if (pFactory->GetStuff(i)->nItemIdx < 1 )
				continue ;

			int sc = ch->m_inventory.searchItemByCondition(pFactory->GetStuff(i)->nItemIdx, 0, 0, vec[i]);
			if (sc == 0 || sc < pFactory->GetStuff(i)->nCount)
				throw MSG_FACTORY_ERROR_ITEM;
		}

		// �κ��丮�� ��ĭ�� �ִ���
		if (ch->m_inventory.getEmptyCount() < 1)
			throw MSG_FACTORY_ERROR_INVEN;

		// ���� ������ �߰�
		CItem * pMakeItem = gserver->m_itemProtoList.CreateItem(pFactory->GetItemIdx(), WEARING_NONE, 0, 0, 1);
		if (pMakeItem == NULL)
			throw MSG_FACTORY_ERROR_SYSTEM;

		if (ch->m_inventory.addItem(pMakeItem) == false)
		{
			delete pMakeItem;
			throw MSG_FACTORY_ERROR_SYSTEM;
		}

		// ������ �Ҹ�
		for (int i = 0; i < MAX_FACTORY_ITEM_STUFF; i++)
		{
			const FACTORY_STUFF * pStuff = pFactory->GetStuff(i);
			if (!pStuff)
				continue;

			ch->m_inventory.deleteItem(vec[i], pStuff->nCount);
		}

		// ���õ� ����
		LONGLONG llCharExp = ch->GetSealExp(pFactory->GetSealType());
		LONGLONG llMakeExp = pFactory->GetMakeExp();
		int	expProb = 0;

		if (llCharExp - llMakeExp < 10 ) expProb = 100 ;
		else if (llCharExp - llMakeExp < 20 ) expProb = 80;
		else if (llCharExp - llMakeExp < 30 ) expProb = 50 ;
		else expProb = 0;

		if( GetRandom(1,100) <= expProb )
		{
			llCharExp ++;
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_FACTORY_EXP);
			RefMsg(rmsg) << (LONGLONG)1;
			SEND_Q(rmsg, ch->m_desc);
		}

		ch->SetSealExp(pFactory->GetSealType(), llCharExp);

		{
			CNetMsg::SP rmsg(new CNetMsg);
			FactoryMakeMsg(rmsg, ch, pFactory);
			SEND_Q(rmsg, ch->m_desc);
		}
	}
	catch (MSG_FACTORY_ERROR_TYPE learnerr)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		FactoryErrorMsg(rmsg, learnerr);
		SEND_Q(rmsg, ch->m_desc);
	}
}

