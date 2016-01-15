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


void do_Raid(CPC* ch, CNetMsg::SP& msg)
{
	// �װų� ��ȯ���̸� ����
	if (!ch || DEAD(ch) || ch->m_currentSkill != NULL || ch->m_personalshop != NULL)
		return ;

	msg->MoveFirst();

	unsigned char subtype;
	RefMsg(msg) >> subtype;

	switch (subtype)
	{
	case MSG_RAID_INZONE_JOIN:
		do_Raid_InzoneJoin(ch, msg);
		break;
	case MSG_RAID_INZONE_QUIT:
		do_Raid_InzoneQuit(ch, msg);
		break;
	}
}

//�� ���� ���� ���� ��û
//������ ȥ�ڼ��� ���� �� �� ����
void do_Raid_InzoneJoin(CPC* ch, CNetMsg::SP& msg)
{
	int nZoneNo;	//����ȣ(��)
	int nDifficulty = -1;

	RefMsg(msg) >> nZoneNo;
	RefMsg(msg) >> nDifficulty;
	if(nDifficulty == -1 && nZoneNo == ZONE_AKAN_TEMPLE)
		nDifficulty = 1;
	if(nDifficulty == -1 && nZoneNo == ZONE_TARIAN_DUNGEON)
		nDifficulty = 0;

	GAMELOG << init("RAID DEBUG INZONE JOIN REQ", ch)
			<< "nZoneNo" << delim << nZoneNo;

	CZone* pZone = gserver->FindZone(nZoneNo);
	if (pZone == NULL)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_NOT_FOUND_ZONE_INFO);
		SEND_Q(rmsg, ch->m_desc);
		
		GAMELOG << " CheckZone Failed" << end;
		return ;
	}

#ifdef LC_GAMIGO
	if(pZone->m_countArea < 2)
	{
		GAMELOG << " CountArea Failed" << end;
		return ;
	}
#endif // LC_GAMIGO

	GAMELOG << end;

	if(gserver->m_bCanEnterTheRaidDungeon == false) // ���̵� ������ �� �� ���� ��Ȳ�̶��...false�� ������.
	{
		//�����޽��� �߼�
		//MSG_RAID_ERROR_INZONE_INIT_TIME
		CNetMsg::SP rmsg(new CNetMsg);
		RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_INIT_TIME);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	if(ch->IsSetPlayerState(PLAYER_STATE_WARP))
	{
		// �̵��� �� ���� �����Դϴ�.
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_CANT_WARP_OTHER);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	if(!ch->IsParty() && !ch->IsExped())
	{
		//����: ��Ƽ �� �����뿡 ���� �Ǿ� ���� ����
		CNetMsg::SP rmsg(new CNetMsg);
		RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_JOIN_NOTJOIN_PARTYOREXPED);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}
	if( nZoneNo == ZONE_TARIAN_DUNGEON)
	{
		if( !(ch->m_questList.FindQuest(617, QUEST_STATE_DONE)) )
		{
			CNetMsg::SP rmsg(new CNetMsg);
			RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_NOT_QUEST);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}
	}
	//������ ����(������)
	if((nZoneNo == ZONE_ALTER_OF_DARK || nZoneNo == ZONE_AKAN_TEMPLE
			|| nZoneNo == ZONE_TARIAN_DUNGEON
	   ) && ch->m_Exped==NULL)
	{
		//����: �ش� ���� ������ ��
		CNetMsg::SP rmsg(new CNetMsg);
		RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_JOIN_THISZONE_EXPEDZONE);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	//�����(��Ƽ)
	if(nZoneNo == ZONE_CAPPELLA_1 && !ch->IsParty())
	{
		//����: �ش� ���� ��Ƽ ��
		CNetMsg::SP rmsg(new CNetMsg);
		RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_JOIN_THISZONE_PARTYZONE);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	// ���̵� ���� ī�尡 ������ ���� ���� 10 ���� ���� ���ѵȴ�.
	// �κ��丮 ������ �˻�, ���̵� ���� ī��
	if (ch->m_inventory.FindByDBIndex(4913, 0, 0))
	{
		//���� ���� PC���� ����
		if( (nZoneNo == ZONE_ALTER_OF_DARK && (ch->m_level < RAID_JOIN_LEVEL_LIMIT_ALTER_OF_DARK - 10)) ||
				(nZoneNo == ZONE_CAPPELLA_1 && (ch->m_level < RAID_JOIN_LEVEL_LIMIT_CAPPELLA - 10)))
		{
			//����: ���� �������� �����Ҽ� ����
			CNetMsg::SP rmsg(new CNetMsg);
			RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_JOIN_INVALIDELEVEL);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}

		if( nZoneNo == ZONE_AKAN_TEMPLE
				&& ((ch->m_level < RAID_JOIN_LEVEL_LOW_LIMIT_AKAN_TEMPLE - 10) || (ch->m_level > RAID_JOIN_LEVEL_HIGH_LIMIT_AKAN_TEMPLE))
				&& nDifficulty == DUNGEON_DIFFICULTY_HARD)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_JOIN_INVALIDELEVEL);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}
		if( nZoneNo == ZONE_AKAN_TEMPLE
				&& ((ch->m_level < RAID_JOIN_LEVEL_LOW_LIMIT_AKAN_TEMPLE_NORMAL - 10) || (ch->m_level > RAID_JOIN_LEVEL_HIGH_LIMIT_AKAN_TEMPLE_NORMAL))
				&& nDifficulty == DUNGEON_DIFFICULTY_NORMAL)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_JOIN_INVALIDELEVEL);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}
		if( nZoneNo == ZONE_TARIAN_DUNGEON
				&& (ch->m_level < RAID_JOIN_LEVEL_LOW_LIMIT_TARIAN_DUNGEON - 10)
				&& nDifficulty == DUNGEON_DIFFICULTY_NORMAL)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_JOIN_INVALIDELEVEL);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}
	}
	else

		//���� ���� PC���� ����
		if( (nZoneNo == ZONE_ALTER_OF_DARK && (ch->m_level < RAID_JOIN_LEVEL_LIMIT_ALTER_OF_DARK)) ||
				(nZoneNo == ZONE_CAPPELLA_1 && (ch->m_level < RAID_JOIN_LEVEL_LIMIT_CAPPELLA)))
		{
			//����: ���� �������� �����Ҽ� ����
			CNetMsg::SP rmsg(new CNetMsg);
			RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_JOIN_INVALIDELEVEL);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}
		else if( nZoneNo == ZONE_AKAN_TEMPLE
				 && ((ch->m_level < RAID_JOIN_LEVEL_LOW_LIMIT_AKAN_TEMPLE_NORMAL) || (ch->m_level > RAID_JOIN_LEVEL_HIGH_LIMIT_AKAN_TEMPLE_NORMAL))
				 && nDifficulty == DUNGEON_DIFFICULTY_NORMAL)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_JOIN_INVALIDELEVEL);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}
		else if( nZoneNo == ZONE_AKAN_TEMPLE
				 && ((ch->m_level < RAID_JOIN_LEVEL_LOW_LIMIT_AKAN_TEMPLE) || (ch->m_level > RAID_JOIN_LEVEL_HIGH_LIMIT_AKAN_TEMPLE))
				 && nDifficulty == DUNGEON_DIFFICULTY_HARD)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_JOIN_INVALIDELEVEL);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}
		else if( nZoneNo == ZONE_TARIAN_DUNGEON
				 && (ch->m_level < RAID_JOIN_LEVEL_LOW_LIMIT_TARIAN_DUNGEON)
				 && nDifficulty == DUNGEON_DIFFICULTY_NORMAL)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_JOIN_INVALIDELEVEL);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}

	// �������̸� ������ ���̵� ä�� �˻�
	if (ch->IsExped() && ch->m_Exped->m_nSubNumber != RAID_SUBNUMBER)
	{
		//����: ���� ä���� ������ ���̵� ä���� �ƴ�
		CNetMsg::SP rmsg(new CNetMsg);
		RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_NOT_EXPED_RAID_SUBNUMBER);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	// ��Ƽ�̸� ��Ƽ ���̵� ä�� �˻�
	else if (ch->IsParty() && ch->m_party->m_nSubNumber != RAID_SUBNUMBER)
	{
		//����: ���� ä���� ������ ���̵� ä���� �ƴ�
		CNetMsg::SP rmsg(new CNetMsg);
		RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_NOT_EXPED_RAID_SUBNUMBER);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	int nBossIndex = 0;
	int nJoinType=0;

	if(ch->IsParty())
	{
		nBossIndex = ch->m_party->GetBossIndex();
		nJoinType = MSG_JOINTYPE_PARTY;
	}

	else if(ch->IsExped())
	{
		nBossIndex = ch->m_Exped->GetBossIndex();
		nJoinType = MSG_JOINTYPE_EXPED;
	}

	// ������ �̸鼭 ���������� �ƴϰ� ���������� �� ���� �ʴٸ� ���� �Ұ�
	if (ch->IsExped() && ch->m_index != nBossIndex && ch->m_Exped->m_nRaidAreaNum == -1)
	{
		//����: ���������� ������ ���� ����
		CNetMsg::SP rmsg(new CNetMsg);
		RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_NOT_JOIN_BOSS);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	if(ch->IsExped())
	{
		CPC* pPCBoss = NULL;
		CExpedMember* pMember = NULL;
		pMember = ch->m_Exped->GetMemberByCharIndex(nBossIndex);
		if(pMember == NULL)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_NOT_JOIN_BOSS);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}

		pPCBoss = pMember->GetMemberPCPtr();
		if(pPCBoss == NULL)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_NOT_JOIN_BOSS);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}

		if(pPCBoss->m_index != ch->m_index && pPCBoss->m_pZone->m_index != nZoneNo)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_NOT_JOIN_BOSS);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}
	}

	// ������� �̸鼭 ���������� (Normal || Hard)�ε� ��������� �� �ݴ� ���̵��� �����Ϸ��� ���
	// ���������� �ִ����� ������ �˻縦 �����ϱ�...
	// ���� ĳ���Ͱ� ������ ���̵��� �������� ���̵��� Ʋ��. �׷��� ���� �Ұ�...
	if( ch->IsExped() &&
			nZoneNo == ZONE_AKAN_TEMPLE &&
			ch->m_Exped->m_nDifficulty > -1 &&
			ch->m_Exped->m_nDifficulty != nDifficulty )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		RaidErrorMsg(rmsg, MSG_RAID_ERROR_INZONE_INVALID_DIFFICULTY);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	if (ch->m_exchange)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExchangeReqMsg(rmsg, MSG_EXCHANGE_REQ_REJECT, NULL, NULL);
		do_Exchange(ch, rmsg);
	}
	//��� ��� ��û
	{
		ch->SetPlayerState(PLAYER_STATE_WARP);

		CNetMsg::SP rmsg(new CNetMsg);
		HelperRaidInzoneGetRoomNoReqMsg(rmsg, ch->m_index, nZoneNo, nBossIndex, nJoinType);
		RefMsg(rmsg) << nDifficulty;
		SEND_Q(rmsg, gserver->m_helper);
	}
}

//������ ����(������)
void do_Raid_InzoneQuit(CPC* ch, CNetMsg::SP& msg)
{
	int i;

	int nZoneNo = -1;		//����ȣ(����)
	int nPcZoneNo,nPcRoomNo;
	int nExtra = -1;			//��ġ �������� ����
	int nGoZone=1;		//���̵� �ϳ�?(1:�̵� 0:�̵� ����)
	int nRebirth=0;		//��Ȱ�ΰ�?(1:��Ȱ)
	CZone* pCurZone=NULL;

	if(msg->CanRead(sizeof(nGoZone)))	//����Ÿ�� ���� ��쿡�� ó��
	{
		RefMsg(msg) >> nGoZone
					>> nRebirth
					>> nZoneNo;
		RefMsg(msg) >> nExtra;
	}

	if(ch->IsSetPlayerState(PLAYER_STATE_WARP))
	{
		// �̵��� �� ���� �����Դϴ�.
		CNetMsg::SP rmsg(new CNetMsg);
		SysMsg(rmsg, MSG_SYS_CANT_WARP_OTHER);
		SEND_Q(rmsg, ch->m_desc);
		goto ERROR_RETURN;
	}

	if(nZoneNo >= 0)
	{
		CZone* pGoZone = gserver->FindZone(nZoneNo);
		if (pGoZone == NULL)
			return ;

		if( ch->m_level < pGoZone->m_minLevel || ch->m_level > pGoZone->m_maxLevel )
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg( rmsg, (MSG_SYS_TYPE) MSG_SYS_CANNOT_GOZONE_LEVEL );
			SEND_Q( rmsg, ch->m_desc );
			return;
		}
	}

	ch->SetPlayerState(PLAYER_STATE_WARP);

	GAMELOG << init("RAID DEBUG INZONE QUIT REQ", ch)
			<< "nGoZone" << delim << nGoZone  << delim
			<< "nRebirth" << delim << nRebirth
			<< end;

	//����ȣ ,���ȣ ���
	nPcZoneNo = ch->m_nJoinInzone_ZoneNo;
	nPcRoomNo = ch->m_nJoinInzone_RoomNo;

	pCurZone = gserver->FindZone(ch->m_nJoinInzone_ZoneNo);
	if(pCurZone && pCurZone->m_area)
	{
		i = ch->m_nJoinInzone_AreaNo;
		if(i == -1)
			goto ERROR_RETURN;

		CArea* area;
		area = pCurZone->m_area + i;
		if(!area)
			goto ERROR_RETURN;

		if(ch->m_nJoinInzone_ZoneNo==ZONE_ALTER_OF_DARK)	// ���� ��: ������ ���
		{
			nZoneNo = ZONE_MONDSHINE;

			if(nRebirth==1)
			{
				nExtra = 4;
			}
			else
			{
				nExtra = 3;
			}
		}
		else if(ch->m_nJoinInzone_ZoneNo==ZONE_CAPPELLA_1 && nZoneNo == -1)	// ���� ��: ����� 1, 2
		{
			nZoneNo = ZONE_MONDSHINE;
			if(nRebirth==1)
			{
				nExtra = 2;
			}
			else
			{
				nExtra = 1;
			}
		}
		else if(ch->m_nJoinInzone_ZoneNo==ZONE_CAPPELLA_1 && nZoneNo != -1)	// ����翡�� ����� ���Ϸ� ����.
		{
			if(nZoneNo == ZONE_MONDSHINE)
				nExtra = 1;
			else if(nZoneNo == ZONE_CAPPELLA_2)
				nExtra = 0;
			else
				goto ERROR_RETURN;
		}
		else if(ch->m_nJoinInzone_ZoneNo == ZONE_AKAN_TEMPLE)
		{
			if(nExtra == -1)
				nExtra = 4;
			// ��ĭ������� ���� ���� �� ����..
			if(nRebirth == 1)
			{
				nZoneNo = ZONE_EGEHA;
//				nExtra = 0;
			}
			else if(nRebirth == 0)
			{
				nZoneNo = ZONE_EGEHA;		// �����Ϸ� ƨ�ܹ���!
//				nExtra = 0;
			}
			else
				goto ERROR_RETURN;
		}
		else if(ch->m_nJoinInzone_ZoneNo == ZONE_TARIAN_DUNGEON)
		{
			if(nExtra == -1)
				nExtra = 1;
			// ��ĭ������� ���� ���� �� ����..
			if(nRebirth == 1)
			{
				nZoneNo = ZONE_TARIAN;
//				nExtra = 0;
			}
			else if(nRebirth == 0)
			{
				nZoneNo = ZONE_TARIAN;		// �����Ϸ� ƨ�ܹ���!
//				nExtra = 0;
			}
			else
				goto ERROR_RETURN;
		}
		else
		{
			goto ERROR_RETURN;
		}

		if(ch->IsExped())
		{
			ch->SetExpedLabel(-1);

			{
				CNetMsg::SP rmsg(new CNetMsg);
				ExpedSetLabelRepMsg(rmsg, MSG_EXPED_SETLABEL_TYPE_PC,MSG_EXPED_SETLABEL_MODE_RESET,-1,ch->m_index);
				ch->m_Exped->SendToAllPC(rmsg);
			}

			if(nGoZone==1)
			{
				CZone* pZone = gserver->FindZone(nZoneNo);
				if (pZone == NULL)
					goto ERROR_RETURN;

				if (pZone->m_zonePos == NULL)
					goto ERROR_RETURN;

				if (nExtra < 0 || nExtra >= pZone->m_countZonePos)
					goto ERROR_RETURN;

				//���� ����� ��, ���ȣ �ʱ�ȭ
				CArea* area = NULL;
				
				ch->m_nJoinInzone_ZoneNo = -1;
				ch->m_nJoinInzone_RoomNo = -1;
				ch->m_nJoinInzone_AreaNo = -1;

				GoZone(ch, nZoneNo,
					   pZone->m_zonePos[nExtra][0],													// ylayer
					   GetRandom(pZone->m_zonePos[nExtra][1], pZone->m_zonePos[nExtra][3]) / 2.0f,		// x
					   GetRandom(pZone->m_zonePos[nExtra][2], pZone->m_zonePos[nExtra][4]) / 2.0f);	// z
			}
		}

		// ��Ƽ ���̵�� �̰����� �´�.
		else if (ch->IsParty() && nGoZone==1)
		{
			CZone* pZone = gserver->FindZone(nZoneNo);
			if (pZone == NULL)
				goto ERROR_RETURN;

			if (pZone->m_zonePos == NULL)
				goto ERROR_RETURN;

			if (nExtra < 0 || nExtra >= pZone->m_countZonePos)
				goto ERROR_RETURN;

			//���� ����� ��, ���ȣ �ʱ�ȭ
			ch->m_nJoinInzone_ZoneNo = -1;
			ch->m_nJoinInzone_RoomNo = -1;
			ch->m_nJoinInzone_AreaNo = -1;

			GoZone(ch, nZoneNo,
				   pZone->m_zonePos[nExtra][0],													// ylayer
				   GetRandom(pZone->m_zonePos[nExtra][1], pZone->m_zonePos[nExtra][3]) / 2.0f,		// x
				   GetRandom(pZone->m_zonePos[nExtra][2], pZone->m_zonePos[nExtra][4]) / 2.0f);	// z
		}

		// � �͵� �ƴϸ� ������ ������ ����
		else
		{
			CZone* pZone = gserver->FindZone(nZoneNo);
			if (pZone == NULL)
				goto ERROR_RETURN;

			if (pZone->m_zonePos == NULL)
				goto ERROR_RETURN;

			// 0�� ������
			nExtra = 0;

			//���� ����� ��, ���ȣ �ʱ�ȭ
			ch->m_nJoinInzone_ZoneNo = -1;
			ch->m_nJoinInzone_RoomNo = -1;
			ch->m_nJoinInzone_AreaNo = -1;

			GoZone(ch, nZoneNo,
				   pZone->m_zonePos[nExtra][0],													// ylayer
				   GetRandom(pZone->m_zonePos[nExtra][1], pZone->m_zonePos[nExtra][3]) / 2.0f,		// x
				   GetRandom(pZone->m_zonePos[nExtra][2], pZone->m_zonePos[nExtra][4]) / 2.0f);	// z
		}
	}

ERROR_RETURN:
	ch->ResetPlayerState(PLAYER_STATE_WARP);
	return;
}

