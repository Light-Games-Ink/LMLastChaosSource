#include "stdhdrs.h"

#include "Log.h"
#include "Character.h"
#include "Server.h"
#include "DratanCastle.h"
#include "../ShareLib/packetType/ptype_old_do_action.h"

void do_Action(CPC* ch, CNetMsg::SP& msg)
{
	CDratanCastle * pCastle = CDratanCastle::CreateInstance();
	pCastle->CheckRespond(ch);

	RequestClient::action* packet = reinterpret_cast<RequestClient::action*>(msg->m_buf);

	//�����ȿ��� (��)����̵� ��� �Ұ�
	if(ch->m_nJoinInzone_ZoneNo >=0 && ch->m_nJoinInzone_RoomNo >= 0)
	{
		if(packet->index == 38)
			return;
	}

	switch (packet->type)
	{
	case ACTION_GENERAL:
		{
			switch (packet->index)
			{
			case AGT_SITDOWN:				// �ɱ� ����
			case AGT_PET_SITDOWN:			// �� �ɱ� ����
				{
					// ���� ���¸� ����
					if (DEAD(ch) || ch->m_personalshop != NULL || ch->IsSetPlayerState(PLAYER_STATE_CHANGE))
						return ;

					// �� �ɱ� ���� ���� ���� Ÿ�� �־�� ��
					if (packet->index == AGT_PET_SITDOWN)
					{
						if (!ch->GetPet() || !ch->GetPet()->IsMount())
							return ;
					}

					if (!ch->IsSetPlayerState(PLAYER_STATE_SITDOWN))
					{
						if (ch->m_currentSkill)
						{
							ch->m_currentSkill->Cancel(ch);
							ch->m_currentSkill = NULL;
						}
						ch->ResetPlayerState(PLAYER_STATE_MOVING);
					}
					ch->TogglePlayerState(PLAYER_STATE_SITDOWN);
					ch->CalcStatus(true);
				}
				break;

			case AGT_PKMODE:			// ��ȭ/��� ���
				{
#ifdef BLOCK_PVP
					return;
#else
					if ( gserver->m_bNonPK )
						return;

					if (DEAD(ch) || ch->m_personalshop != NULL)
						return ;

					if (ch->m_pZone->m_index == ZONE_GUILDROOM)
						return;

					if (ch->m_assist.FindBySkillIndex(PVP_PROTECT_SKILL_INDEX) != 0)
					{
						CNetMsg::SP rmsg(new CNetMsg);
						SysMsg(rmsg, MSG_SYS_DO_NOT_PLAY_PVP);
						SEND_Q(rmsg, ch->m_desc);
						return;
					}

					if (ch->m_level <= PKMODE_LIMIT_LEVEL)
					{
						bool bSkipLevel = false;
						// ���� ���� �� ���� ���� �������� �������� PK ����
						if (ch->m_pZone->m_index == CWarCastle::GetCurSubServerCastleZoneIndex() && ch->GetMapAttr() & MATT_WAR)
						{
							CWarCastle* castle = CWarCastle::GetCastleObject(ch->m_pZone->m_index);
							if (castle && castle->GetState() != WCSF_NORMAL)
							{
								bSkipLevel = true;
							}
						}

						if (!bSkipLevel)
						{
							CNetMsg::SP rmsg(new CNetMsg);
							SysMsg(rmsg, MSG_SYS_PKMODE_LIMITLEVEL);
							SEND_Q(rmsg, ch->m_desc);
							return;
						}
					}

					if (ch->IsSetPlayerState(PLAYER_STATE_PKMODEDELAY) || ch->IsSetPlayerState(PLAYER_STATE_RAMODE) || ch->IsSetPlayerState(PLAYER_STATE_CHANGE))
						return ;

					if (ch->IsInPeaceZone(true))
						return ;

					if (ch->GetMapAttr() & MATT_FREEPKZONE)
						return ;

					// PVP �Ʒ��� ���̸��� ���������� ���� ����� ��� pk��尡 �ȵǰ� ����..   yhj
					if ( !(ch->GetMapAttr() & MATT_FREEPKZONE)
							&& !(ch->GetMapAttr() & MATT_PEACE)
							&& ch->m_pZone->m_index == ZONE_PK_TOURNAMENT )
					{
						return ;
					}

					// ��Ʈ���̾Ƴ� ���� ��ܿ��� pk��尡 �ȵǰ� ����..   yhj
					// ���� ��ǥ�� ��Ʈ���̾Ƴ� ������ ������ǥ
					if ( (ch->GetMapAttr() & MATT_STAIR_UP || ch->GetMapAttr() & MATT_STAIR_DOWN )
							&& ch->m_pZone->m_index == ZONE_STREIANA
							&& GET_X(ch) >= 940.0 && GET_X(ch) <= 1090.0
							&& GET_Z(ch) >= 515.0 && GET_Z(ch) <= 695.0 )
					{
						return ;
					}

					if (ch->m_pZone->IsOXQuizRoom())
					{
						CNetMsg::SP rmsg(new CNetMsg);
						SysMsg(rmsg, MSG_SYS_DO_NOT_CHANGE_PK_MODE);
						SEND_Q(rmsg, ch->m_desc);
						return ;
					}

					if (ch->m_pZone->isRVRZone())
					{
						CNetMsg::SP rmsg(new CNetMsg);
						SysMsg(rmsg, MSG_SYS_DO_NOT_CHANGE_PK_MODE);
						SEND_Q(rmsg, ch->m_desc);
						return ;
					}

					if (ch->IsSetPlayerState(PLAYER_STATE_PKMODE))
					{
						ch->SetPlayerState(PLAYER_STATE_PKMODEDELAY);
						ch->m_pkmodedelay = PULSE_PKMODE_DELAY;
					}
					else
					{
						ch->TogglePlayerState(PLAYER_STATE_PKMODE);
						ch->CancelInvisible();
					}
#endif
				}
				break;

			case AGT_THROW_WATER:
				{
					if( !gserver->isActiveEvent(A_EVENT_SONGKRAN) )
						return;

					CCharacter* tch = ch->m_pArea->FindCharInCell( ch, packet->targetIndex, MSG_CHAR_PC, true );
					if( tch == NULL )
					{
						return;
					}

					CSkill* skill = new CSkill( gserver->m_skillProtoList.Find( 436 ), 1 );
					bool bApply;
					ApplySkill( ch, tch, skill, -1, bApply );
					delete skill;
				}

				break;
			} // end switch
		}
		break;

	case ACTION_SOCIAL:
	case ACTION_PARTY:
	case ACTION_GUILD:
	case ACTION_TITLE:
		break;

	default:
		{
			LOG_ERROR("HACKING : invalid action type[%d]. charIndex[%d]", packet->type, ch->m_index);
			ch->m_desc->Close("invalid action type");
			return;
		}
	} // end switch

	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makeAction(rmsg, ch, packet->type, packet->index);
		ch->m_pArea->SendToCell(rmsg, ch, true);
	}
}
//