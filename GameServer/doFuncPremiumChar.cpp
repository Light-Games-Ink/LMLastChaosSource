#include "stdhdrs.h"

#ifdef PREMIUM_CHAR

#include "Log.h"
#include "Character.h"
#include "Server.h"
#include "Area.h"

#include "../ShareLib/packetType/ptype_premium_char.h"

void do_PremiumChar_Use_Item(CPC* ch, CNetMsg::SP& msg)
{
	RequestClient::premiumChar_UseItem* packet = reinterpret_cast<RequestClient::premiumChar_UseItem*>(msg->m_buf);

	if (ch->m_inventory.isValidNormalInventory(packet->tab, packet->invenIndex) == false)
	{
		LOG_ERROR("HACKING? : invalid packet. char_index[%d] tab[%d] invenIndex[%d]",
			ch->m_index, packet->tab, packet->invenIndex);
		ch->m_desc->Close("invalid packet");
		return;
	}

	// ������ ã��
	CItem* pItem = ch->m_inventory.getItem(packet->tab, packet->invenIndex);
	if (pItem == NULL)
	{
		LOG_ERROR("HACKING? : not found item. char_index[%d] tab[%d] invenIndex[%d]",
			ch->m_index, packet->tab, packet->invenIndex);
		ch->m_desc->Close("not found item");
		return;
	}

	if (pItem->getVIndex() != packet->virtualIndex)
	{
		LOG_ERROR("HACKING? : invalid virtual index. char_index[%d] tab[%d] invenIndex[%d]",
			ch->m_index, packet->tab, packet->invenIndex);
		ch->m_desc->Close("invalid virtual index");
		return;
	}

	// ���� ���
	if (ch->m_premiumChar.isActive())
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makePremiumCharUseItem(rmsg, ERR_PREMIUM_CHAR_ALREADY_USED);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}
	
	ch->m_premiumChar.reset();

	switch (pItem->getDBIndex())
	{
	case PREMIUM_CHAR_ITEM_INDEX:
		{
			ch->m_premiumChar.setPremiumType(PREMIUM_CHAR_TYPE_FIRST);
			ch->m_premiumChar.setExpireTime(gserver->getNowSecond() + (30 * 86400));
			ch->m_premiumChar.sendInfo();
		}
		break;

	default:
		{
			LOG_ERROR("HACKING? : invalid item index. char_index[%d] tab[%d] invenIndex[%d]",
				ch->m_index, packet->tab, packet->invenIndex);
			ch->m_desc->Close("invalid item index");
		}
		return;
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makePremiumCharUseItem(rmsg, ERR_PREMIUM_CHAR_NONE);
		SEND_Q(rmsg, ch->m_desc);
	}

	ch->m_premiumChar.setActive();

	// ������ �����
	ch->m_inventory.decreaseItemCount(pItem, 1);

	ch->m_premiumChar.saveNow();

	ch->m_premiumChar.saveSetLog();

	ch->SendStatus();
}

//XX - �����̾�ĳ���� : ģ������ ����
extern bool isPossibleWarp(CPC* fromChar, CPC* toChar);		// doFuncWarp.cpp

void do_PremiumChar_Jump(CPC* ch, CNetMsg::SP& msg)
{
	if (ch->m_premiumChar.isActive() == false)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makePremiumCharJumpToChar(rmsg, ERR_PREMIUM_CHAR_NOT_PREMIUM_CHAR);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	if (ch->CanWarp() != 0)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makePremiumCharJumpToChar(rmsg, ERR_PREMIUM_CHAR_NOT_JUMP_STATUS);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	RequestClient::premiumChar_JumpToChar* packet = reinterpret_cast<RequestClient::premiumChar_JumpToChar*>(msg->m_buf);
	packet->toCharName[MAX_CHAR_NAME_LENGTH] = '\0';

	CPC* pToChar = PCManager::instance()->getPlayerByName(packet->toCharName, false);
	if (pToChar == NULL)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makePremiumCharJumpToChar(rmsg, ERR_PREMIUM_CHAR_NOT_FOUNC_CHAR);
		SEND_Q(rmsg, ch->m_desc);

		return;
	}

	if (ch->m_premiumChar.getJumpCount() >= PREMIUM_CHAR_JUMP_COUNT_MAX)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makePremiumCharJumpToChar(rmsg, ERR_PREMIUM_CHAR_MAX_JUMP_COUNT);
		SEND_Q(rmsg, ch->m_desc);

		return;
	}

	// ������ ����� ģ���ΰ�?
	CFriendMember* pFriend = ch->m_Friend->FindFriendMember(pToChar->m_index);
	if (pFriend == NULL)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makePremiumCharJumpToChar(rmsg, ERR_PREMIUM_CHAR_NOT_FOUNC_CHAR);
		SEND_Q(rmsg, ch->m_desc);

		return;
	}

	// ģ���� ���°� offline�ΰ�?
	if (pFriend->GetCondition() == MSG_FRIEND_CONDITION_OFFLINE)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makePremiumCharJumpToChar(rmsg, ERR_PREMIUM_CHAR_FRIEND_IS_OFFLINE);
		SEND_Q(rmsg, ch->m_desc);

		return;
	}

	// ģ������ �����Ҽ� �ִ°�? item 857�� ������ �������� �˻� (��Ŷ MSG_WARP)
	if (isPossibleWarp(ch, pToChar) == false)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makePremiumCharJumpToChar(rmsg, ERR_PREMIUM_CHAR_INVALID_JUMPING_POSITION);
		SEND_Q(rmsg, ch->m_desc);

		return;
	}

	// ������� �����ϱ�
	if(ch->m_pZone->m_index != pToChar->m_pZone->m_index)
		ch->SetPlayerState(PLAYER_STATE_CASH_ZONE_MOVE);

	if(!GoZone(ch, pToChar->m_pZone->m_index, pToChar->m_pos.m_yLayer, pToChar->m_pos.m_x, pToChar->m_pos.m_z))
	{
		if(ch->IsSetPlayerState(PLAYER_STATE_CASH_ZONE_MOVE))
			ch->ResetPlayerState(PLAYER_STATE_CASH_ZONE_MOVE);

		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makePremiumCharJumpToChar(rmsg, ERR_PREMIUM_CHAR_INVALID_JUMPING_POSITION);
		SEND_Q(rmsg, ch->m_desc);

		return;
	}

	ch->m_premiumChar.setJumpCount(ch->m_premiumChar.getJumpCount() + 1);

	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makePremiumCharJumpToChar(rmsg, ERR_PREMIUM_CHAR_NONE);
		SEND_Q(rmsg, ch->m_desc);
	}
}

//////////////////////////////////////////////////////////////////////////

void do_PremiumChar(CPC* ch, CNetMsg::SP& msg)
{
	pTypeBase* pBase = reinterpret_cast<pTypeBase*>(msg->m_buf);
	
	switch (pBase->subType)
	{
	case MSG_SUB_PREMIUM_CHAR_USE_ITEM:
		do_PremiumChar_Use_Item(ch, msg);
		break;

	case MSG_SUB_PREMIUM_CHAR_JUMP_TO_CHAR:
		do_PremiumChar_Jump(ch, msg);
		break;

	default:
		{
			LOG_INFO("HACKING? : invalid subtype(%d)", pBase->subType);
			ch->m_desc->Close("invalid subtype");
		}
		break;
	}
}

#endif
