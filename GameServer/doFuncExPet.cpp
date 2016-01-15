#include "stdhdrs.h"

#include "Server.h"
#include "Log.h"
#include "Character.h"
#include "CmdMsg.h"
#include "doFunc.h"
#include "Battle.h"


void do_ExPetCall(CPC* ch, CNetMsg::SP& msg)
{
	int index;
	RefMsg(msg) >> index;
	CPet* pet = ch->GetPet(index);
	if (!pet)
		return ;
	if (!IS_IN_CELL(pet))
		return ;
	pet->Disappear();

	GET_X(pet) = GET_X(ch);
	GET_Z(pet) = GET_Z(ch);
	GET_R(pet) = GET_H(ch);
	GET_YLAYER(pet) = GET_YLAYER(ch);

	pet->Appear(true);
}

void do_ExPetLearn(CPC* ch, CNetMsg::SP& msg)
{
	int skillindex = -1;
	RefMsg(msg) >> skillindex;

	if (ch->m_wearInventory.wearItemInfo[WEARING_PET] == NULL)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetLearnMsg(rmsg, 0, 0, MSG_EX_PET_LEARN_ERROR_WEAR);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	CPet* pet = ch->GetPet();
	if (pet == NULL)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetLearnMsg(rmsg, 0, 0, MSG_EX_PET_LEARN_ERROR_WEAR);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	const CSkillProto* skillProto = gserver->m_skillProtoList.Find(skillindex);
	if (skillProto == NULL)
		return ;
	if (skillProto->m_type != ST_PET_COMMAND && skillProto->m_type != ST_PET_SKILL_PASSIVE && skillProto->m_type != ST_PET_SKILL_ACTIVE)
		return ;

	// ���� �ش� ��ų�� ������ �ִ��� �˻�
	CSkill* curSkill = pet->FindSkill(skillindex);
	int nextlevel = (curSkill == NULL) ? 1 : curSkill->m_level + 1;

	const CSkillLevelProto* skillLevelProto = skillProto->Level(nextlevel);
	if (skillLevelProto == NULL)
		return ;
	if (skillProto->m_maxLevel < nextlevel)
		return ;

	// ���� �˻� : ����
	if (!skillProto->IsSameJob(pet))
		return ;

	// ���� �˻� : ����
	if (skillLevelProto->m_learnLevel > pet->m_level)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetLearnMsg(rmsg, 0, 0, MSG_EX_PET_LEARN_ERROR_LEVEL);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	// ���� �˻� : ����Ʈ : ��� ����Ʈ�� SP�� �޸� �ܱͰ� 1�� ����
	if (skillLevelProto->m_learnSP > pet->GetAbilityPoint())
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetLearnMsg(rmsg, 0, 0, MSG_EX_PET_LEARN_ERROR_POINT);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	// ���� �˻� : ������
	item_search_t vec[MAX_SKILL_LEARN_ITEM];
	for (int i = 0; i < MAX_SKILL_LEARN_ITEM; i++)
	{
		if (skillLevelProto->m_learnItemIndex[i] < 0)
			continue ;

		int sc = ch->m_inventory.searchItemByCondition(skillLevelProto->m_learnItemIndex[i], 0, 0, vec[i]);
		if (sc == 0 || sc < skillLevelProto->m_learnItemCount[i])
		{
			CNetMsg::SP rmsg(new CNetMsg);
			ExPetLearnMsg(rmsg, 0, 0, MSG_EX_PET_LEARN_ERROR_ITEM);
			SEND_Q(rmsg, ch->m_desc);
			return ;
		}
	}

	// ��ų �߰�
	if (curSkill == NULL)
	{
		// �ű� �߰�
		curSkill = gserver->m_skillProtoList.Create(skillindex);
		if (!curSkill)
			return ;
		pet->AddSkill(curSkill);
	}
	else
	{
		// ���� ��ų ���� ��
		curSkill->m_level++;
	}

	{
		// ����Ʈ �Ҹ�
		pet->DecreaseAbilityPoint(skillLevelProto->m_learnSP);
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetStatusMsg(rmsg, pet);
		SEND_Q(rmsg, ch->m_desc);
	}

	// ������ �Ҹ�
	for (int i = 0; i < MAX_SKILL_LEARN_ITEM; i++)
	{
		ch->m_inventory.deleteItem(vec[i], skillLevelProto->m_learnItemCount[i]);
	}

	{
		// ��� �˸���
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetLearnMsg(rmsg, curSkill->m_proto->m_index, curSkill->m_level, MSG_EX_PET_LEARN_ERROR_OK);
		SEND_Q(rmsg, ch->m_desc);
	}

	// ��ų ȿ�� ����
	ch->CalcStatus(true);

	// ���� �α�
	// TODO : petlog
	GAMELOG << init("PET SKILL LEARN", ch)
			<< "PET" << delim
			<< pet->m_index << delim
			<< pet->GetPetTypeGrade() << delim
			<< "SKILL" << delim
			<< curSkill->m_proto->m_index << delim
			<< curSkill->m_level
			<< end;
}

void do_ExPetResetSkill(CPC* ch, CNetMsg::SP& msg)
{
#ifdef NPC_CHECK
	int npcIndex;
	CNPC *npc;
	RefMsg(msg) >> npcIndex;
	npc = ch->m_pArea->FindProtoNPCInCell(ch, npcIndex, false, 2);
	if(!npc)
	{
		GAMELOG << init("ExPetResetSkill FAIL : NOT EXIST NPC (Dangerous User)", ch) << end;
		return ;
	}
	if( !npc->IsFlag(NPC_EVENT) && !npc->IsFlag(NPC_QUEST) )
	{
		GAMELOG << init("ExPetResetSkill FAIL : NOT EXIST NPC (Dangerous User)", ch) << end;
		return ;
	}
#endif
	if (ch->m_wearInventory.wearItemInfo[WEARING_PET] == NULL)
		return ;

	if (ch->m_currentSkill)		// 080408 ����
		return ;

	CPet* pet = ch->GetPet();
	if (pet == NULL)
		return ;

	if (!pet->ResetSkill())
		return ;

	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetResetSkillMsg(rmsg);
		SEND_Q(rmsg, ch->m_desc);
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetStatusMsg(rmsg, pet);
		SEND_Q(rmsg, ch->m_desc);
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetSkillListMsg(rmsg, pet);
		SEND_Q(rmsg, ch->m_desc);
	}

	// ���� �α�
	// TODO : petlog
	GAMELOG << init("PET SKILL RESET", ch)
			<< "PET" << delim
			<< pet->m_index << delim
			<< pet->GetPetTypeGrade()
			<< end;
}

void do_ExPetChangeMount(CPC* ch, CNetMsg::SP& msg)
{
	if(ch)
	{
		if(ch->m_evocationIndex != EVOCATION_NONE)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_CANT_MAKE_PET_EVOCATION);
			SEND_Q(rmsg, ch->m_desc);
			return ;
		}
	}
//#endif // BUGFIX_CANT_EVOCATION

	// �������� ���� �ֳ�?
	CPet* pet = ch->GetPet();
	if (!pet )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetChangeMountMsg(rmsg, MSG_EX_PET_CHANGE_MOUNT_ERROR_WEAR);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	if( ch->m_job == JOB_NIGHTSHADOW )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetChangeMountMsg(rmsg, MSG_EX_PET_CHANGE_MOUNT_ERROR_JOB);
		SEND_Q(rmsg, ch->m_desc);
		return;
	}

#ifdef NPC_CHECK
	int npcIndex;
	CNPC *npc;
	RefMsg(msg) >> npcIndex;
	npc = ch->m_pArea->FindProtoNPCInCell(ch, npcIndex, false, 2);
	if(!npc)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetChangeMountMsg(rmsg, MSG_EX_PET_CHANGE_MOUNT_ERROR_NOT_EXIST_NPC);
		SEND_Q(rmsg, ch->m_desc);
		GAMELOG << init("EX PET CHANGE MOUNT FAIL : NOT EXIST NPC (Dangerous User)", ch) << end;
		return ;
	}
	if( !npc->IsFlag(NPC_EVENT) && !npc->IsFlag(NPC_QUEST) )
	{
		GAMELOG << init("EX PET CHANGE MOUNT FAIL : NOT EXIST NPC (Dangerous User)", ch) << end;
		return ;
	}
#endif

	// �������� ���� ������ PET_MOUNT_LEVEL�̻��ΰ�?
	if (pet->m_level < PET_MOUNT_LEVEL )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetChangeMountMsg(rmsg, MSG_EX_PET_CHANGE_MOUNT_ERROR_LEVEL);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	// ���� ���� �����ΰ�
	if (pet->GetPetGrade() != PET_GRADE_ADULT)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetChangeMountMsg(rmsg, MSG_EX_PET_CHANGE_MOUNT_ERROR_GRADE);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	if( pet->GetPetTurnToNpc() > 0 )
	{
		pet->SetPetTurnToNpc(0);
		CNetMsg::SP rmsg(new CNetMsg);
		HelperPetTurnToNPCMsg(rmsg, ch->m_index, pet->m_index, pet->GetPetTurnToNpc() , pet->GetPetTurnToNpcSize() );
		SEND_Q(rmsg, gserver->m_helper );
	}

	// �� Disappear ������
	pet->Disappear();

	// �� Ÿ�� ����
	pet->ChangeToMount();

	// �� ��ų �ʱ�ȭ : ����Ʈ ȯ��
	pet->ResetSkill();

	pet->Mount(true);

	{
		// ��� ����
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetChangeMountMsg(rmsg, MSG_EX_PET_CHANGE_MOUNT_ERROR_OK);
		SEND_Q(rmsg, ch->m_desc);
	}

	{
		// �� ���� ����
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetStatusMsg(rmsg, pet);
		SEND_Q(rmsg, ch->m_desc);
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetSkillListMsg(rmsg, pet);
		SEND_Q(rmsg, ch->m_desc);
	}

	{
		// PC ����Ʈ ����
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetMountMsg(rmsg, ch->m_index, pet->GetPetTypeGrade());
		RefMsg(rmsg) << pet->GetPetColor();
		RefMsg(rmsg) << pet->GetPetTurnToNpc();
		ch->m_pArea->SendToCell(rmsg, ch, true);
	}

	ch->m_bChangeStatus = true;

	// ���� �α�
	// TODO : petlog
	GAMELOG << init("PET CHANGE MOUNT", ch)
			<< "PET" << delim
			<< pet->m_index << delim
			<< pet->GetPetTypeGrade()
			<< end;
}

void do_ExPetCommand(CPC* ch, CNetMsg::SP& msg)
{
	int petindex = -1;
	int commandindex = -1;
	char targettype = -1;
	int targetindex = -1;

	RefMsg(msg) >> petindex
				>> commandindex
				>> targettype
				>> targetindex;

	CPet* pet = ch->GetPet();
	if (!pet || pet->m_index != petindex || !pet->IsSummon() || !pet->m_pArea)
		return ;

	CSkill* skill = pet->FindSkill(commandindex);
	if (!skill || skill->m_proto->m_type != ST_PET_COMMAND)
		return ;

	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetCommandMsg(rmsg, petindex, commandindex, targettype, targetindex);
		pet->m_pArea->SendToCell(rmsg, pet, true);
	}
}

void do_ExPetMixItem(CPC* ch, CNetMsg::SP& msg)
{
	typedef struct __tagMixData
	{
		int needitem[3];		// ��� 3��
		int prob;				// Ȯ��
		int itemindex;			// ���� ������
		int method;				// ���� ���
	} PET_MIX_DATA;

	static const int nSizeMixTable = 74;

	static const PET_MIX_DATA mixTable[nSizeMixTable] =
	{
		{{895, 896, 897}, 70, 887,  0},				// �Ѽհ�
		{{896, 897, 898}, 70, 907,  0},				// �̵���
		{{897, 898, 899}, 70, 908,  0},				// ���
		{{898, 899, 901}, 70, 909,  0},				// ����
		{{899, 901, 902}, 70, 910,  0},				// ���� �ϵ�
		{{901, 902, 903}, 70, 911,  0},				// Ȱ
		{{902, 903, 904}, 70, 912,  0},				// ��������
		{{903, 904, 905}, 70, 913,  0},				// ������
		{{904, 905, 900}, 70, 914,  0},				// �ܰ�
		{{905, 900, 906}, 70, 915,  0},				// ����
		{{900, 906, 895}, 70, 916,  0},				// ���̵�
		{{906, 895, 896}, 70, 917,  0},				// ����
		{{886, 889, 894}, 25, 918,  0},				// ����
		{{888, 891, 894}, 75, 918,  1},				// ����
		{{891, 892, 894}, 25, 919,  0},				// ����
		{{893, 889, 894}, 75, 919,  1},				// ����
		{{886, 891, 894}, 25, 920,  0},				// ����
		{{890, 892, 894}, 75, 920,  1},				// ����
		{{889, 892, 894}, 25, 921,  0},				// ����
		{{890, 886, 894}, 75, 921,  1},				// ����
		{{889, 892, 894}, 25, 922,  0},				// �尩
		{{888, 886, 894}, 75, 922,  1},				// �尩
		{{886, 889, 894}, 25, 923,  0},				// ����
		{{888, 891, 894}, 75, 923,  1},				// ����
		{{891, 892, 894}, 25, 924,  0},				// ����
		{{893, 889, 894}, 75, 924,  1},				// ����
		{{886, 891, 894}, 25, 925,  0},				// ����
		{{890, 892, 894}, 75, 925,  1},				// ����
		{{889, 892, 894}, 25, 926,  0},				// ����
		{{890, 886, 894}, 75, 926,  1},				// ����
		{{889, 892, 894}, 25, 927,  0},				// �尩
		{{888, 886, 894}, 75, 927,  1},				// �尩
		{{886, 892, 894}, 25, 1280, 0},				// ����
		{{893, 891, 894}, 75, 1280, 1},				// ����
		{{886, 889, 894}, 25, 928,  0},				// ����
		{{888, 891, 894}, 75, 928,  1},				// ����
		{{891, 892, 894}, 25, 929,  0},				// ����
		{{893, 889, 894}, 75, 929,  1},				// ����
		{{886, 891, 894}, 25, 930,  0},				// ����
		{{890, 892, 894}, 75, 930,  1},				// ����
		{{889, 892, 894}, 25, 931,  0},				// ����
		{{890, 886, 894}, 75, 931,  1},				// ����
		{{889, 892, 894}, 25, 932,  0},				// �尩
		{{888, 886, 894}, 75, 932,  1},				// �尩
		{{886, 889, 894}, 25, 933,  0},				// ����
		{{888, 891, 894}, 75, 933,  1},				// ����
		{{891, 892, 894}, 25, 934,  0},				// ����
		{{893, 889, 894}, 75, 934,  1},				// ����
		{{886, 891, 894}, 25, 935,  0},				// ����
		{{890, 892, 894}, 75, 935,  1},				// ����
		{{889, 892, 894}, 25, 936,  0},				// ����
		{{890, 886, 894}, 75, 936,  1},				// ����
		{{889, 892, 894}, 25, 937,  0},				// �尩
		{{888, 886, 894}, 75, 937,  1},				// �尩
		{{886, 889, 894}, 25, 938,  0},				// ����
		{{888, 891, 894}, 75, 938,  1},				// ����
		{{891, 892, 894}, 25, 939,  0},				// ����
		{{893, 889, 894}, 75, 939,  1},				// ����
		{{886, 891, 894}, 25, 940,  0},				// ����
		{{890, 892, 894}, 75, 940,  1},				// ����
		{{889, 892, 894}, 25, 941,  0},				// ����
		{{890, 886, 894}, 75, 941,  1},				// ����
		{{889, 892, 894}, 25, 942,  0},				// �尩
		{{888, 886, 894}, 75, 942,  1},				// �尩
		{{886, 889, 894}, 25, 943,  0},				// ����
		{{888, 891, 894}, 75, 943,  1},				// ����
		{{891, 892, 894}, 25, 944,  0},				// ����
		{{893, 889, 894}, 75, 944,  1},				// ����
		{{886, 891, 894}, 25, 945,  0},				// ����
		{{890, 892, 894}, 75, 945,  1},				// ����
		{{889, 892, 894}, 25, 946,  0},				// ����
		{{890, 886, 894}, 75, 946,  1},				// ����
		{{889, 892, 894}, 25, 947,  0},				// �尩
		{{888, 886, 894}, 75, 947,  1},				// �尩
	};

	int		nWantItemIndex = 0;		// ������� �ϴ� ������
	int		nMethod = 0;			// ����� ���
	int		nIndexTable = 0;		// �ش� ���� ��� �ε���
	int		i;				// ��� ������ ������
	CItem*	pItem[3] = { NULL, };				// ��� ������

#ifdef NPC_CHECK
	int npcIndex;
	CNPC *npc;
	RefMsg(msg) >> npcIndex
				>> nWantItemIndex
				>> nMethod;
	npc = ch->m_pArea->FindProtoNPCInCell(ch, npcIndex, false, 2);
	if(!npc)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetMixItemMsg(rmsg, MSG_EX_PET_MIX_ITEM_ERROR_NOT_EXIST_NPC);
		SEND_Q(rmsg, ch->m_desc);
		GAMELOG << init("ExPet MixItem FAIL: NOT EXIST NPC (Dangerous User)", ch) << end;
		return ;
	}
	if( !npc->IsFlag(NPC_EVENT) && !npc->IsFlag(NPC_QUEST) )
	{
		GAMELOG << init("ExPet MixItem FAIL: NOT EXIST NPC (Dangerous User)", ch) << end;
		return ;
	}
#else
	RefMsg(msg) >> nWantItemIndex
				>> nMethod;
#endif
	// ���̺��� �˻�
	for (nIndexTable = 0; nIndexTable < nSizeMixTable; nIndexTable++)
	{
		// ������� �ϴ� �����۰� ������� �˻�
		if (mixTable[nIndexTable].itemindex == nWantItemIndex && mixTable[nIndexTable].method == nMethod)
			break;
	}

	// ã�� ���� ������
	if (nIndexTable == nSizeMixTable)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetMixItemMsg(rmsg, MSG_EX_PET_MIX_ITEM_ERROR_NOMIX);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	// �ش� ��Ḧ ã��
	for (i = 0; i < 3; i++)
	{
		pItem[i] = ch->m_inventory.FindByDBIndex(mixTable[nIndexTable].needitem[i], 0, 0);
		if (pItem[i] == NULL)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			ExPetMixItemMsg(rmsg, MSG_EX_PET_MIX_ITEM_ERROR_NOITEM);
			SEND_Q(rmsg, ch->m_desc);
			return ;
		}
	}

	// ��� ����
	for (i = 0; i < 3; i++)
	{
		ch->m_inventory.decreaseItemCount(pItem[i], 1);
	}

	// Ȯ�� ����
	if (GetRandom(1, 100) <= mixTable[nIndexTable].prob)
	{
		// ������ ����
		CItem* pMixItem = gserver->m_itemProtoList.CreateItem(nWantItemIndex, -1, 0, 0, 1);
		if (pMixItem)
		{
			GAMELOG << init("ITEM MIX PET", ch)
					<< "CONSUME" << delim
					<< mixTable[nIndexTable].needitem[0] << delim
					<< mixTable[nIndexTable].needitem[1] << delim
					<< mixTable[nIndexTable].needitem[2] << delim
					<< "NEW ITEM" << delim
					<< itemlog(pMixItem) << delim;

			bool bDrop = false;
			if (ch->m_inventory.addItem(pMixItem) == false)
			{
				ch->m_pArea->DropItem(pMixItem, ch);
				pMixItem->m_preferenceIndex = ch->m_index;
				CNetMsg::SP rmsg(new CNetMsg);
				ItemDropMsg(rmsg, ch, pMixItem);
				ch->m_pArea->SendToCell(rmsg, GET_YLAYER(pMixItem), pMixItem->m_cellX, pMixItem->m_cellZ);
				bDrop = true;
			}

			GAMELOG << ((bDrop) ? "DROP" : "GIVE")
					<< end;

			{
				CNetMsg::SP rmsg(new CNetMsg);
				ExPetMixItemMsg(rmsg, MSG_EX_PET_MIX_ITEM_ERROR_OK);
				SEND_Q(rmsg, ch->m_desc);
			}
		}
		else
		{
			GAMELOG << init("ITEM MIX PET : FATAL ERROR", ch)
					<< "CONSUME" << delim
					<< mixTable[nIndexTable].needitem[0] << delim
					<< mixTable[nIndexTable].needitem[1] << delim
					<< mixTable[nIndexTable].needitem[2]
					<< end;

			CNetMsg::SP rmsg(new CNetMsg);
			ExPetMixItemMsg(rmsg, MSG_EX_PET_MIX_ITEM_ERROR_FAIL);
			SEND_Q(rmsg, ch->m_desc);
		}
	}
	else
	{
		// ���� �˸�
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetMixItemMsg(rmsg, MSG_EX_PET_MIX_ITEM_ERROR_FAIL);
		SEND_Q(rmsg, ch->m_desc);
	}
}

void do_ExPetWarpTown(CPC* ch, CNetMsg::SP& msg)
{
	if (!ch->GetPet() || !ch->GetPet()->IsMount())
		return ;

	int zone, extra;
	CZone* pZone = gserver->FindNearestZone(ch->m_pZone->m_index, GET_X(ch), GET_Z(ch), &zone, &extra, ch->getSyndicateType());
	if (pZone == NULL)
		return ;

	// ���� �̱� �������δ� �̵� �Ұ�
	if ((ch->m_pZone->IsPersonalDungeon() || ch->m_pZone->IsGuildRoom()) && ch->m_pZone->m_index == zone)
		return ;

	if (extra < 0 || extra >= pZone->m_countZonePos)
		return ;

	GoZone(ch, zone,
		   pZone->m_zonePos[extra][0],														// ylayer
		   GetRandom(pZone->m_zonePos[extra][1], pZone->m_zonePos[extra][3]) / 2.0f,		// x
		   GetRandom(pZone->m_zonePos[extra][2], pZone->m_zonePos[extra][4]) / 2.0f);		// z
}

void do_ExPetChangeItem(CPC* ch, CNetMsg::SP& msg)
{
#ifdef NPC_CHECK
	int npcIndex;
	CNPC *npc;
	RefMsg(msg) >> npcIndex;
	npc = ch->m_pArea->FindProtoNPCInCell(ch, npcIndex, false, 2);
	if(!npc)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetChangeItemMsg(rmsg, MSG_EX_PET_CHANGE_ITEM_ERROR_NOT_EXIST_NPC);
		SEND_Q(rmsg, ch->m_desc);
		GAMELOG << init("ExPet Change Item FAIL: NOT EXIST NPC (Dangerous User)", ch) << end;
		return ;
	}
	if( !npc->IsFlag(NPC_EVENT) && !npc->IsFlag(NPC_QUEST) )
	{
		GAMELOG << init("ExPet Change Item FAIL: NOT EXIST NPC (Dangerous User)", ch) << end;
		return ;
	}
#endif

	// ���� �����ϰ� �ִ°�?
	CPet* pet = ch->GetPet();
	if (!pet)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetChangeItemMsg(rmsg, MSG_EX_PET_CHANGE_ITEM_ERROR_FAIL);
		SEND_Q(rmsg, ch->m_desc);
		return ;
	}

	// ���� ���������� ��ȯ
	bool bSuccess = DropPetItem(pet);

	// TODO : petlog
	GAMELOG << init("PET CHANGE ITEM", ch)
			<< "PET" << delim
			<< pet->m_index << delim
			<< "TYPE" << delim
			<< pet->GetPetType() << delim
			<< "LEVEL" << delim
			<< pet->m_level << delim
			<< "SUCCESS" << delim
			<< ((bSuccess) ? 1 : 0)
			<< end;
	{
		CNetMsg::SP rmsg(new CNetMsg);
		if (bSuccess)
			ExPetChangeItemMsg(rmsg, MSG_EX_PET_CHANGE_ITEM_ERROR_OK);
		else
			ExPetChangeItemMsg(rmsg, MSG_EX_PET_CHANGE_ITEM_ERROR_NOITEM);

		SEND_Q(rmsg, ch->m_desc);
	}

	{
		CNetMsg::SP rmsg(new CNetMsg);
		HelperPetDeleteReqMsg(rmsg, pet->m_index, ch->m_index);
		SEND_Q(rmsg, gserver->m_helper);
	}

	// �� ����
	ch->DelPet(pet->m_index);
}

void do_ExPetRebirth(CPC* ch, CNetMsg::SP& msg)
{
	int			nPetIndex = 0;
	CPet*		pet;
	LONGLONG	nNeedNas;

	RefMsg(msg) >> nPetIndex;

	pet = ch->GetPet(nPetIndex);

	if (pet == NULL)
	{
		// �� ������ ����
		CNetMsg::SP rmsg(new CNetMsg);
		ExPetRebirthMsg(rmsg, nPetIndex, MSG_EX_PET_REBIRTH_ERROR_NOPET);
		SEND_Q(rmsg, ch->m_desc);
	}
	else
	{
		// ���� ������
		if (pet->GetRemainRebirthTime() < 1)
		{
			// ��� ������ ����
			CNetMsg::SP rmsg(new CNetMsg);
			ExPetRebirthMsg(rmsg, nPetIndex, MSG_EX_PET_REBIRTH_ERROR_NODEAD);
			SEND_Q(rmsg, ch->m_desc);
		}
		else
		{
			bool bRebirth = true;

			// �ʿ䳪�� ����
			nNeedNas = (pet->m_level - 1) * pet->m_level * pet->m_level * 7;

			if (nNeedNas > 0)
			{
				// ������ �ʿ��ϸ� �����ݰ� ��
				if (ch->m_inventory.getMoney() < nNeedNas)
				{
					// ������ �����ϸ� ����
					CNetMsg::SP rmsg(new CNetMsg);
					ExPetRebirthMsg(rmsg, nPetIndex, MSG_EX_PET_REBIRTH_ERROR_NOMONEY);
					SEND_Q(rmsg, ch->m_desc);
					bRebirth = false;
				}
				else
				{
					ch->m_inventory.decreaseMoney(nNeedNas);
					// ������ ����ϸ� ��� ����
				}
			} // �ʿ� ���� �˻�

			if (bRebirth)
			{
				// ��� ������ �� ��Ȱ
				pet->SetRemainRebirthTime(0);
				pet->m_hp = 30;

				{
					CNetMsg::SP rmsg(new CNetMsg);
					ExPetStatusMsg(rmsg, pet);
					SEND_Q(rmsg, ch->m_desc);
				}

				// TODO : petlog
				GAMELOG << init("PET REBIRTH", ch)
						<< "PET" << delim
						<< pet->m_index << delim
						<< "TYPE" << delim
						<< pet->GetPetType() << delim
						<< "LEVEL" << delim
						<< pet->m_level << delim
						<< "MONEY" << delim
						<< nNeedNas
						<< end;

				{
					CNetMsg::SP rmsg(new CNetMsg);
					ExPetRebirthMsg(rmsg, nPetIndex, MSG_EX_PET_REBIRTH_ERROR_OK);
					SEND_Q(rmsg, ch->m_desc);
				}
			}
		} // �� ��� �˻�
	} // �� ���� �˻�
}

void do_PetNameChange(CPC* ch, CNetMsg::SP& msg)
{
	int			nPetIndex = 0;
	CLCString	strPetName( 30 );
	CPet*		pet;

	RefMsg(msg) >> nPetIndex
				>> strPetName;

	bool bApet = false;

	if( strlen( strPetName ) < 4 || strlen(strPetName) > 16  )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		PetNameChange( rmsg, MSG_EX_PET_CHANGE_NAME_ERROR_FAIL, nPetIndex, strPetName );
		SEND_Q( rmsg, ch->m_desc );
		return;
	}

	if( strinc( strPetName, "'" ) || strinc( strPetName, " " ) )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		PetNameChange( rmsg, MSG_EX_PET_CHANGE_NAME_ERROR_FAIL, nPetIndex, strPetName );
		SEND_Q( rmsg, ch->m_desc );
		return;
	}

	if(findPercentChar(strPetName.getBuffer()) != NULL)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		PetNameChange( rmsg, MSG_EX_PET_CHANGE_NAME_ERROR_FAIL, nPetIndex, strPetName );
		SEND_Q( rmsg, ch->m_desc );
		return;
	}

// [selo: 101229] ���þƴ� ���þƾ� �̿��� ���� ���� �ȵȴ�.
#ifdef LC_RUS
	if( CheckNoRussianCharacter(strPetName) )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		PetNameChange( rmsg, MSG_EX_PET_CHANGE_NAME_ERROR_FAIL, nPetIndex, strPetName );
		SEND_Q( rmsg, ch->m_desc );
		return;
	}
#endif // LC_RUS

	CItem* pItem = ch->m_inventory.FindByDBIndex(2360, 0, 0 );
	if (pItem == NULL)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		PetNameChange( rmsg, MSG_EX_PET_CHANGE_NAME_ERROR_ITEM, nPetIndex, strPetName );
		SEND_Q( rmsg, ch->m_desc );
		return;
	}
	CAPet* apet = ch->GetAPet();
	if( apet )
		bApet = true;

	pet = ch->GetPet( nPetIndex );
	if( !bApet && pet == NULL )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		PetNameChange( rmsg, MSG_EX_PET_CHANGE_NAME_ERROR_NOPET, nPetIndex, strPetName );
		SEND_Q( rmsg, ch->m_desc );
		return;
	}

	if( bApet )
	{
		ch->m_inventory.decreaseItemCount(pItem, 1);
		apet->m_name = strPetName;
	}
	else
	{
		CNetMsg::SP rmsg(new CNetMsg);
		HelperPetNameChange( rmsg, ch->m_index, pet->m_index, strPetName );
		SEND_Q( rmsg, gserver->m_helper );
	}
}

