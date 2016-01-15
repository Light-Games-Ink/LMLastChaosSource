#include "stdhdrs.h"

#include "Log.h"
#include "Character.h"
#include "Server.h"
#include "WarCastle.h"
#include "DratanCastle.h"
#include "gameserver_config.h"
#include "../ShareLib/packetType/ptype_old_mempos.h"

/////////////////
// ��� ��� ����

void do_MemPos(CPC* ch, CNetMsg::SP& msg)
{
	CDratanCastle * pCastle = CDratanCastle::CreateInstance();
	pCastle->CheckRespond(ch);

	if (DEAD(ch))
		return ;

	pTypeBase* pBase = reinterpret_cast<pTypeBase*>(msg->m_buf);
	switch (pBase->subType)
	{
	case MSG_MEMPOS_WRITE:
		{
			RequestClient::memposWrite* packet = reinterpret_cast<RequestClient::memposWrite*>(msg->m_buf);
			packet->comment[MEMPOS_COMMENT_LENGTH] = '\0';

			if(findPercentChar(packet->comment) != NULL)
				return;

			// ������ �˻�
			if (ch->m_inventory.FindByDBIndex(47, 0, 0) == NULL)
				return ;

			// ��� �Ұ��� ���̸� ����
			if (!ch->m_pZone || !ch->m_pZone->m_bCanMemPos)
			{
				CNetMsg::SP rmsg(new CNetMsg);
				SysMsg(rmsg, MSG_SYS_MEMPOS_CANT_WRITE);
				SEND_Q(rmsg, ch->m_desc);
				return ;
			}

			// BLOCK���� �Ұ�
			if (ch->m_pArea->GetAttr(GET_YLAYER(ch), GET_X(ch), GET_Z(ch)) & MATT_UNWALKABLE)
			{
				CNetMsg::SP rmsg(new CNetMsg);
				SysMsg(rmsg, MSG_SYS_MEMPOS_CANT_WRITE);
				SEND_Q(rmsg, ch->m_desc);
				return ;
			}

			int valid_slot = (ch->m_memposTime < gserver->getNowSecond()) ? MAX_MEMPOS_NORMAL : MAX_MEMPOS;
			if (packet->slot < 0 || packet->slot >= valid_slot)
			{
				LOG_ERROR("HACKING : invalid slot[%d]. charIndex[%d]", packet->slot, ch->m_index);
				ch->m_desc->Close("invalid slot");
				return;
			}

			if (ch->m_mempos.Write(packet->slot, ch->m_pZone->m_index, GET_X(ch), GET_Z(ch), GET_YLAYER(ch), packet->comment))
			{
				CNetMsg::SP rmsg(new CNetMsg);
				ResponseClient::makeMemposWrite(rmsg, ch, packet->slot);
				SEND_Q(rmsg, ch->m_desc);
			}
		}
		break;

	case MSG_MEMPOS_MOVE:
		{
			RequestClient::memposMove* packet = reinterpret_cast<RequestClient::memposMove*>(msg->m_buf);
			int slot = packet->slot;

			int canWarp = ch->CanWarp();
			if (canWarp != 0)
			{
				CNetMsg::SP rmsg(new CNetMsg);
				SysMsg(rmsg, (MSG_SYS_TYPE)canWarp);
				SEND_Q(rmsg, ch->m_desc);
				return ;
			}

			int valid_slot = (ch->m_memposTime < gserver->getNowSecond()) ? MAX_MEMPOS_NORMAL : MAX_MEMPOS;
			if (slot < 0 || slot >= valid_slot)
			{
				LOG_ERROR("HACKING : invalid slot[%d]. charIndex[%d]", slot, ch->m_index);
				ch->m_desc->Close("invalid slot");
				return;
			}

			if (ch->m_mempos.m_data[slot] == NULL)
			{
				LOG_ERROR("HACKING : slot[%d] index is empty. charIndex[%d]", slot, ch->m_index);
				ch->m_desc->Close("slot index is empty");
				return ;
			}

			// �� �˻� : ���� ��������
			if (!ch->m_pZone || ch->m_pZone->m_index != ch->m_mempos.m_data[slot]->m_zone)
			{
				CNetMsg::SP rmsg(new CNetMsg);
				SysMsg(rmsg, MSG_SYS_MEMPOS_OTHERZONE);
				SEND_Q(rmsg, ch->m_desc);
				return ;
			}

			// BLOCK���� �Ұ�
			if (ch->m_pArea->GetAttr(GET_YLAYER(ch), GET_X(ch), GET_Z(ch)) & MATT_UNWALKABLE)
				return ;

			CWarCastle* castle = CWarCastle::GetCastleObject(ch->m_pZone->m_index);
			if (castle)
			{
				// �������̰ų� ���� 1�� ������ ������������ �̵� �Ұ�
				if (castle->GetState() != WCSF_NORMAL || castle->IsWarCastleReady())
				{
					if (ch->m_pArea->GetAttr(ch->m_mempos.m_data[slot]->m_ylayer, ch->m_mempos.m_data[slot]->m_x, ch->m_mempos.m_data[slot]->m_z ) & MATT_WAR
							|| ch->m_pZone->IsWarZone((int)ch->m_mempos.m_data[slot]->m_x, (int)ch->m_mempos.m_data[slot]->m_z) == true)
					{
						CNetMsg::SP rmsg(new CNetMsg);
						SysMsg(rmsg, MSG_SYS_MEMPOS_CASTLE);
						SEND_Q(rmsg, ch->m_desc);
						return;
					}

					//�����ð��� �Ÿ� ��Ŭ�� ���α�
					GAMELOG << init(" MEMORY SCROLL IN WARTIME " , ch )
							<< "CUR : " << ch->m_pos.m_yLayer << "," << ch->m_pos.m_x << "," << ch->m_pos.m_z << delim
							<< "MOV : " << ch->m_mempos.m_data[slot]->m_ylayer << "," << ch->m_mempos.m_data[slot]->m_x << ","
							<< ch->m_mempos.m_data[slot]->m_z << end;
				}
			}

			// ������ �˻�
			CItem* item = ch->m_inventory.FindByDBIndex(47, 0, 0);
			if (item == NULL)
			{
				LOG_ERROR("HACKING : not found item. charIndex[%d]", ch->m_index);
				ch->m_desc->Close("not found item");
				return;
			}

			// ī��Ʈ ���̱�
			ch->m_inventory.decreaseItemCount(item, 1);

			// 10���� �̵��ϰ� �ϱ�
			ch->m_reqWarpType = IONCE_WARP_MEMPOS;
			ch->m_reqWarpData = slot;
			ch->m_reqWarpTime = PULSE_WARPDELAY;
			ch->SetPlayerState(PLAYER_STATE_WARP);

			{
				// ��������� �˸���
				CNetMsg::SP rmsg(new CNetMsg);
				WarpStartMsg(rmsg, ch);
				ch->m_pArea->SendToCell(rmsg, ch, true);
			}
		}
		break;
	}
}
