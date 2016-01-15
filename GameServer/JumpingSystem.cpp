#include "stdhdrs.h"

#include "../ShareLib/bnf.h"
#include "Server.h"
#include "JumpingSystem.h"
#include "../ShareLib/packetType/ptype_old_do_item.h"
#include "../ShareLib/packetType/ptype_old_do_changejob.h"
#include "../ShareLib/packetType/ptype_old_do_skill.h"

#include "Log.h"
#include "CmdMsg.h"

CJumpingSystem::CJumpingSystem(CPC* _ch, int _itemIndex)
	: m_pc(_ch), m_itemIndex(_itemIndex), m_jumpingLevel(0), m_rewardIndex(-1)
{
	const CItemProto* _itemProto = gserver->m_itemProtoList.FindIndex(_itemIndex);
	if ( _itemProto )
	{
		m_jumpingLevel	= _itemProto->getItemNum0();
		m_rewardIndex	= _itemProto->getItemNum1();
	}
}

CJumpingSystem::~CJumpingSystem(void)
{
}

bool CJumpingSystem::check(void)
{
	// �⺻ �˻�
	if ( m_pc == NULL )
		return false;

	if ( m_pc->m_teachType == MSG_TEACH_STUDENT_TYPE )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::ItemNotUseMsg(rmsg, MSG_ITEM_USE_ERROR_GUARDIAN);
		SEND_Q(rmsg, m_pc->m_desc);

		return false;
	}

	// ���� ���� >= ������ �����̸� ��� �Ұ���
	if ( m_pc->m_level >= m_jumpingLevel )
		return false;

	// ���� ���� ���� ���� �� ���� ����
	if ( m_rewardIndex > 0 )
	{
		return gserver->m_rewardMgr.setRewardData(m_pc, m_rewardIndex);
	}

	return true;
}

bool CJumpingSystem::jump(void)
{
	if ( m_pc == NULL )
		return false;

	if ( m_jumpingLevel <= 0 )
		return false;

	// ������
	m_pc->m_level = m_jumpingLevel;
	m_pc->LevelUp(true);

	// ĳ���� �ʱ�ȭ
	if ( !initCharacter() )
	{
		return false;
	}

	// ���� ����
	if ( !giveReward() )
	{
		GAMELOG << init("JUMPING SYSTEM")
				<< "GIVE REWARD FAIL" << end;
		return false;
	}

	return true;
}

bool CJumpingSystem::initCharacter(void)
{
	// ���� �ʱ�ȭ
	if ( !initJob() )
	{
		GAMELOG << init("JUMPING SYSTEM")
				<< "INIT JOB FAIL" << end;
		return false;
	}

	// ��ų �ʱ�ȭ
	if ( !initSkill() )
	{
		GAMELOG << init("JUMPING SYSTEM")
				<< "INIT SKILL FAIL" << end;
		return false;
	}

	// ���� �ʱ�ȭ
	if ( !initStat() )
	{
		GAMELOG << init("JUMPING SYSTEM")
				<< "INIT STAT FAIL" << end;
		return false;
	}

	// HP, MP ä���
	m_pc->m_hp = m_pc->m_maxHP;
	m_pc->m_mp = m_pc->m_maxMP;

	return true;
}

bool CJumpingSystem::giveReward(void)
{
	if ( m_rewardIndex > 0 )
	{
		return gserver->m_rewardMgr.doReward(m_pc, m_rewardIndex);
	}

	return true;
}

bool CJumpingSystem::initJob(void)
{
	if ( m_pc->m_job != JOB_NIGHTSHADOW )
		m_pc->m_job2 = 0;

	{
		CNetMsg::SP rmsg(new CNetMsg);
		ResponseClient::makeChangeJob(rmsg, m_pc->m_index, m_pc->m_job, m_pc->m_job2);
		m_pc->m_pArea->SendToCell(rmsg, m_pc, true);
	}

	if (m_pc->IsParty())
	{
		CNetMsg::SP rmsg(new CNetMsg);
		HelperPartyMemberChangeJobMsg(rmsg, m_pc->m_party->GetBossIndex(), m_pc->m_index, m_pc->m_job, m_pc->m_job2);
		SEND_Q(rmsg, gserver->m_helper);
	}

	return true;
}

bool CJumpingSystem::initSkill(void)
{
	// Active Skill �ʱ�ȭ
	m_pc->InitSkill(&m_pc->m_activeSkillList);

	// Passive Skill �ʱ�ȭ
	m_pc->InitSkill(&m_pc->m_passiveSkillList, true);

	CNetMsg::SP rmsg(new CNetMsg);
	ResponseClient::makeSkillListMsg(rmsg, m_pc);
	SEND_Q(rmsg, m_pc->m_desc);

	return true;
}

bool CJumpingSystem::initStat(void)
{
	// ���� ����
	// ���� ����Ʈ �ʱ�ȭ
	if ( m_pc->m_job == JOB_NIGHTSHADOW )
	{
		m_pc->m_statpt_remain = (m_pc->m_level - JOB_2ND_LEVEL) * 3 + (m_pc->m_level - JOB_2ND_LEVEL) / 2 + (JOB_2ND_LEVEL - 1);
	}
	else
	{
		m_pc->m_statpt_remain = m_pc->m_level - 1;
	}

	m_pc->m_statpt_str = m_pc->m_statpt_dex = m_pc->m_statpt_int = m_pc->m_statpt_con = 0;

	m_pc->InitStat();
	m_pc->CalcStatus(true);

	CNetMsg::SP rmsg(new CNetMsg);
	StatPointRemainMsg(rmsg, m_pc);
	SEND_Q(rmsg, m_pc->m_desc);

	return true;
}

