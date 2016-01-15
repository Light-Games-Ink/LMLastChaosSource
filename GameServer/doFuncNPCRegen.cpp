#include "stdhdrs.h"

#include "Log.h"
#include "Character.h"
#include "Server.h"
#include "CmdMsg.h"
#include "doFunc.h"

////////////////////////////
// �̱۴��� NPC Regen ��û
void do_NPCRegen(CPC* ch, CNetMsg::SP& msg)
{
	int index, dbIndex, yLayer;
	int	entityIndex;	// Ŭ���̾�Ʈ ���� �ε���
	float x, z, h, r;

	msg->MoveFirst();

	RefMsg(msg) >> index
				>> dbIndex
				>> x >> z >> h >> r >> yLayer
				>> entityIndex;

	// npc regen ��û�� �ƴϸ�
	if (index != -1)
		return;

	// �̱۴����� �ƴϸ�
	if (!ch->m_pZone->IsPersonalDungeon())
		return;

	// ���� ���̺� ���� ���� Ŭ���̾�Ʈ���� ������ Regen Msg�� ���� �´ٸ�
	// �̱۴������� ��ŸƮ ������ ƨ���
	bool gohome = false;

	if( !ch->m_pArea->m_npcRegenList.SetRegened(ch->m_pArea, dbIndex, ch->m_pZone->m_index, x, yLayer, z) )
		gohome = true;

	CNPC* npc;
	// npc ����
	npc = gserver->m_npcProtoList.Create(dbIndex, NULL);

	if (!npc)	return;

	if( gohome)
	{
		// ����� ������
		int nearZone;
		int nearZonePos;
		CZone* pZone = gserver->FindNearestZone(ch->m_pZone->m_index, GET_X(ch), GET_Z(ch), &nearZone, &nearZonePos);
		if (pZone == NULL)
			return;

		GAMELOG << init("PD_BUG_HACK_REGEN", ch) << "ZONE" << ch->m_pZone->m_index << delim << "NPC" << delim << dbIndex << end;

		GoZone(ch, nearZone,
			   pZone->m_zonePos[nearZonePos][0],															// ylayer
			   GetRandom(pZone->m_zonePos[nearZonePos][1], pZone->m_zonePos[nearZonePos][3]) / 2.0f,		// x
			   GetRandom(pZone->m_zonePos[nearZonePos][2], pZone->m_zonePos[nearZonePos][4]) / 2.0f);		// z

		if(npc)
			delete npc;

		return;
	}
	// TODO : ���� �۽��� ������ �� ���̵� ������ ���� ������ ���� ���� ������ �Ķ���͸� �ְ� ������Ų��

	npc->m_regenX = GET_X(npc) = x;
	npc->m_regenZ = GET_Z(npc) = z;
	npc->m_regenY = GET_YLAYER(npc) = yLayer;
	GET_R(npc) = r;

	// TODO : ���̸ʿ� ���� height ����
	npc->m_pos.m_h = h;

	int cx, cz;
	ch->m_pArea->AddNPC(npc);
	ch->m_pArea->PointToCellNum(GET_X(npc), GET_Z(npc), &cx, &cz);
	ch->m_pArea->CharToCell(npc, GET_YLAYER(npc), cx, cz);

	{
		CNetMsg::SP rmsg(new CNetMsg);
		NPCRegenMsg(rmsg, npc, entityIndex);
		ch->m_pArea->SendToCell(rmsg, ch, true);
	}

#ifdef LC_GAMIGO
	if(ch->m_pZone->m_index == ZONE_SINGLE_DUNGEON1)
	{
		GAMELOG << init("NPC REGEN PD1", ch)
				<< index << delim
				<< dbIndex << delim
				<< x << delim
				<< z << delim
				<< h << delim
				<< r << delim
				<< yLayer << delim
				<< entityIndex << end;
	}
#endif // LC_GAMIGO
}
//