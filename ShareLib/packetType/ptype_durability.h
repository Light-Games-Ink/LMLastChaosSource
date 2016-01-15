#ifndef __PTYPE_DURABILITY_H__
#define __PTYPE_DURABILITY_H__

#include "ptype_base.h"

// MAIN TYPE = MSG_DURABILITY
enum // SUB TYPE
{
	MSG_SUB_DURABILITY_NORMAL_REPAIR,		// ��� ������ �Ϲ� ����
	MSG_SUB_DURABILITY_SPECIAL_REPAIR,		// ��� ������ Ư�� ����
	MSG_SUB_DURABILITY_RECOVERY,			// ��� ������ ����
};

enum
{
	ERR_NO_DURABILITY_OK,				// ����
	ERR_NO_DURABILITY_NOT_ENOUGH_MONEY,	// ���� ����
	ERR_NO_DURABILITY_NOT_ENOUGH_ITEM_COUNT,	// �ź��� ��ġ ������ ������
	ERR_NO_DURABILITY_TOO_MUSH_ITEM_COUNT,	// �ź��� ��ġ ������ �ʹ� �����ϴ�.
	ERR_NO_DURABILITY_DO_NOT_REPAIR,		// ������ �ʿ䰡 ����
	ERR_NO_DURABILITY_DO_NOT_RECOVERY,		// ������ �ʿ䰡 ����
};

#pragma pack(push, 1)
//////////////////////////////////////////////////////////////////////////
namespace RequestClient
{
struct repairDurability : public pTypeBase	// ��� ������ �Ϲ� ����
{
	unsigned short		tab_no;
	unsigned short		invenIndex;
	int					virualIndex;
	LONGLONG			fee;
};

struct specialRepairDurability : public pTypeBase // ��� ������ Ư�� ����
{
	unsigned short		tab_no;
	unsigned short		invenIndex;
	int					virualIndex;

	// Ư�� ���
	unsigned short		material_tab_no;
	unsigned short		material_invenIndex;
	int					material_virtualIndex;
};

struct recoveryDurability : public pTypeBase // ��� ������ ����
{
	unsigned short		tab_no;
	unsigned short		invenIndex;
	int					virualIndex;

	// Ư�� ���
	unsigned short		material_tab_no;
	unsigned short		material_invenIndex;
	int					material_virtualIndex;
	unsigned short		material_count;
};
}

namespace ResponseClient
{
struct repairDurability : public pTypeBase	// ��� ������ �Ϲ� ����
{
	unsigned char		result;
};

struct specialRepairDurability : public pTypeBase // ��� ������ Ư�� ����
{
	unsigned char		result;
};

struct recoveryDurability : public pTypeBase // ��� ������ ����
{
	unsigned char		result;
};

#ifndef _CLIENT_
inline void makeRepairDurability(CNetMsg::SP& msg, int result)
{
	repairDurability* packet = reinterpret_cast<repairDurability*>(msg->m_buf);
	packet->type = MSG_DURABILITY;
	packet->subType = MSG_SUB_DURABILITY_NORMAL_REPAIR;
	packet->result = result;
	msg->setSize(sizeof(repairDurability));
}

inline void makeSpecialRepairDurability(CNetMsg::SP& msg, int result)
{
	specialRepairDurability* packet = reinterpret_cast<specialRepairDurability*>(msg->m_buf);
	packet->type = MSG_DURABILITY;
	packet->subType = MSG_SUB_DURABILITY_SPECIAL_REPAIR;
	packet->result = result;
	msg->setSize(sizeof(specialRepairDurability));
}

inline void makeRecoveryDurability(CNetMsg::SP& msg, int result)
{
	recoveryDurability* packet = reinterpret_cast<recoveryDurability*>(msg->m_buf);
	packet->type = MSG_DURABILITY;
	packet->subType = MSG_SUB_DURABILITY_RECOVERY;
	packet->result = result;
	msg->setSize(sizeof(recoveryDurability));
}
#endif
}

namespace UpdateClient
{
struct itemDurabilityForInventory : public pTypeBase
{
	unsigned short		tab;
	unsigned short		invenIndex;

	unsigned short		now_durability; // ���� ��� ������
	unsigned short		max_durability; // �ִ� ��� ������
};

struct itemDurabilityForWear: public pTypeBase
{
	unsigned short		wearPos;

	unsigned short		now_durability; // ���� ��� ������
};

#ifndef _CLIENT_
inline void makeItemDurabilityForInventory(CNetMsg::SP& msg, int tab, int invenIndex, int now_dur, int max_dur)
{
	itemDurabilityForInventory* packet = reinterpret_cast<itemDurabilityForInventory*>(msg->m_buf);
	packet->type = MSG_UPDATE_DATA_FOR_CLIENT;
	packet->subType = MSG_SUB_UPDATE_ITEM_DURABILITY_FOR_INVENTORY;
	packet->tab = tab;
	packet->invenIndex = invenIndex;
	packet->now_durability = now_dur;
	packet->max_durability = max_dur;
	msg->setSize(sizeof(itemDurabilityForInventory));
}

inline void makeItemDurabilityForWear(CNetMsg::SP& msg, int wearPos, int now_dur)
{
	itemDurabilityForWear* packet = reinterpret_cast<itemDurabilityForWear*>(msg->m_buf);
	packet->type = MSG_UPDATE_DATA_FOR_CLIENT;
	packet->subType = MSG_SUB_UPDATE_ITEM_DURABILITY_FOR_WEAR;
	packet->wearPos = wearPos;
	packet->now_durability = now_dur;
	msg->setSize(sizeof(itemDurabilityForWear));
}
#endif
}
//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)

#endif
