#include "stdhdrs.h"

#include "Server.h"
#include "Log.h"
#include "CmdMsg.h"
#include "WarCastle.h"
#include "DratanCastle.h"

void CheckWarCastle()
{
	meracCastle.CheckWarCastle(false);
	CDratanCastle * pCastle = CDratanCastle::CreateInstance();
	pCastle->CheckWarCastle();
}

bool LoadCastleData()
{
	// �޶�ũ
	if (!meracCastle.LoadCastleData())
		return false;

#ifdef WARCASTLE_SUBNUMBER_MEMBER_VALUE
	// yhj... ���þƿ��� �ö� �� �޶�ũ �������� �������ٴ� �� ������ ��ġ
	// �۷ι� ������ �ɹ� ������ ����
	else
		meracCastle.m_subNumber = gserver->m_subno;
#endif // WARCASTLE_SUBNUMBER_MEMBER_VALUE

	// ���ź(���� ������ ���ź ������ �ִ�)
	CDratanCastle * pCastle = CDratanCastle::CreateInstance();
	if (pCastle->LoadCastleData() == false)
	{
		return false;
	}

	return true;
}

bool IsValidGuildCommandWithWarCastle(CPC* ch, unsigned char subtype)
{
	// ���� ���� ������ ��� ���� Ŀ�ǵ� �� �Ϻθ� �� �� ����
	int warZoneCount = 2;
	int warZoneIndex[] = { ZONE_MERAC, ZONE_DRATAN };

	int i;
	for (i = 0; i < warZoneCount; i++)
	{
		CWarCastle* castle = CWarCastle::GetCastleObject(warZoneIndex[i]);
		if (castle)
		{
			if (castle->GetState() != WCSF_NORMAL && ch->GetJoinFlag(warZoneIndex[i]) != WCJF_NONE)
			{
				switch (subtype)
				{
				case MSG_GUILD_CREATE:
				case MSG_GUILD_LEVELUP:
				case MSG_GUILD_BREAKUP:
				case MSG_GUILD_REGIST_REQ:
				case MSG_GUILD_REGIST_ALLOW:
				case MSG_GUILD_REGIST_CANCEL:
				case MSG_GUILD_OUT_REQ:
				case MSG_GUILD_KICK:
				case MSG_GUILD_CHANGE_BOSS:
				case MSG_GUILD_APPOINT_OFFICER:
				case MSG_GUILD_FIRE_OFFICER:
				case MSG_GUILD_BATTLE_REQ_REQ:
				case MSG_GUILD_BATTLE_REQ_REJECT:
				case MSG_GUILD_BATTLE_REQ_ACCEPT:
				case MSG_GUILD_BATTLE_STOP_REQ:
				case MSG_GUILD_BATTLE_STOP_REJECT:
				case MSG_GUILD_BATTLE_STOP_ACCEPT:
				case MSG_GUILD_MASTER_KICK_REQ:
				case MSG_GUILD_MASTER_KICK_CANCEL_REQ:
					return false;
				}
			}
		}
	}

	return true;
}

