#include "stdhdrs.h"

#include "Log.h"
#include "Character.h"
#include "Server.h"
#include "CmdMsg.h"
#include "doFunc.h"
#include "hardcoding.h"
#include "Battle.h"
#include "Exp.h"
#include "WarCastle.h"
#include "DratanCastle.h"
#include "../ShareLib/packetType/ptype_tradeAgent_system.h"
#include "DBManager.h"

float g_TradeAgent_Fee = 5.0f;	// �ŷ����� ������
int g_TradeAgent_Deposit= 1000;		// �ŷ����� ������

void do_TradeAgent(CPC* ch, CNetMsg::SP& msg)
{
	// �װų� ��ȯ���̸� ����
	pTypeBase* pBase = reinterpret_cast<pTypeBase*>(msg->m_buf);

	if(pBase->subType != MSG_TRADEAGENT_REG_LIST)
	{
		if (DEAD(ch) || ch->m_exchange != NULL || ch->m_currentSkill != NULL || ch->m_personalshop != NULL)
			return ;
	}

	switch (pBase->subType)
	{
	case MSG_TRADEAGENT_LIST:
		do_TradeAgent_SearchReq(ch, msg);
		break;

	case MSG_TRADEAGENT_REG_LIST:
		do_TradeAgent_RegListReq(ch, msg);
		break;

	case MSG_TRADEAGENT_REG:
		do_TradeAgent_RegReq(ch, msg);
		break;

	case MSG_TRADEAGENT_REG_CANCEL:
		do_TradeAgent_RegCancelReq(ch, msg);
		break;

	case MSG_TRADEAGENT_BUY:
		do_TradeAgent_BuyReq(ch, msg);
		break;

	case MSG_TRADEAGENT_LIKE_LIST:
		do_TradeAgent_LikeListReq(ch, msg);
		break;

	case MSG_TRADEAGENT_LIKE_REG:
		do_TradeAgent_LikeRegReq(ch, msg);
		break;

	case MSG_TRADEAGENT_LIKE_CANCEL:
		do_TradeAgent_LikeCancel(ch, msg);
		break;

	case MSG_TRADEAGENT_ERROR:
		do_TradeAgent_Error(ch, msg);
		break;
	}
}

// ��� ����Ʈ ��û
void do_TradeAgent_RegListReq(CPC* ch, CNetMsg::SP& msg)
{
	if (gserver->isRunSubHelper())
	{
		/*CNetMsg::SP rmsg(msg);
		RequestClient::TradeAgentSystemRegList *p = reinterpret_cast<RequestClient::TradeAgentSystemRegList*>(rmsg->m_buf);
		rmsg->setSize(sizeof(RequestClient::TradeAgentSystemRegList));
		p->charIndex = ch->m_index;
		SEND_Q(rmsg, gserver->m_subHelper);*/
		RequestClient::TradeAgentSystemRegList *p = reinterpret_cast<RequestClient::TradeAgentSystemRegList*>(msg->m_buf);
		p->charIndex = ch->m_index;
		SEND_Q(msg, gserver->m_subHelper);
	}
	else
	{
		CNetMsg::SP rmsg(new CNetMsg);
		makeTradeAgentError(rmsg, ResponseClient::TRADEAGENT_ERROR_SEARCH_ERROR);
		SEND_Q(rmsg, ch->m_desc);
	}
}

// ��� ��û
void do_TradeAgent_RegReq(CPC* ch, CNetMsg::SP& msg)
{
	if (gserver->isRunSubHelper() == false)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		makeTradeAgentError(rmsg, ResponseClient::TRADEAGENT_ERROR_ITEM_REG_FAIL);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	{
		RequestClient::TradeAgentSystemReg *p = reinterpret_cast<RequestClient::TradeAgentSystemReg*>(msg->m_buf);

		//���� ����� ó�������� üũ(���� �޼��� ������ �ʰ� ����)
		if(ch->m_bTradeAgentRegIng == true)
		{
			LOG_ERROR("tradeagent - registering. charIndex[%d]", ch->m_index);
			return ;
		}

		if(ch->m_tradeAgentRegCount >= MAX_REG_COUNT)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			makeTradeAgentError(rmsg, ResponseClient::TRADEAGENT_ERROR_ITEM_REG_FULL);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}

		// ������ ã��
		CItem* item = ch->m_inventory.getItem(p->tab, p->invenIndex);
		if (item == NULL || item->getDBIndex() != p->itemIndex || item->getWearPos() != WEARING_NONE)
			return;

		const CItemProto* itemproto = item->m_itemProto;
		if (itemproto == NULL)
			return;

		if(item->m_socketList.GetSocketCount() > 0 && item->m_socketList.getSocketPos(0)->GetJewelDBIdx() == -1)
			return;

		//������ ���� üũ
		if(p->quantity <= 0 || item->Count() < p->quantity)
		{
			// ���� �޼���: ������ ���� ����Ȯ
			CNetMsg::SP rmsg(new CNetMsg);
			makeTradeAgentError(rmsg, ResponseClient::TRADEAGENT_ERROR_NOT_EXIST_ITEM);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}

		//�� üũ (�Է�Ƽ)
		if(ch->m_inventory.getMoney() < (ch->m_tradeAgentRegCount + 1) * g_TradeAgent_Deposit)
		{
			// ���� �޼���: �� ����
			CNetMsg::SP rmsg(new CNetMsg);
			makeTradeAgentError(rmsg, ResponseClient::TRADEAGENT_ERROR_NOT_ENOUGH_MONEY);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}

		//�� �ݾ� üũ
		if(p->TotalMoney <= 0)
		{
			// ���� �޼���: �Ǹ� �� �ݾ� ����Ȯ
			CNetMsg::SP rmsg(new CNetMsg);
			makeTradeAgentError(rmsg, ResponseClient::TRADEAGENT_ERROR_INCORRECT_MONEY);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}

#ifdef ENABLE_SUBJOB
		if( item && item->CheckTrader() && item->CanUseTrader( TRADE_TRADEAGENT, ch->IsSubJob(JOB_SUB_TRADER) ) == false )
		{
			CNetMsg::SP rmsg(new CNetMsg);
			makeTradeAgentError(rmsg, ResponseClient::TRADEAGENT_ERROR_NOT_TRADEITEM);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}

		if( !item->CheckTrader() )
		{
#endif //ENABLE_SUBJOB
			if( !(itemproto->getItemFlag() & ITEM_FLAG_EXCHANGE) ||
					(itemproto->getItemFlag() & ITEM_FLAG_COMPOSITE) ||
					(item->getFlag() & FLAG_ITEM_SEALED) ||
					(item->getFlag() & FLAG_ITEM_COMPOSITION) ||
					(item->getFlag() & FLAG_ITEM_LENT) ||
					(item->getFlag() & FLAG_ITEM_BELONG) ||
					( item->getUsed() > 0 ) || ( item->getUsed_2() > 0 ) || (itemproto->getItemFlag() & ITEM_FLAG_NOTTRADEAGENT)
			  )
			{
				// ���� �޼���: �ŷ� �Ұ��� ������
				CNetMsg::SP rmsg(new CNetMsg);
				makeTradeAgentError(rmsg, ResponseClient::TRADEAGENT_ERROR_NOT_TRADEITEM);
				SEND_Q(rmsg, ch->m_desc);
				return;
			}
#ifdef ENABLE_SUBJOB
		}
#endif //ENABLE_SUBJOB
		//�Ǽ��縮 , �ֿϵ��� �ŷ� �Ұ� || �Ǽ��縮 , ������ �ֿϵ��� �ŷ� �Ұ�
		if( ((itemproto->getItemTypeIdx() == 5) && (itemproto->getItemSubTypeIdx() == 6)) ||
				((itemproto->getItemTypeIdx() == 5) && (itemproto->getItemSubTypeIdx() == 7)) )
		{
			// ���� �޼���: �ŷ� �Ұ��� ������
			CNetMsg::SP rmsg(new CNetMsg);
			makeTradeAgentError(rmsg, ResponseClient::TRADEAGENT_ERROR_NOT_TRADEITEM);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}

		int itemCount;
		itemCount = item->Count();

		CNetMsg::SP rmsg(new CNetMsg);
		rmsg->BufferClear();
		RequestClient::TradeAgentSystemRegServer* packet = reinterpret_cast<RequestClient::TradeAgentSystemRegServer *>(rmsg->m_buf);
		packet->type = MSG_TRADEAGENT;
		packet->subType = MSG_TRADEAGENT_REG;

		packet->itemIndex = item->getDBIndex();
		packet->item.item_index = item->getDBIndex();
		packet->charIndex = ch->m_index;
		strcpy(packet->charName, (const char*)ch->m_nick);
		packet->Guarantee = (ch->m_tradeAgentRegCount + 1) * g_TradeAgent_Deposit;
		strcpy(packet->item.serial, item->m_serial.c_str());
		packet->item.flag = item->getFlag();
		packet->item.plus = item->getPlus();
		packet->item.plus2 = item->getPlus_2();
		packet->item.item_level = item->GetItemLevel();
		packet->itemCount = itemCount;
		packet->quantity = p->quantity;
		packet->TotalMoney = p->TotalMoney;
#ifdef DURABILITY
		packet->item.now_durability = item->getNowDurability();
		packet->item.max_durability = item->getMaxDurability();
#endif
		if(item->IsRareItem())
		{
			//������ �������̸�
			if( item->m_pRareOptionProto != NULL
					&& item->m_pRareOptionProto->GetIndex() > 0)
			{
				//���� �������̸� ����� ���� �̾� �������ش�.
				packet->item.rareGrade = item->m_pRareOptionProto->GetGrade();
			}
			//�̰��� �������̸�
			else
			{
				packet->item.rareGrade = eITEM_GRADE_RARE_NOTOPEN;
			}
		}
		else if(item->IsOriginItem())
		{
			packet->item.rareGrade = eITEM_GRADE_ORIGIN;
		}
		else
		{
			packet->item.rareGrade = eITEM_GRADE_NORMAL;
		}

		packet->item.option_count = item->m_nOption;
		for( int i = 0; i< item->m_nOption; i++)
		{
			packet->item.option_type[i] = item->m_option[i].m_type;
			packet->item.option_level[i] = item->m_option[i].m_level;
		}

		packet->item.socketCount = item->m_socketList.GetSocketCount();
		memset(packet->item.socket, -1, sizeof(packet->item.socket));
		if (packet->item.socketCount > 0)
		{
			int count = (item->m_socketList.GetJewelAt(0) == -1) ? 1 : 0;
			int end = (item->m_socketList.GetJewelAt(0) == -1) ? item->m_socketList.GetSocketCount() + 1 : item->m_socketList.GetSocketCount();
			for( int i = count; i < end; i++)
			{
				packet->item.socket[i] = item->m_socketList.GetJewelAt(i);
			}
		}

		for( int i = 0; i < MAX_VARIATION_COUNT; i++)
		{
			packet->item.item_origin[i] = item->m_OriginVar[i];
		}
		packet->tab = p->tab;
		packet->invenIndex = p->invenIndex;
		packet->itemCount = item->Count() - p->quantity;

		rmsg->setSize(sizeof(RequestClient::TradeAgentSystemRegServer));

		//���� �����, ������ ����� (�޸𸮻󿡼��� ����)

		{
			int Guarantee = (ch->m_tradeAgentRegCount + 1) * g_TradeAgent_Deposit;
			ch->m_inventory.decreaseMoney(Guarantee);
			ch->m_inventory.decreaseItemCount(item, p->quantity);

			// ���� ũ���ö� ������ ���縦 �����ϱ� ���ؼ� �ǽð� ������ ��
			DBManager::instance()->SaveCharacterInfo(ch->m_desc, false);
		}

		// ���� ũ���ö� ó���� ���� ĳ���� ������ ���� �����ϰ� SubHelper�� ������ ����
		SEND_Q(rmsg, gserver->m_subHelper);

		//���� ��� ó�� �� ����
		ch->m_bTradeAgentRegIng = true;
	}
}

// ��� ���
void do_TradeAgent_RegCancelReq(CPC* ch, CNetMsg::SP& msg)
{
	if (gserver->isRunSubHelper())
	{
		RequestClient::TradeAgentSystemRegCancel *p = reinterpret_cast<RequestClient::TradeAgentSystemRegCancel*>(msg->m_buf);
		p->charIndex = ch->m_index;
		SEND_Q(msg, gserver->m_subHelper);
	}
	else
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::TradeAgentSystemError *p = reinterpret_cast<ResponseClient::TradeAgentSystemError*>(rmsg->m_buf);
		makeTradeAgentError(rmsg, ResponseClient::TRADEAGENT_ERROR_ITEM_REG_CANCEL_FAIL);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}
}

// ��ȸ
void do_TradeAgent_SearchReq(CPC* ch, CNetMsg::SP& msg)
{
	if (gserver->isRunSubHelper())
	{
		RequestClient::TradeAgentSystemListBase *p = reinterpret_cast<RequestClient::TradeAgentSystemListBase*>(msg->m_buf);

		if(findPercentChar(p->itemName) != NULL)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			makeTradeAgentError(rmsg, ResponseClient::TRADEAGENT_ERROR_SEARCH_ERROR);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}

		if(p->itemTypeTab < 2)
		{
			const int nGrade[] = { eITEM_GRADE_MAX,
								   eITEM_GRADE_NORMAL,
								   eITEM_GRADE_RARE_BASIC,
								   eITEM_GRADE_RARE_MAGIC,
								   eITEM_GRADE_RARE_RARE,
								   eITEM_GRADE_RARE_UNIQUE,
								   eITEM_GRADE_RARE_HERO,
								   eITEM_GRADE_ORIGIN,
								   eITEM_GRADE_RARE_NOTOPEN
								 };

			RequestClient::TradeAgentSystemList *packet = reinterpret_cast<RequestClient::TradeAgentSystemList*>(msg->m_buf);
			packet->itemGrade = nGrade[packet->itemGrade];
			packet->charIndex = ch->m_index;
			SEND_Q(msg, gserver->m_subHelper);
		}
		else
		{
			p->charIndex = ch->m_index;
			SEND_Q(msg, gserver->m_subHelper);
		}
	}
	else
	{
		CNetMsg::SP rmsg(new CNetMsg);
		makeTradeAgentError(rmsg, ResponseClient::TRADEAGENT_ERROR_SEARCH_ERROR);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}
}

// ����
void do_TradeAgent_BuyReq(CPC* ch, CNetMsg::SP& msg)
{
	RequestClient::TradeAgentSystemBuy *p = reinterpret_cast<RequestClient::TradeAgentSystemBuy*>(msg->m_buf);

	CCharacter::tradestash_t::iterator it = ch->m_tradeAgentViewMap.find(p->TradeAgentIndex);
	if (it == ch->m_tradeAgentViewMap.end())
	{
		it = ch->m_tradeAgentViewMap2.find(p->TradeAgentIndex);
		if (it == ch->m_tradeAgentViewMap2.end())
		{
			ch->m_desc->Close("Not Found tradestash Index.....");
			return;
		}
	}

	GoldType_t needNas = it->second;

	if(ch->m_inventory.getMoney() < needNas)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		makeTradeAgentError(rmsg, ResponseClient::TRADEAGENT_ERROR_NOT_ENOUGH_MONEY);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	if (gserver->isRunSubHelper())
	{
		ch->m_inventory.decreaseMoneyNotSend(needNas);

		GAMELOG << init("TRADEAGENT BUY.1.(GameServer)", ch)
				<< "TradeAgent Index" << delim
				<< p->TradeAgentIndex << delim
				<< "needNas" << delim
				<< needNas << end;

		//���� ��Ŷ ���� ����
		RequestClient::TradeAgentSystemBuy *packet = reinterpret_cast<RequestClient::TradeAgentSystemBuy*>(msg->m_buf);
		packet->charIndex = ch->m_index;
		packet->needNas = needNas;
		packet->TradeAgentIndex = p->TradeAgentIndex;
		SEND_Q(msg, gserver->m_subHelper);
		return;
	}
	else
	{
		CNetMsg::SP rmsg(new CNetMsg);
		makeTradeAgentError(rmsg, ResponseClient::TRADEAGENT_ERROR_ITEM_BUY_FAIL);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}
}

void do_TradeAgent_Error(CPC* ch, CNetMsg::SP& msg)
{
	//Ư�� ������ ������ ��쿡�� �������� �������־�� �Ѵ�.
	//��� �����Ͽ��� ���....
	ResponseClient::TradeAgentSystemError *p = reinterpret_cast<ResponseClient::TradeAgentSystemError*>(msg->m_buf);
	CPC *pc = PCManager::instance()->getPlayerByCharIndex(p->charIndex);

	switch(p->errorCode)
	{
	case ResponseClient::TRADEAGENT_ERROR_ITEM_REG_FAIL:
		break;
	}
}

void do_TradeAgent_LikeListReq(CPC* ch, CNetMsg::SP& msg)
{
	if (gserver->isRunSubHelper())
	{
		RequestClient::TradeAgentSystemLikeList *p = reinterpret_cast<RequestClient::TradeAgentSystemLikeList*>(msg->m_buf);
		p->charIndex = ch->m_index;
		SEND_Q(msg, gserver->m_subHelper);
	}
	else
	{
		CNetMsg::SP rmsg(new CNetMsg);
		makeTradeAgentError(rmsg, ResponseClient::TRADEAGENT_ERROR_ITEM_BUY_FAIL);
		SEND_Q(rmsg, ch->m_desc);
	}
}
void do_TradeAgent_LikeRegReq(CPC* ch, CNetMsg::SP& msg)
{
	if (gserver->isRunSubHelper())
	{
		RequestClient::TradeAgentSystemLikeReg *p = reinterpret_cast<RequestClient::TradeAgentSystemLikeReg*>(msg->m_buf);
		p->charIndex = ch->m_index;
		SEND_Q(msg, gserver->m_subHelper);
	}
	else
	{
		CNetMsg::SP rmsg(new CNetMsg);
		makeTradeAgentError(rmsg, ResponseClient::TRADEAGENT_ERROR_ITEM_LIKE_REG_FAIL);
		SEND_Q(rmsg, ch->m_desc);
	}
}
void do_TradeAgent_LikeCancel(CPC* ch, CNetMsg::SP& msg)
{
	if (gserver->isRunSubHelper())
	{
		RequestClient::TradeAgentSystemLikeCancel *p = reinterpret_cast<RequestClient::TradeAgentSystemLikeCancel*>(msg->m_buf);
		p->charIndex = ch->m_index;
		SEND_Q(msg, gserver->m_subHelper);
	}
	else
	{
		CNetMsg::SP rmsg(new CNetMsg);
		makeTradeAgentError(rmsg, ResponseClient::TRADEAGENT_ERROR_ITEM_LIKE_CANCEL_FAIL);
		SEND_Q(rmsg, ch->m_desc);
	}
}

