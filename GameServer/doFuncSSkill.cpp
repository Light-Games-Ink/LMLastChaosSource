#include "stdhdrs.h"

#include "Log.h"
#include "Character.h"
#include "DratanCastle.h"
#include "../ShareLib/packetType/ptype_old_do_sskill.h"

void do_SSkill(CPC* ch, CNetMsg::SP& msg)
{
	CDratanCastle * pCastle = CDratanCastle::CreateInstance();
	pCastle->CheckRespond(ch);

	if (DEAD(ch))
		return;

	RequestClient::sskill* packet = reinterpret_cast<RequestClient::sskill*>(msg->m_buf);

	if (packet->subType != MSG_SSKILL_LEARN)
	{
		LOG_ERROR("HACKING : invalid subtype[%d]. charIndex[%d]", packet->subType, ch->m_index);
		ch->m_desc->Close("invalid subtype");
		return;
	}

	CNPC* pNpc = (CNPC*)ch->m_pArea->FindCharInCell(ch, packet->npcIndex, MSG_CHAR_NPC, true);
	if (pNpc == NULL)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makeSSkillLearnError(rmsg, MSG_SSKILL_LEARN_ERROR_NOT_EXIST_NPC);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	// index�� 0���ϸ� ����
	if (packet->sskillIndex <= 0)
	{
		LOG_ERROR("HACKING : invalid sskill index[%d]. charIndex[%d]", packet->sskillIndex, ch->m_index);
		ch->m_desc->Close("invalid sskill index");
		return;
	}

	CSSkillProto* proto = gserver->m_sSkillProtoList.FindProto(packet->sskillIndex);
	if (proto == NULL)
	{
		LOG_ERROR("HAKCING : not found sskill[%d]. charIndex[%d]", packet->sskillIndex, ch->m_index);
		ch->m_desc->Close("not found sskill");
		return;
	}

	// �켱���� Validation �˻�
	CSSkillNode* node = ch->m_sSkillList.m_head;
	while (node)
	{
		// Ÿ���� ���� ��ų�� �ְ�
		if (node->m_sskill->m_proto->m_type == proto->m_type)
		{
			// �ִ� ��ų���� ���� �켱������ ��ų�� ���� �ϸ� Error
			if (node->m_sskill->m_proto->m_preference != -1 && node->m_sskill->m_proto->m_preference > proto->m_preference)
			{
				CNetMsg::SP rmsg(new CNetMsg);
				ResponseClient::makeSSkillLearnError(rmsg, MSG_SSKILL_LEARN_ERROR_SYSTEM);
				SEND_Q(rmsg, ch->m_desc);
				return;
			}
		}
		node = node->m_next;
	}

	CSSkill* sskill = ch->m_sSkillList.Find(packet->sskillIndex);
	int level = 1;

	if (sskill)
	{
		if (sskill->m_level >= sskill->m_proto->m_maxLevel)
			return;

		level = sskill->m_level + 1;
	}

	// �ʿ� Level �˻�
	if (proto->m_needLevel[level - 1] > ch->m_level)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makeSSkillLearnError(rmsg, MSG_SSKILL_LEARN_ERROR_LEVEL);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	// �ʿ� SP �˻�
	if (proto->m_needSP[level - 1] * 10000 > ch->m_skillPoint)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makeSSkillLearnError(rmsg, MSG_SSKILL_LEARN_ERROR_SP);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

	// �ʿ� SSkill �˻�
	if (proto->m_needSSkill != -1 && !sskill)
	{
		CSSkill* check = ch->m_sSkillList.Find(proto->m_needSSkill);

		// �ʿ� sskill�� �ƿ� ����
		if (!check)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			ResponseClient::makeSSkillLearnError(rmsg, MSG_SSKILL_LEARN_ERROR_SSKILL);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}
		//  �ʿ� sskill level�� �ȵȴ�
		else if (proto->m_needSSkillLevel > check->m_level)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			ResponseClient::makeSSkillLearnError(rmsg, MSG_SSKILL_LEARN_ERROR_SSKILL_LEVEL);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}
	}

	// ������ ��ų�� �־��ٸ� Level Up
	if (sskill)
	{
		// �ְ� �ʰ�
		if (sskill->m_level >= sskill->m_proto->m_maxLevel)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			ResponseClient::makeSSkillLearnError(rmsg, MSG_SSKILL_LEARN_ERROR_SYSTEM);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}

		sskill->m_level++;
	}
	else
	{
		sskill = gserver->m_sSkillProtoList.Create(packet->sskillIndex, 1);
		if (!sskill)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			ResponseClient::makeSSkillLearnError(rmsg, MSG_SSKILL_LEARN_ERROR_SYSTEM);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}

		if (!ch->m_sSkillList.Add(sskill))
		{
			CNetMsg::SP rmsg(new CNetMsg);
			ResponseClient::makeSSkillLearnError(rmsg, MSG_SSKILL_LEARN_ERROR_SYSTEM);
			SEND_Q(rmsg, ch->m_desc);
			return;
		}
	}

	// SP �Ҹ�
	ch->m_skillPoint -= proto->m_needSP[level - 1] * 10000;

	{
		// Learn Msg ����
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makeSSkillLearn(rmsg, sskill);
		SEND_Q(rmsg, ch->m_desc);
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		StatusMsg(rmsg, ch);
		SEND_Q(rmsg, ch->m_desc);
	}
}

