#include "stdhdrs.h"

#include "Log.h"
#include "Character.h"
#include "Server.h"
#include "CmdMsg.h"
#include "doFunc.h"
#include "WarCastle.h"
#include "DratanCastle.h"

void do_WarpCancel(CPC* ch, CNetMsg::SP& msg);
void do_WarpTeleport(CPC* ch, CNetMsg::SP& msg);
void do_WarpPrompt(CPC* ch, CNetMsg::SP& msg);
void do_WarpToReq(CPC* ch, CNetMsg::SP& msg);
void do_WarpTo(CPC* ch, CNetMsg::SP& msg);
void do_WarpTakeReq(CPC* ch, CNetMsg::SP& msg);
void do_WarpTake(CPC* ch, CNetMsg::SP& msg);
bool do_WarpFromTo(CPC* fromChar, CPC* toChar, bool recvMsg = false);
void do_TelePortCancel(CPC* ch, CNetMsg::SP& msg);
void WarpPrompttoTeleport(CPC* ch, int pos = -1);

void do_Warp(CPC* ch, CNetMsg::SP& msg)
{
	CDratanCastle * pCastle = CDratanCastle::CreateInstance();
	pCastle->CheckRespond(ch);

	unsigned char subtype = (unsigned char)-1;
	msg->MoveFirst();
	RefMsg(msg) >> subtype;

	switch (subtype)
	{
	case MSG_WARP_CANCEL:
		do_WarpCancel(ch, msg);
		break;
	case MSG_WARP_TELEPORT:
		do_WarpTeleport(ch, msg);
		break;
	case MSG_WARP_PROMPT:
		do_WarpPrompt(ch, msg);
		break;
	case MSG_WARP_TO_REQ:			// �����̵� �������� ����Ҷ�. toChar���� ���ιޱ� ��.
		do_WarpToReq(ch, msg);
		break;
	case MSG_WARP_TO:				// �����̵� �������� ���� ����. toChar => fromChar
		do_WarpTo(ch, msg);
		break;
	case MSG_WARP_TAKE_REQ:			// ��ȯ �������� ����� �� toChar���� ���� �ޱ���
		do_WarpTakeReq(ch, msg);
		break;
	case MSG_WARP_TAKE:				// ��ȯ �������� ���� ���� toChar => fromChar
		do_WarpTake(ch, msg);
		break;
	case MSG_WARP_TO_REQING:
		do_TelePortCancel(ch, msg);
		break;
	case MSG_WARP_TAKE_REQING:
		do_TelePortCancel(ch, msg);
		break;
	default:
		break;
	}
}

void do_TelePortCancel(CPC* ch, CNetMsg::SP& msg)
{
	CLCString tcharname(MAX_CHAR_NAME_LENGTH + 1);

	RefMsg(msg) >> tcharname;

	CPC* tpc = PCManager::instance()->getPlayerByName(tcharname, true );

	if( !tpc )
		return;

	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_CANCEL, ch->GetName() );
		SEND_Q(rmsg, tpc->m_desc);
	}
}

bool do_WarpFromTo(CPC* fromChar, CPC* toChar, bool recvMsg)	// fromChar �� toChar�� ���� ���Ǵ� �Լ�
{
	if( toChar->m_pZone->IsPersonalDungeon()
			|| toChar->m_pZone->IsOXQuizRoom()
			|| toChar->m_pZone->IsExtremeCube()
			|| toChar->m_pZone->IsWarGroundZone()
			|| toChar->m_pZone->IsPersonalRaid()
	  )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTALLOWZONE, toChar->GetName());
		if( !recvMsg )
		{
			SEND_Q(rmsg, fromChar->m_desc);
		}
		else
		{
			SEND_Q(rmsg, toChar->m_desc);
		}
		return false;
	}
	if( toChar->m_pZone->IsComboZone() )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTALLOWZONE, toChar->GetName());
		if( !recvMsg )
		{
			SEND_Q(rmsg, fromChar->m_desc);
		}
		else
		{
			SEND_Q(rmsg, toChar->m_desc);
		}
		return false;
	}
	if( toChar->m_pZone->IsExpedRaidZone() || toChar->m_pZone->IsPartyRaidZone() )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTALLOWZONE, toChar->GetName());
		if( !recvMsg )
		{
			SEND_Q(rmsg, fromChar->m_desc);
		}
		else
		{
			SEND_Q(rmsg, toChar->m_desc);
		}
		return false;
	}
	int canWarp = fromChar->CanWarp();
	if (canWarp != 0)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, (MSG_SYS_TYPE)canWarp);
		if( !recvMsg )
		{
			SEND_Q(rmsg, fromChar->m_desc);
		}
		else
		{
			SEND_Q(rmsg, toChar->m_desc);
		}
		return false;
	}

	// �����߿� ���� ����(MAPATT_WARZONE) ���� �̵� �Ұ�
	CWarCastle* castle = CWarCastle::GetCastleObject(toChar->m_pZone->m_index);
	if (castle)
	{
		if (castle->GetState() != WCSF_NORMAL)
		{
#ifdef BUGFIX_WARP_TO_WARZONE
			if(castle->CheckSubServer() && castle->IsInInnerCastle(toChar))
#else
			if ( toChar->GetMapAttr() & MATT_WAR)
#endif
			{
				CNetMsg::SP rmsg(new CNetMsg);
				SysMsg(rmsg, MSG_SYS_MEMPOS_CASTLE);
				if( !recvMsg )
				{
					SEND_Q(rmsg, fromChar->m_desc);
				}
				else
				{
					SEND_Q(rmsg, toChar->m_desc);
				}
				return false;
			}
		}
	}

	if(fromChar->m_pZone->m_index != toChar->m_pZone->m_index)
		fromChar->SetPlayerState(PLAYER_STATE_CASH_ZONE_MOVE);

	if(!GoZone(fromChar, toChar->m_pZone->m_index, toChar->m_pos.m_yLayer, toChar->m_pos.m_x, toChar->m_pos.m_z))
	{
		if(fromChar->IsSetPlayerState(PLAYER_STATE_CASH_ZONE_MOVE))
			fromChar->ResetPlayerState(PLAYER_STATE_CASH_ZONE_MOVE);
	}

	if(!recvMsg )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpEndMsg(rmsg, fromChar);
		fromChar->m_pArea->SendToCell(rmsg, fromChar, true);
	}
	else
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpEndMsg(rmsg, toChar);
		toChar->m_pArea->SendToCell(rmsg, toChar, true);
	}

	return true;
}

void do_WarpTake(CPC* ch, CNetMsg::SP& msg)
{
	// ch�� ��ȯ�� �ɸ�
	CLCString scharName(MAX_CHAR_NAME_LENGTH + 1);
	char ballow = false;

	RefMsg(msg) >> scharName
				>> ballow;

	if(findPercentChar(scharName.getBuffer()) != NULL)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_CANCEL, ch->GetName() );
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	CPC* sPC = PCManager::instance()->getPlayerByName(scharName, true );

	if( !sPC )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_CANCEL, ch->GetName() );
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	if( !ballow )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTALLOW, ch->GetName() );
		SEND_Q(rmsg, sPC->m_desc);
		return;
	}

	// ������ �˻�
	CItem* item = sPC->m_inventory.FindByDBIndex(858, 0, 0);
	if (item == NULL)
		return;

	if( !do_WarpFromTo(ch, sPC, true) )
	{
		return;
	}

	if( (item->m_itemProto->getItemFlag() & ITEM_FLAG_CASH) )
	{
		GAMELOG << init("CASH_ITEM_USE", sPC)
				<< itemlog(item)
				<< end;
	}

	sPC->m_inventory.decreaseItemCount(item, 1);
}

void do_WarpTakeReq(CPC* ch, CNetMsg::SP& msg)
{
	// ������ �˻�
	if (ch->m_inventory.FindByDBIndex(858, 0, 0) == NULL)
		return ;

	//��ȯ �Ϸ��� ����� rvr ���� �ִ� ���
	if (ch->m_pZone->isRVRZone())
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTALLOWZONE, ch->GetName());
		SEND_Q( rmsg, ch->m_desc);
		return ;
	}

	// ch�� ����� �ɸ�
	CLCString tcharName(MAX_CHAR_NAME_LENGTH + 1);

	RefMsg(msg) >> tcharName;

	CPC * tPC = PCManager::instance()->getPlayerByName(tcharName, true );

	if( !tPC )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTCHAR, tcharName);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	if( tPC->m_plusEffect & CLIENT_OPTION_WARP )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTALLOW, tcharName);
		SEND_Q( rmsg, ch->m_desc);
		return;
	}

	if( ch->m_pZone->IsPersonalDungeon()
			|| ch->m_pZone->IsWarGroundZone()
			|| ch->m_pZone->IsPersonalRaid()
	  )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTALLOWZONE, ch->GetName());

		SEND_Q( rmsg, ch->m_desc);
		return;
	}
	if( ch->m_pZone->IsSummonZone() )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTALLOWZONE, ch->GetName() );
		SEND_Q( rmsg, ch->m_desc );
		return;
	}

	if( tPC->m_pZone->IsPersonalDungeon() ||
			tPC->m_pZone->IsPersonalRaid() ||
			tPC->m_pZone->IsWarGroundZone() ||
			tPC->m_pZone->IsExpedRaidZone() ||
			tPC->m_pZone->IsPartyRaidZone()
	  )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTALLOWZONE, ch->GetName());
		SEND_Q( rmsg, ch->m_desc);
		return;
	}
	// �� �޼��� ����
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpReqIngMsg(rmsg, MSG_WARP_TAKE_REQING);
		SEND_Q(rmsg, ch->m_desc);
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpReqMsg(rmsg, ch, MSG_WARP_TAKE_REQ);
		SEND_Q(rmsg, tPC->m_desc);
	}
}

void do_WarpTo(CPC* ch, CNetMsg::SP& msg)
{
	// ch�� ������ �ִ� �ɸ�
	CLCString scharName(MAX_CHAR_NAME_LENGTH + 1);
	char ballow = false;

	RefMsg(msg) >> scharName
				>> ballow;

	CPC* sPC = PCManager::instance()->getPlayerByName(scharName, true );

	if( !sPC )
	{
		return;
	}

	if( !ballow )		// �����̵� ������ ������ ��.
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTALLOW, scharName);
		SEND_Q(rmsg, sPC->m_desc);
		return;
	}

	// ������ �˻�
	CItem* item = sPC->m_inventory.FindByDBIndex(857, 0, 0);
	if (item == NULL)
		return ;

	//�̵��Ϸ��� Ÿ���� rvr ���� �ִ� ���
	if (ch->m_pZone->isRVRZone())
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTALLOWZONE, ch->GetName());
		SEND_Q( rmsg, sPC->m_desc);
		return ;
	}

	if( !do_WarpFromTo(sPC, ch, false) )
	{
		return;
	}

	if( (item->m_itemProto->getItemFlag() & ITEM_FLAG_CASH) )
	{
		GAMELOG << init("CASH_ITEM_USE", sPC)
				<< itemlog(item)
				<< end;
	}

	sPC->m_inventory.decreaseItemCount(item, 1);
}

bool isPossibleWarp(CPC* fromChar, CPC* toChar)
{
	if (toChar->m_plusEffect & CLIENT_OPTION_WARP)
	{
		return false;
	}

	if (toChar->m_pZone->IsGuildRoom())
	{
		if (fromChar->m_guildInfo == NULL || fromChar->m_guildInfo->guild() == NULL)
		{
			return false;
		}
	}

	if (toChar->m_pZone->IsPersonalDungeon()
		|| toChar->m_pZone->IsWarGroundZone()
		|| toChar->m_pZone->IsPersonalRaid()
		|| toChar->m_pZone->IsSummonZone()
		|| toChar->m_pZone->IsExpedRaidZone()
		|| toChar->m_pZone->isRVRZone()
		|| toChar->m_pZone->IsOXQuizRoom()
		|| toChar->m_pZone->IsComboZone()
		|| toChar->m_pZone->IsPartyRaidZone())
	{
		return false;
	}

#ifdef EXTREME_CUBE
	if (toChar->m_pZone->IsExtremeCube() )
	{
		return false;
	}

	if (fromChar->m_pZone->IsExtremeCube() || fromChar->m_pZone->IsWarGroundZone() || fromChar->m_pZone->IsPersonalRaid())
	{
		return false;
	}
#endif

	// �����߿� ���� ����(MAPATT_WARZONE) ���� �̵� �Ұ�
	CWarCastle* castle = CWarCastle::GetCastleObject(toChar->m_pZone->m_index);
	if (castle)
	{
		if (castle->GetState() != WCSF_NORMAL)
		{
#ifdef BUGFIX_WARP_TO_WARZONE
			if(castle->CheckSubServer() && castle->IsInInnerCastle(toChar))
#else
			if ( toChar->GetMapAttr() & MATT_WAR || toChar->m_pZone->IsWarZone(GET_X(toChar), GET_Z(toChar)) == true ) 
#endif
			{
				return false;
			}
		}
	}

	return true;
}

void do_WarpToReq(CPC* ch, CNetMsg::SP& msg)
{
	// ������ �˻�
	if (ch->m_inventory.FindByDBIndex(857, 0, 0) == NULL)
		return ;

	// ch�� ����� �ɸ�
	CLCString tcharName(MAX_CHAR_NAME_LENGTH + 1);

	RefMsg(msg) >> tcharName;

	if(findPercentChar(tcharName.getBuffer()) != NULL)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTCHAR, tcharName);
		SEND_Q( rmsg, ch->m_desc);
		return;
	}

	CPC * tPC = PCManager::instance()->getPlayerByName(tcharName, true );

	if( !tPC )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTCHAR, tcharName);
		SEND_Q( rmsg, ch->m_desc);
		return;
	}

	if (ch->CanWarp() != 0)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTALLOW, tcharName);
		SEND_Q( rmsg, ch->m_desc);
		return;
	}

	if (isPossibleWarp(ch, tPC) == false)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpErrorMsg(rmsg, MSG_WARP_ERROR_NOTALLOWZONE, tcharName);
		SEND_Q( rmsg, ch->m_desc);
		return;
	}
	
	// �� �޼��� ����
	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpReqIngMsg(rmsg, MSG_WARP_TO_REQING);
		SEND_Q(rmsg, ch->m_desc);
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpReqMsg(rmsg, ch, MSG_WARP_TO_REQ);
		SEND_Q(rmsg, tPC->m_desc);
	}
}

void do_WarpCancel(CPC* ch, CNetMsg::SP& msg)
{
	ch->m_reqWarpTime = 0;
	ch->m_reqWarpType = -1;
	ch->m_reqWarpData = -1;
	ch->ResetPlayerState(PLAYER_STATE_SITDOWN | PLAYER_STATE_MOVING | PLAYER_STATE_WARP);

	CNetMsg::SP rmsg(new CNetMsg);
	WarpCancelMsg(rmsg, ch);
	ch->m_pArea->SendToCell(rmsg, ch, true);
}

void do_WarpTeleport(CPC* ch, CNetMsg::SP& msg)
{
	int pos = -1;
	RefMsg(msg) >> pos;

	// �̱۴��� ��ġ�е� �ε���(999) : �߷� bw
	if ( (pos != 999 && (pos < 0 || pos >= ch->m_pZone->m_countZonePos))
			&& (pos != 998 && (pos < 0 || pos >= ch->m_pZone->m_countZonePos)) )
		return ;

	bool bSendGoZoneEffect = false;
	// �޶�ũ 4, 5, 6, 7�� ������ �������� ����
	if (ch->m_admin < 2 && ch->m_pZone->m_index == CWarCastle::GetCurSubServerCastleZoneIndex())
	{
		CWarCastle* castle = CWarCastle::GetCastleObject(ch->m_pZone->m_index);
		if (castle && castle->IsDefenseWarpPoint(pos))
		{
			if (castle->GetState() == WCSF_NORMAL)
				return ;
			if (!IS_DEFENSE_TEAM(ch->GetJoinFlag(ch->m_pZone->m_index)))
				return ;

			bSendGoZoneEffect = true;
		}
	}

	// ���ź ���� Ÿ�� 13 - 17 �� ������ ���� ��常 ����
	if (ch->m_admin < 2 && ch->m_pZone->m_index == ZONE_DRATAN)
	{
		CDratanCastle * pCastle = CDratanCastle::CreateInstance();
		if (pCastle->IsDefenseWarpPoint(pos))
		{
			if (pCastle->GetState() == WCSF_NORMAL)
			{
				return;
			}

			if (!IS_DEFENSE_TEAM(ch->GetJoinFlag(ch->m_pZone->m_index)))
			{
				return;
			}

			bSendGoZoneEffect = true;
		}
	}

	// �� �� ������� �� �������, �ٸ� ����� ����� �˸���
	ch->SendDisappearAllInCell(true);

	// ������ ����
	ch->m_pArea->CharFromCell(ch, true);

	if( ch->m_pZone->m_index == ZONE_SINGLE_DUNGEON1)
	{
		// �߷� ������ ���� �̵�
		if( pos == 999 )
		{
			GET_YLAYER(ch)	= 0;
			GET_X(ch)		= 80;
			GET_Z(ch)		= 240;
		}
		else
		{
			GET_YLAYER(ch)	= 0;
			GET_X(ch)		= 120;
			GET_Z(ch)		= 120;
			ch->m_pArea->m_bBoss = true;
		}
	}
	else
	{
		if (pos < 0 || pos >= ch->m_pZone->m_countZonePos)
		{
			LOG_ERROR("HACKING? : invalid pos data. pos[%d], charIndex[%d]", pos, ch->m_index);
			ch->m_desc->Close("invalid pos data");
			return;
		}
		
		// ĳ������ ��ġ ����
		GET_YLAYER(ch)	= ch->m_pZone->m_zonePos[pos][0];
		GET_X(ch)		= GetRandom(ch->m_pZone->m_zonePos[pos][1], ch->m_pZone->m_zonePos[pos][3]) / 2.0f;
		GET_Z(ch)		= GetRandom(ch->m_pZone->m_zonePos[pos][2], ch->m_pZone->m_zonePos[pos][4]) / 2.0f;
	}
	ch->ResetPlayerState(PLAYER_STATE_SITDOWN | PLAYER_STATE_MOVING);

	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpTeleportMsg(rmsg, pos, ch);
		SEND_Q(rmsg, ch->m_desc);
	}

	int cx, cz;
	ch->m_pArea->PointToCellNum(GET_X(ch), GET_Z(ch), &cx, &cz);
	ch->m_pArea->CharToCell(ch, GET_YLAYER(ch), cx, cz);

	{
		CNetMsg::SP rmsg(new CNetMsg);
		AppearMsg(rmsg, ch, true);
		ch->m_pArea->SendToCell(rmsg, ch);
	}
	ch->m_pArea->SendCellInfo(ch, true);

	if( ch->m_guildInfo )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		GuildPointRankingMsg( rmsg, ch->m_index, ch->m_guildInfo->guild()->index(), ch->m_guildInfo->guild()->GetGuildPointRanking() );
		ch->m_pArea->SendToCell( rmsg, ch );
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		GuildNameColorStateMsg(rmsg, ch );
		ch->m_pArea->SendToCell( rmsg, ch );
	}

	if (ch->GetPet())
	{
		CPet* pet = ch->GetPet();
		if (pet->IsSummon())
			GoTo(pet, GET_YLAYER(ch), GET_X(ch), GET_Z(ch), GET_H(ch), GET_R(pet));
	}

	CAPet *apet = ch->GetAPet();
	if( apet && apet->IsSummon() )
	{
		GoTo(apet, GET_YLAYER(ch), GET_X(ch), GET_Z(ch), GET_H(ch), GET_R(apet));
	}

	if (ch->m_elementalList)
	{
		CElemental* pElemental = ch->m_elementalList;
		while (pElemental)
		{
			GoTo(pElemental, GET_YLAYER(ch), GET_X(ch), GET_Z(ch), GET_H(ch), GET_R(pElemental));
			pElemental = pElemental->m_nextElemental;
		}
	}

	if (bSendGoZoneEffect)
	{
		// GO_ZONE ȿ�� �޽��� ����
		CNetMsg::SP rmsg(new CNetMsg);
		EffectEtcMsg(rmsg, ch, MSG_EFFECT_ETC_GOZONE);
		ch->m_pArea->SendToCell(rmsg, ch, true);
	}
}

void do_WarpPrompt(CPC* ch, CNetMsg::SP& msg)
{
	int zoneindex;
	int pos;

	RefMsg(msg) >> zoneindex
				>> pos;
	if(ch->m_pZone->m_index != zoneindex)
	{
		CZone* pZone = gserver->FindZone(zoneindex);
		if (pZone == NULL)
			return;

		if (pos < 0 || pos >= pZone->m_countZonePos)
			return ;

		if( pZone->m_index == ZONE_SINGLE_DUNGEON1)
		{
			// �߷� ������ ���� �̵�
			GoZone(ch, pZone->m_index,
				   GET_YLAYER(ch),
				   80,
				   240);
		}
		else
		{
			// ������ ���� ���� ��尡 �ƴ� ĳ���ʹ� ���� �̵� ����
			CDratanCastle * pCastle = CDratanCastle::CreateInstance();
			if(pZone->m_index == ZONE_DRATAN
					&& pCastle->GetState() != WCSF_NORMAL
					&& ch->GetJoinFlag(pZone->m_index) == WCJF_ATTACK_GUILD
					&& pos == 25) // �������� ��ġ 25
				//			&& ch->m_admin < 2)
			{
				return;
			}

			GoZone(ch, pZone->m_index,
				   pZone->m_zonePos[pos][0],
				   GetRandom(pZone->m_zonePos[pos][1], pZone->m_zonePos[pos][3]) / 2.0f,
				   GetRandom(pZone->m_zonePos[pos][2], pZone->m_zonePos[pos][4]) / 2.0f);
		}

		// �̵��� 3�ʰ� ����
		ch->SetDisableTime(3);
	}
	else
	{
		//�ڷ���Ʈ
		WarpPrompttoTeleport(ch, pos);
	}
}

void WarpPrompttoTeleport(CPC* ch, int pos)
{
	// �̱۴��� ��ġ�е� �ε���(999) : �߷� bw
	if ( (pos != 999 && (pos < 0 || pos >= ch->m_pZone->m_countZonePos))
			&& (pos != 998 && (pos < 0 || pos >= ch->m_pZone->m_countZonePos)) )
		return ;

	bool bSendGoZoneEffect = false;
	// �޶�ũ 4, 5, 6, 7�� ������ �������� ����
	if (ch->m_admin < 2 && ch->m_pZone->m_index == CWarCastle::GetCurSubServerCastleZoneIndex())
	{
		CWarCastle* castle = CWarCastle::GetCastleObject(ch->m_pZone->m_index);
		if (castle && castle->IsDefenseWarpPoint(pos))
		{
			if (castle->GetState() == WCSF_NORMAL)
				return ;
			if (!IS_DEFENSE_TEAM(ch->GetJoinFlag(ch->m_pZone->m_index)))
				return ;

			bSendGoZoneEffect = true;
		}
	}

	// ���ź ���� Ÿ�� 13 - 17 �� ������ ���� ��常 ����
	if (ch->m_admin < 2 && ch->m_pZone->m_index == ZONE_DRATAN)
	{
		CDratanCastle * pCastle = CDratanCastle::CreateInstance();
		if (pCastle->IsDefenseWarpPoint(pos))
		{
			if (pCastle->GetState() == WCSF_NORMAL)
			{
				return;
			}

			if (!IS_DEFENSE_TEAM(ch->GetJoinFlag(ch->m_pZone->m_index)))
			{
				return;
			}

			bSendGoZoneEffect = true;
		}
	}

	// �� �� ������� �� �������, �ٸ� ����� ����� �˸���
	ch->SendDisappearAllInCell(true);

	// ������ ����
	ch->m_pArea->CharFromCell(ch, true);

	if( ch->m_pZone->m_index == ZONE_SINGLE_DUNGEON1)
	{
		// �߷� ������ ���� �̵�
		if( pos == 999 )
		{
			GET_YLAYER(ch)	= 0;
			GET_X(ch)		= 80;
			GET_Z(ch)		= 240;
		}
		else
		{
			GET_YLAYER(ch)	= 0;
			GET_X(ch)		= 120;
			GET_Z(ch)		= 120;
			ch->m_pArea->m_bBoss = true;
		}
	}
	else
	{
		// ĳ������ ��ġ ����
		GET_YLAYER(ch)	= ch->m_pZone->m_zonePos[pos][0];
		GET_X(ch)		= GetRandom(ch->m_pZone->m_zonePos[pos][1], ch->m_pZone->m_zonePos[pos][3]) / 2.0f;
		GET_Z(ch)		= GetRandom(ch->m_pZone->m_zonePos[pos][2], ch->m_pZone->m_zonePos[pos][4]) / 2.0f;
	}
	ch->ResetPlayerState(PLAYER_STATE_SITDOWN | PLAYER_STATE_MOVING);

	{
		CNetMsg::SP rmsg(new CNetMsg);
		WarpTeleportMsg(rmsg, pos, ch);
		SEND_Q(rmsg, ch->m_desc);
	}

	int cx, cz;
	ch->m_pArea->PointToCellNum(GET_X(ch), GET_Z(ch), &cx, &cz);
	ch->m_pArea->CharToCell(ch, GET_YLAYER(ch), cx, cz);

	{
		CNetMsg::SP rmsg(new CNetMsg);
		AppearMsg(rmsg, ch, true);
		ch->m_pArea->SendToCell(rmsg, ch);
	}

	ch->m_pArea->SendCellInfo(ch, true);

	if( ch->m_guildInfo )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		GuildPointRankingMsg( rmsg, ch->m_index, ch->m_guildInfo->guild()->index(), ch->m_guildInfo->guild()->GetGuildPointRanking() );
		ch->m_pArea->SendToCell( rmsg, ch );
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		GuildNameColorStateMsg(rmsg, ch );
		ch->m_pArea->SendToCell( rmsg, ch );
	}

	if (ch->GetPet())
	{
		CPet* pet = ch->GetPet();
		if (pet->IsSummon())
			GoTo(pet, GET_YLAYER(ch), GET_X(ch), GET_Z(ch), GET_H(ch), GET_R(pet));
	}

	CAPet *apet = ch->GetAPet();
	if( apet && apet->IsSummon() )
	{
		GoTo(apet, GET_YLAYER(ch), GET_X(ch), GET_Z(ch), GET_H(ch), GET_R(apet));
	}

	if (ch->m_elementalList)
	{
		CElemental* pElemental = ch->m_elementalList;
		while (pElemental)
		{
			GoTo(pElemental, GET_YLAYER(ch), GET_X(ch), GET_Z(ch), GET_H(ch), GET_R(pElemental));
			pElemental = pElemental->m_nextElemental;
		}
	}

	if (bSendGoZoneEffect)
	{
		// GO_ZONE ȿ�� �޽��� ����
		CNetMsg::SP rmsg(new CNetMsg);
		EffectEtcMsg(rmsg, ch, MSG_EFFECT_ETC_GOZONE);
		ch->m_pArea->SendToCell(rmsg, ch, true);
	}
}
