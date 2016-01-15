#include "stdhdrs.h"

#include "Server.h"
#include "Battle.h"
#include "WarCastle.h"
#include "Log.h"
#include "CmdMsg.h"
#include "doFunc.h"
#include "../ShareLib/packetType/ptype_old_do_item.h"

void ProcDead(CPet* df, CCharacter* of)
{
	CPC*		opc				= NULL;
	CNPC*		onpc			= NULL;
	CPet*		opet			= NULL;
	CAPet*		oapet			= NULL;
	CElemental*	oelemental		= NULL;

	if( IS_NPC(of) && TO_NPC(of)->Check_MobFlag(STATE_MONSTER_MERCENARY) && TO_NPC(of)->GetOwner() )
	{
		TO_NPC(of)->GetOwner()->SetSummonOwners_target(NULL);
	}

	switch (of->m_type)
	{
	case MSG_CHAR_PC:
		opc = TO_PC(of);
		break;

	case MSG_CHAR_NPC:
		onpc = TO_NPC(of);
		break;

	case MSG_CHAR_PET:
		opet = TO_PET(of);
		opc = opet->GetOwner();
		break;

	case MSG_CHAR_ELEMENTAL:
		oelemental = TO_ELEMENTAL(of);
		opc = oelemental->GetOwner();
		break;

	case MSG_CHAR_APET:
		oapet = TO_APET(of);
		opc = oapet->GetOwner();
		break;

	default:
		return ;
	}

	if( opc )
		opc->SetSummonOwners_target(NULL);

	if (df->GetOwner())
	{
		// NPC ����� ��� �г�Ƽ�� �⺻���� true, PC���� ����� ��� �г�Ƽ�� �⺻���� false
		// * bPKPenalty������ pk �г�Ƽ�� �ִ°� �Ӹ� �ƴ϶� ����ȸ������ ����ǹǷ� ���� ȸ���̳� �г�Ƽ�� ����Ϳ��� �ɸ��� true
		bool bPKPenalty = (opc) ? IsPK(opc, df) : false;

		if (bPKPenalty)
			CalcPKPoint(opc, df->GetOwner(), true);
	}

	DelAttackList(df);

	CPC* owner = df->GetOwner();
	const char* ownerName = "NO OWNER";
	const char* ownerNick = "NO OWNER";
	const char* ownerID = "NO OWNER";
	if (owner)
	{
		ownerNick = (owner->IsNick()) ? owner->GetName() : ownerNick;
		ownerName = owner->m_name;
		ownerID = owner->m_desc->m_idname;
	}

	// TODO : petlog
	GAMELOG << init("PET DEAD")
			<< "PET" << delim
			<< df->GetPetTypeGrade() << delim
			<< "INDEX" << delim
			<< df->m_index << delim
			<< "LEVEL" << delim
			<< df->m_level << delim
			<< "OWNER" << delim
			<< ownerName << delim
			<< ownerNick << delim
			<< ownerID << delim
			<< "ATTACKER" << delim
			<< "TYPE" << delim;
	switch (of->m_type)
	{
	case MSG_CHAR_NPC:
		GAMELOG << "NPC" << delim
				<< onpc->m_name
				<< end;
		break;
	case MSG_CHAR_PC:
	case MSG_CHAR_PET:
	case MSG_CHAR_ELEMENTAL:
	default:
		if (opc)
		{
			GAMELOG << "PC" << delim
					<< opc->m_index << delim
					<< opc->GetName()
					<< end;
		}
		else
		{
			GAMELOG << "UNKNOWN" << delim
					<< of->m_index << delim
					<< of->m_name
					<< end;
		}
		break;
	}

	// 060221 : bs : �ֿϵ��� ����� ���� �����ϰ� ���� ���� �ð����� ���� �Ҵ�
	if (owner)
	{
		// ��� ����
		df->SetRemainRebirthTime();

		{
			// �� ���� ����
			CNetMsg::SP rmsg(new CNetMsg);
			ExPetStatusMsg(rmsg, df);
			SEND_Q(rmsg, owner->m_desc);
		}
	}
}

// �� ����� ������ ����
bool DropPetItem(CPet* pet)
{
	bool bRet = false;

	CPC* owner = pet->GetOwner();

	if (owner)
	{
		const int nHorseDropList = 8;
		const int nDragonDropList = 12;

		int horseDropList[nHorseDropList][2] =
		{
			{886, 70},			// ����
			{888, 30},			// ���ǰ���
			{889, 70},			// ����
			{890, 30},			// ���ǵ��
			{891, 70},			// ���Ǿ�ݴ�
			{892, 70},			// ������
			{893, 30},			// ������
			{894, 100}			// ��������
		};
		int dragonDropList[nDragonDropList][2] =
		{
			{895, 45},			// ����
			{896, 45},			// ����
			{897, 45},			// �۰���
			{898, 45},			// ����
			{899, 45},			// ��
			{900, 45},			// ����
			{901, 45},			// ����
			{902, 45},			// ����
			{903, 45},			// ��
			{904, 45},			// ����
			{905, 45},			// ���
			{906, 45},			// ���
		};

		// �� ������ ���� ��� ���̺� ����
		bool bNoDrop = true;
		int i;
		int prob = GetRandom(1, 10000);
		int selIndex = 0;	// ������ �ε���
		int nStartIndex;
		int j;

		switch (pet->GetPetType())
		{
		case PET_TYPE_HORSE:
		case PET_TYPE_BLUE_HORSE:
		case PET_TYPE_UNKOWN_HORSE:
			nStartIndex = GetRandom(0, nHorseDropList - 1);
			for (i = 0; i < nHorseDropList; i++)
			{
				j = (i + nStartIndex) % nHorseDropList;
				horseDropList[j][1] = horseDropList[j][1] * pet->m_level / 2;
				if (i > 0)
				{
					horseDropList[j][1] += horseDropList[(j + nHorseDropList - 1) % nHorseDropList][1];
				}

				if (prob <= horseDropList[j][1])
				{
					selIndex = horseDropList[j][0];
					bNoDrop = false;
					break;
				}
			}
			break;
		case PET_TYPE_DRAGON:
		case PET_TYPE_PINK_DRAGON:
		case PET_TYPE_UNKOWN_DRAGON:
			nStartIndex = GetRandom(0, nDragonDropList - 1);
			for (i = 0; i < nDragonDropList; i++)
			{
				j = (i + nStartIndex) % nDragonDropList;
				dragonDropList[j][1] = dragonDropList[j][1] * pet->m_level / 2;
				if (i > 0)
					dragonDropList[j][1] += dragonDropList[(j + nDragonDropList - 1) % nDragonDropList][1];

				if (prob <= dragonDropList[j][1])
				{
					selIndex = dragonDropList[j][0];
					bNoDrop = false;
					break;
				}
			}
			break;
		default:
			bNoDrop = true;
		}

		if (!bNoDrop)
		{
			CItem* dropItem = gserver->m_itemProtoList.CreateItem(selIndex, -1, 0, 0, 1);
			if (dropItem)
			{
				// TODO : petlog
				GAMELOG << init("PET ITEM CHANGE")
						<< "PET" << delim
						<< pet->GetPetTypeGrade() << delim
						<< "INDEX" << delim
						<< pet->m_index << delim
						<< "LEVEL" << delim
						<< pet->m_level << delim
						<< "OWNER" << delim
						<< owner->m_index << delim
						<< owner->GetName() << delim
						<< "ITEM" << delim
						<< itemlog(dropItem, true) << delim;

				bool bDrop = false;
				if (owner->m_inventory.addItem(dropItem) == false)
				{
					owner->m_pArea->DropItem(dropItem, owner);
					dropItem->m_preferenceIndex = owner->m_index;
					CNetMsg::SP rmsg(new CNetMsg);
					ItemDropMsg(rmsg, owner, dropItem);
					owner->m_pArea->SendToCell(rmsg, GET_YLAYER(dropItem), dropItem->m_cellX, dropItem->m_cellZ);
					bDrop = true;
				}

				GAMELOG << ((bDrop) ? "DROP" : "GIVE")
						<< end;

				bRet = true;
			}
		}
	}

	return bRet;
}
