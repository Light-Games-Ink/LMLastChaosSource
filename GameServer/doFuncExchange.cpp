#include "stdhdrs.h"

#include "Log.h"
#include "Character.h"
#include "Server.h"
#include "CmdMsg.h"
#include "doFunc.h"
#include "DratanCastle.h"
#include "DBManager.h"
#include "Artifact_Manager.h"

/////////////////
// ��ȯ ���� �Լ�

void do_Exchange(CPC* ch, CNetMsg::SP& msg)
{
	CDratanCastle * pCastle = CDratanCastle::CreateInstance();
	pCastle->CheckRespond(ch);

	// �Ⱥ��̸� ����
	if (!ch->m_bVisible || ch->m_personalshop != NULL)
		return ;

	msg->MoveFirst();

	unsigned char subtype;
	RefMsg(msg) >> subtype;

	switch (subtype)
	{
	case MSG_EXCHANGE_REQ:
		do_ExchangeReq(ch, msg);
		break;
	case MSG_EXCHANGE_ITEM:
		do_ExchangeItem(ch, msg);
		break;
	}
}

void do_ExchangeReq(CPC* ch, CNetMsg::SP& msg)
{
	msg->MoveFirst();

	unsigned char subtype, reqtype;
	RefMsg(msg) >> subtype
				>> reqtype;

	switch (reqtype)
	{
	case MSG_EXCHANGE_REQ_REQ:
		do_ExchangeReqReq(ch, msg);
		break;
	case MSG_EXCHANGE_REQ_REP:
		do_ExchangeReqRep(ch, msg);
		break;
	case MSG_EXCHANGE_REQ_REJECT:
		do_ExchangeReqReject(ch, msg);
		break;
	case MSG_EXCHANGE_REQ_READY:
		do_ExchangeReqReady(ch, msg);
		break;
	case MSG_EXCHANGE_REQ_OK:
		do_ExchangeReqOK(ch, msg);
		break;
	}
}

void do_ExchangeItem(CPC* ch, CNetMsg::SP& msg)
{
	msg->MoveFirst();

	unsigned char subtype, action;
	RefMsg(msg) >> subtype
				>> action;

	switch (action)
	{
	case MSG_EXCHANGE_ITEM_ADD:
		do_ExchangeItemAdd(ch, msg);
		break;
	case MSG_EXCHANGE_ITEM_DEL:
// 050224 : bs : ��ȯ���� ������ ������ ����
//		do_ExchangeItemDel(ch, msg);
// --- 050224 : bs : ��ȯ���� ������ ������ ����
		break;
	}
}

void do_ExchangeReqReq(CPC* ch, CNetMsg::SP& msg)
{
	msg->MoveFirst();

	unsigned char subtype, reqtype;
	int srcIndex, destIndex;
	CLCString srcName(MAX_CHAR_NAME_LENGTH + 1);
	CLCString destName(MAX_CHAR_NAME_LENGTH + 1);

	RefMsg(msg) >> subtype
				>> reqtype
				>> srcIndex >> srcName
				>> destIndex >> destName;

	if (ch->IsSetPlayerState(PLAYER_STATE_CHANGE))
		return;

	// ���� ����� �� ��� �³�?
	if (ch->m_index != srcIndex)
		return ;

	// ������ ã��
	CPC* dest = TO_PC(ch->m_pArea->FindCharInCell(ch, destIndex, MSG_CHAR_PC));

	if (!dest)
	{
		// ��밡 ����
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_EXCHANGE_NOTFOUND);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	if (ch->m_exchange || dest->m_exchange)
	{
		// �̹� ��ȯ ��
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_EXCHANGE_ALREADY);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	if (ch->m_pZone->isRVRZone() && dest->m_pZone->isRVRZone() && ch->getSyndicateType() != dest->getSyndicateType())
	{
		// rvr �������� �ٸ� ����ʹ� �ŷ��� ���� ���� ����.
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_DO_NOT_SYSTEM_NOT_EQ_RVRTYPE);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	// ��ȯ ����
	CExchangeItems* exchange = new CExchangeItems;
	exchange->SetCharacter(ch, dest);
	ch->m_exchange = dest->m_exchange = exchange;

	GAMELOG << init("do_ExchangeReqReq()") << delim;

	if (ch)		GAMELOG << "src " << delim << ch->m_index << delim << ch->GetName() << delim;
	else		GAMELOG << "src is null" << delim;

	if (dest)	GAMELOG << "dest " << delim << dest->m_index << delim << dest->GetName() << end;
	else		GAMELOG << "dest is null" << end;

	{
		// ���濡�� �ǻ縦 ����
		CNetMsg::SP rmsg(new CNetMsg);
		ExchangeReqMsg(rmsg, MSG_EXCHANGE_REQ_REQ, ch, dest);
		SEND_Q(rmsg, dest->m_desc);
	}
}

void do_ExchangeReqRep(CPC* ch, CNetMsg::SP& msg)
{
	if (ch->m_exchange == NULL)
		return ;

	msg->MoveFirst();

	unsigned char subtype, reqtype;

	RefMsg(msg) >> subtype
				>> reqtype;

	CExchangeItems* exc = ch->m_exchange;
	CPC* src = exc->m_src.m_ch;
	CPC* dest = exc->m_dest.m_ch;

	// ��ȯ ���°� ��û �ǻ縦 ���� �ֳ�?
	if (exc->m_status != MSG_EXCHANGE_REQ_REQ)
		return ;

	exc->m_status = MSG_EXCHANGE_REQ_REP;

	{
		// ��ȯ ���� �˸���
		CNetMsg::SP rmsg(new CNetMsg);
		ExchangeReqMsg(rmsg, MSG_EXCHANGE_REQ_REP, src, dest);
		SEND_Q(rmsg, src->m_desc);
		SEND_Q(rmsg, dest->m_desc);
	}
}

void do_ExchangeReqReject(CPC* ch, CNetMsg::SP& msg)
{
	// ��ȯ ��?
	if (ch->m_exchange == NULL)
		return ;

	CExchangeItems* exc = ch->m_exchange;
	CPC* src = TO_PC(exc->m_src.m_ch);
	CPC* dest = TO_PC(exc->m_dest.m_ch);

	if( !src || !dest )
		return;

	// ���� ��ȯ ����ϱ�?
	bool bCancelBySrc;
	if (src == ch)
		bCancelBySrc = true;
	else if (dest == ch)
		bCancelBySrc = false;
	else
		return ;

	GAMELOG << init("do_ExchangeReqReject() - after 1 ") << delim;

	if (src)	GAMELOG << "src " << delim << src->m_index << delim << src->GetName() << delim;
	else		GAMELOG << "src is null" << delim;

	if (dest)	GAMELOG << "dest " << delim << dest->m_index << delim << dest->GetName() << delim;
	else		GAMELOG << "dest is null" << delim;

	GAMELOG << "bCancelBySrc" << bCancelBySrc << end;

	msg->MoveFirst();

	unsigned char subtype, reqtype;

	RefMsg(msg) >> subtype
				>> reqtype;

	// ��ȯ ���
	delete exc;
	exc = NULL;

	src->m_exchange = dest->m_exchange = NULL;

	if (dest && !(dest->GetName()))
	{
		GAMELOG << init("do_ExchangeReqReject() - after 2 ") << delim << "dest name is null" << end;
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExchangeReqMsg(rmsg, ((bCancelBySrc) ? MSG_EXCHANGE_REQ_REJECT_SRC : MSG_EXCHANGE_REQ_REJECT_DEST), src, dest);
		SEND_Q(rmsg, src->m_desc);
		SEND_Q(rmsg, dest->m_desc);
	}
}

void do_ExchangeReqReady(CPC* ch, CNetMsg::SP& msg)
{
	// ��ȯ ��?
	if (ch->m_exchange == NULL)
		return ;

	CExchangeItems* exc = ch->m_exchange;
	CPC* src = exc->m_src.m_ch;
	CPC* dest = exc->m_dest.m_ch;

	// ���� �غ� �Ϸ��ϱ�?
	bool bReadyBySrc;
	if (src == ch)
		bReadyBySrc = true;
	else if (dest == ch)
		bReadyBySrc = false;
	else
		return ;

	msg->MoveFirst();

	unsigned char subtype, reqtype;

	RefMsg(msg) >> subtype
				>> reqtype;

	if (exc->m_status != MSG_EXCHANGE_REQ_REP && exc->m_status != MSG_EXCHANGE_REQ_READY_SRC && exc->m_status != MSG_EXCHANGE_REQ_READY_DEST)
		return ;

	CNetMsg::SP retmsg(new CNetMsg);
	bool isSuccess = false;

	switch (exc->CanExchange())
	{
	case 0:		// ����
		isSuccess = true;
		break;

	case 1:		// src ����
		ExchangeReqMsg(retmsg, MSG_EXCHANGE_REQ_FULL_SRC, src, dest);
		exc->m_status = MSG_EXCHANGE_REQ_REP;
		goto SEND_AND_EXIT;

	case 2:		// dest ����
		ExchangeReqMsg(retmsg, MSG_EXCHANGE_REQ_FULL_DEST, src, dest);
		exc->m_status = MSG_EXCHANGE_REQ_REP;
		goto SEND_AND_EXIT;
	case 3:		// dest ����
		ExchangeReqMsg(retmsg, MSG_EXCHANGE_REQ_SRC_PET_ANYMORE, src, dest);
		exc->m_status = MSG_EXCHANGE_REQ_REP;
		goto SEND_AND_EXIT;
	case 4:		// dest ����
		ExchangeReqMsg(retmsg, MSG_EXCHANGE_REQ_DEST_PET_ANYMORE, src, dest);
		exc->m_status = MSG_EXCHANGE_REQ_REP;
		goto SEND_AND_EXIT;
	}

	// ���� ���¿� ����
	if (exc->m_status == MSG_EXCHANGE_REQ_REP)
	{
		// �ƹ��� �غ� �Ϸ� ���ߴ�
		// �����̸� 1�ܰ� �غ� �Ϸ� ���·� ����
		exc->m_status = (bReadyBySrc) ? MSG_EXCHANGE_REQ_READY_SRC : MSG_EXCHANGE_REQ_READY_DEST;
		// �� ����� �˸���
		ExchangeReqMsg(retmsg, exc->m_status, src, dest);
	}

	else if ( (exc->m_status == MSG_EXCHANGE_REQ_READY_SRC && !bReadyBySrc)
			  || (exc->m_status == MSG_EXCHANGE_REQ_READY_DEST && bReadyBySrc) )
	{
		// �� �� �غ� �ϷḦ �ߴ�
		// �׷��� 2�ܰ� �غ� �Ϸ� ���·� ��ȯ
		exc->m_status = MSG_EXCHANGE_REQ_READY;
		// �� ����� �˸���
		ExchangeReqMsg(retmsg, exc->m_status, src, dest);
	}
	else
	{
		// �� ��� �ƴϸ� �غ�Ϸ� �޽����� ���� �ȵȴ�
		return ;
	}

SEND_AND_EXIT:
	SEND_Q(retmsg, src->m_desc);
	SEND_Q(retmsg, dest->m_desc);
	if(isSuccess == false)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExchangeReqMsg(rmsg, MSG_EXCHANGE_REQ_REJECT, NULL, NULL);
		if(src->m_exchange) do_Exchange(src, rmsg);
		if(dest->m_exchange) do_Exchange(src, rmsg);
	}
}

void do_ExchangeReqOK(CPC* ch, CNetMsg::SP& msg)
{
	// ��ȯ ��?
	if (ch->m_exchange == NULL)
		return ;

	CExchangeItems* exc = ch->m_exchange;
	CPC* src = exc->m_src.m_ch;
	CPC* dest = exc->m_dest.m_ch;

	// ���� Ȯ���ϱ�?
	bool bOKBySrc;
	if (src == ch)
		bOKBySrc = true;
	else if (dest == ch)
		bOKBySrc = false;
	else
		return ;

	msg->MoveFirst();

	unsigned char subtype, reqtype;

	RefMsg(msg) >> subtype
				>> reqtype;

	// ���� ���¿� ����
	if (exc->m_status == MSG_EXCHANGE_REQ_READY)
	{
		// �ƹ��� Ȯ�� ���ߴ�
		// ���� ������ 1�ܰ� Ȯ�� ���·�
		exc->m_status = (bOKBySrc) ? MSG_EXCHANGE_REQ_OK_SRC : MSG_EXCHANGE_REQ_OK_DEST;
		// �� ����� �˸���
		CNetMsg::SP rmsg(new CNetMsg);
		ExchangeReqMsg(rmsg, exc->m_status, src, dest);
		SEND_Q(rmsg, src->m_desc);
		SEND_Q(rmsg, dest->m_desc);
	}

	else if ( (exc->m_status == MSG_EXCHANGE_REQ_OK_SRC && !bOKBySrc)
			  || (exc->m_status == MSG_EXCHANGE_REQ_OK_DEST && bOKBySrc) )
	{
		// ��ȯ ���� �˻�
		switch (exc->CanExchange())
		{
		case 0:		// ��ȯ ����
			{
				// ��ȯ �ϱ�
				// ��ȯ �Լ� �ȿ��� ���� �޽����� ����
				exc->DoExchange();

				// ��ȯ ����
				CNetMsg::SP rmsg(new CNetMsg);
				ExchangeReqMsg(rmsg, MSG_EXCHANGE_REQ_OK, src, dest);
				SEND_Q(rmsg, src->m_desc);
				SEND_Q(rmsg, dest->m_desc);

				// �ǽð� ����
				DBManager::instance()->SaveCharacterInfo(src->m_desc, false);
				DBManager::instance()->SaveCharacterInfo(dest->m_desc, false);
			}
			break;

		case 1:		// src�� ��ȯ �Ұ�
			{
				CNetMsg::SP rmsg(new CNetMsg);
				ExchangeReqMsg(rmsg, MSG_EXCHANGE_REQ_REJECT_SRC, src, dest);
				SEND_Q(rmsg, src->m_desc);
				SEND_Q(rmsg, dest->m_desc);
			}
			break;

		case 2:		// dest�� ��ȯ �Ұ�
			{
				CNetMsg::SP rmsg(new CNetMsg);
				ExchangeReqMsg(rmsg, MSG_EXCHANGE_REQ_REJECT_DEST, src, dest);
				SEND_Q(rmsg, src->m_desc);
				SEND_Q(rmsg, dest->m_desc);
			}
			break;
		}

		// ��ȯ ����
		delete exc;
		exc = NULL;

		src->m_exchange = dest->m_exchange = NULL;
	}

	return ;
}

void do_ExchangeItemAdd(CPC* ch, CNetMsg::SP& msg)
{
	// ��ȯ ���ΰ�?
	if (ch->m_exchange == NULL)
		return ;

	CPC* his;
	bool bSrc;
	// ���� �����ΰ�?
	if (ch->m_exchange->m_src.m_ch == ch)
	{
		his = ch->m_exchange->m_dest.m_ch;
		bSrc = true;
	}
	else if (ch->m_exchange->m_dest.m_ch == ch)
	{
		his = ch->m_exchange->m_src.m_ch;
		bSrc = false;
	}
	else
		return ;

	CExchangeItems* exc = ch->m_exchange;
	CPC* src = exc->m_src.m_ch;
	CPC* dest = exc->m_dest.m_ch;

	// ������ ���� ���°� �ƴϸ� ����
	switch (exc->m_status)
	{
	case MSG_EXCHANGE_REQ_READY_SRC:
		// ���� src�̰� �غ� ����
		if (bSrc)
			return ;
		break;

	case MSG_EXCHANGE_REQ_READY_DEST:
		// ���� dest�̰� �غ� ����
		if (!bSrc)
			return ;
		break;

	case MSG_EXCHANGE_REQ_REP:
		break;

	default:
		return ;
	}

	msg->MoveFirst();

	unsigned char subtype, action;
	int item_idx;
	LONGLONG count;
	CItem* item;

	RefMsg(msg) >> subtype
				>> action
				>> item_idx
				>> count;

	if (item_idx <= 0)
	{
		LOG_ERROR("do_ExchangeItemAdd : invalid item_idx[%d]", item_idx);
		return;
	}

	if (count <= 0)
	{
		LOG_ERROR("do_ExchangeItemAdd : invalid count[%d]", count);
		return;
	}

	if(item_idx != NAS_ITEM_DB_INDEX)
	{
		// ���� �������� �̹� �߰��Ǿ� �ִ��� �˻�
		int exidx = exc->Find(ch, item_idx);
		LONGLONG preCount = 0;
		if (exidx != -1)
			preCount = (bSrc) ? exc->m_src.m_count[exidx] : exc->m_dest.m_count[exidx];

		// ���̻� ��� �Ұ�?
		if (exidx == -1 && ((bSrc && exc->m_src.m_regCount >= MAX_EXCHANGE_ITEMS) || (!bSrc && exc->m_dest.m_regCount >= MAX_EXCHANGE_ITEMS)))
			return ;

		// �κ��� �ְ�, ������ ������ �ǰ�, ��ȯ �����ϰ�, �������ΰ�?
		// 060406 : bs : ���ε� ������ ��ȯ �Ұ�
		CItem* item = ch->m_inventory.FindByVirtualIndex(item_idx);

		if(item->IsBelong() == true)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_BELONG_ITEM);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}

		if (item == NULL || item->CanExchange() == false)
			return;

		if (((exidx == -1 && item->Count() < count) || (exidx != -1 && item->Count() < count + preCount)))
			return ;

#ifdef ENABLE_SUBJOB
		if( item && item->CanUseTrader( TRADE_EXCHANGE, ch->IsSubJob(JOB_SUB_TRADER) ) == false )
		{
			return;
		}
#endif //ENABLE_SUBJOB

		if( item->m_itemProto->getItemTypeIdx() == ITYPE_ACCESSORY && item->m_itemProto->getItemSubTypeIdx() == IACCESSORY_ARTIFACT )
		{
			if(dest == NULL || src == NULL)
				return ;

			int error_ = ArtifactManager::instance()->checkValidUser(dest);
			if(error_ != 0)
			{
				CNetMsg::SP rmsg(new CNetMsg);
				SysMsg(rmsg, (MSG_SYS_TYPE)error_);
				SEND_Q(rmsg, src->m_desc);
				return ;
			}

			error_ = ArtifactManager::instance()->checkValidUser(src);
			if(error_ != 0)
			{
				CNetMsg::SP rmsg(new CNetMsg);
				SysMsg(rmsg, (MSG_SYS_TYPE)error_);
				SEND_Q(rmsg, dest->m_desc);
				return ;
			}
		}

		// ���� ��� ��ų�� �ϳ��� ��� ���� ��ȯ �Ұ�
		CPet* pet = NULL;
		if (item->IsPet())
		{
			pet = ch->GetPet(item->getPlus());
			if (!pet || pet->GetSkillList()->count() != 0)
				return ;
		}
		if( item->IsAPet() )
			{
			CAPet* apet = ch->GetAPet(item->getPlus());
			if( apet && apet->IsPetEquip() )
				return ;
		}

		bool bUpdate;
		int idx = exc->Add(ch, item_idx, count, &bUpdate);
		if (idx == -1)
		{
			return ;
		}

		if (!bUpdate)
		{
			// ���̸� ���� ������ ������
			if (pet)
			{
				CNetMsg::SP rmsg(new CNetMsg);
				ExPetSellInfoMsg(rmsg, pet);
				if (bSrc)
				{
					SEND_Q(rmsg, dest->m_desc);
				}
				else
				{
					SEND_Q(rmsg, src->m_desc);
				}
			}

			CNetMsg::SP rmsg(new CNetMsg);
			ExchangeItemMsg(rmsg, (bSrc) ? MSG_EXCHANGE_ITEM_ADD_SRC : MSG_EXCHANGE_ITEM_ADD_DEST, item, count, (bSrc) ? exc->m_src.m_nasCount : exc->m_dest.m_nasCount);
			SEND_Q(rmsg, src->m_desc);
			SEND_Q(rmsg, dest->m_desc);
		}
		else
		{
			CNetMsg::SP rmsg(new CNetMsg);

			if (bSrc)
				ExchangeItemMsg(rmsg, MSG_EXCHANGE_ITEM_UPDATE_SRC, item, exc->m_src.m_count[idx], exc->m_src.m_nasCount);
			else
				ExchangeItemMsg(rmsg, MSG_EXCHANGE_ITEM_UPDATE_DEST, item, exc->m_dest.m_count[idx], exc->m_dest.m_nasCount);

			SEND_Q(rmsg, src->m_desc);
			SEND_Q(rmsg, dest->m_desc);
		}
	}
	else
	{
		item = gserver->m_itemProtoList.CreateItem(item_idx, WEARING_NONE, 0, 0, count);
		//���� ��Ŷ�� �����϶�
		if(bSrc)
		{
			if( src->m_inventory.getMoney() < count )
			{
				CNetMsg::SP rmsg(new CNetMsg);
				SysMsg(rmsg, MSG_SYS_NOT_ENOUGH_MONEY);			//���� ������
				SEND_Q(rmsg, src->m_desc);
			}
			else
				exc->m_src.m_nasCount = count;

			CNetMsg::SP rmsg(new CNetMsg);
			ExchangeItemMsg(rmsg, MSG_EXCHANGE_ITEM_UPDATE_SRC, item, 0, exc->m_src.m_nasCount);
			SEND_Q(rmsg, src->m_desc);
			SEND_Q(rmsg, dest->m_desc);
		}
		else
		{
			if( dest->m_inventory.getMoney() < count )
			{
				CNetMsg::SP rmsg(new CNetMsg);
				SysMsg(rmsg, MSG_SYS_NOT_ENOUGH_MONEY);			//���� ������
				SEND_Q(rmsg, dest->m_desc);
			}
			else
				exc->m_dest.m_nasCount = count;

			CNetMsg::SP rmsg(new CNetMsg);
			ExchangeItemMsg(rmsg, MSG_EXCHANGE_ITEM_UPDATE_DEST, item, 0, exc->m_dest.m_nasCount);
			SEND_Q(rmsg, src->m_desc);
			SEND_Q(rmsg, dest->m_desc);
		}
	}
}

void do_ExchangeItemDel(CPC* ch, CNetMsg::SP& msg)
{
	// ��ȯ ���ΰ�?
	if (ch->m_exchange == NULL)
		return ;

	CPC* his;
	bool bSrc;
	// ���� �����ΰ�?
	if (ch->m_exchange->m_src.m_ch == ch)
	{
		his = ch->m_exchange->m_dest.m_ch;
		bSrc = true;
	}
	else if (ch->m_exchange->m_dest.m_ch == ch)
	{
		his = ch->m_exchange->m_src.m_ch;
		bSrc = false;
	}
	else
		return ;

	CExchangeItems* exc = ch->m_exchange;
	CPC* src = exc->m_src.m_ch;
	CPC* dest = exc->m_dest.m_ch;

	// ������ ���� ���°� �ƴϸ� ����
	switch (exc->m_status)
	{
	case MSG_EXCHANGE_REQ_READY_SRC:
		// ���� src�̰� �غ� ����
		if (bSrc)
			return ;
		break;

	case MSG_EXCHANGE_REQ_READY_DEST:
		// ���� dest�̰� �غ� ����
		if (!bSrc)
			return ;
		break;

	case MSG_EXCHANGE_REQ_REP:
		break;

	default:
		return ;
	}

	msg->MoveFirst();

	unsigned char subtype, action;
	int item_idx;
	LONGLONG count;
	GoldType_t nasCount;

	RefMsg(msg) >> subtype
				>> action
				>> item_idx
				>> count
				>> nasCount;

	// �߰��� �������ΰ�?
	int exidx = exc->Find(ch, item_idx);
	if (exidx == -1)
		return ;

	CItem* item = NULL;
	if (bSrc)
		item = exc->m_src.m_ch->m_inventory.FindByVirtualIndex(exc->m_src.m_itemindex[exidx]);
	else
		item = exc->m_dest.m_ch->m_inventory.FindByVirtualIndex(exc->m_dest.m_itemindex[exidx]);

	if (item == NULL)
		return ;

	// �� ��ŭ ������ �ֳ�?
	LONGLONG preCount = (bSrc) ? exc->m_src.m_count[exidx] : exc->m_dest.m_count[exidx];
	if (preCount < count)
		return ;

	// ����
	int idx = exc->Del(ch, item_idx, count);

	// ���� ������ �ֳ�?
	if (idx == -1)
		return;

	{
		CNetMsg::SP rmsg(new CNetMsg);

		if ((bSrc && exc->m_src.m_count[idx] == 0) || (!bSrc && exc->m_dest.m_count[idx] == 0))
		{
			// ���� �� ����
			ExchangeItemMsg(rmsg, (bSrc) ? MSG_EXCHANGE_ITEM_DEL_SRC : MSG_EXCHANGE_ITEM_DEL_DEST, item, 0, nasCount);
		}
		else
		{
			// ���� �� ����
			if (bSrc)
				ExchangeItemMsg(rmsg, MSG_EXCHANGE_ITEM_UPDATE_SRC, item, exc->m_src.m_count[idx], nasCount);
			else
				ExchangeItemMsg(rmsg, MSG_EXCHANGE_ITEM_UPDATE_DEST, item, exc->m_dest.m_count[idx], nasCount);
		}

		SEND_Q(rmsg, src->m_desc);
		SEND_Q(rmsg, dest->m_desc);
	}
}
//