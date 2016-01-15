#include "stdhdrs.h"

#include <vector>
#include <map>

#include "HolyWaterData.h"
#include "Server.h"
#include "Character.h"
#include "../ShareLib/DBCmd.h"


HolyWaterData::HolyWaterData(){}

HolyWaterData::~HolyWaterData(){}

HolyWaterData* HolyWaterData::instance()
{
	static HolyWaterData __instance;
	return &__instance;
}

void HolyWaterData::load()
{
	std::string query = "SELECT * FROM t_holy_water";
	
	CDBCmd cmd;
	cmd.Init(&gserver->m_dbdata);
	cmd.SetQuery(query);
	if( cmd.Open() == false )
	{
		LOG_ERROR("LOAD HOLY_WATER DATA FAIL. ERROR[%s]", mysql_error(&gserver->m_dbdata));
	}
	
	
	int itemIndex, type, physical_npc, physical_pc, range_npc, range_pc, magic_npc, magic_pc, skill_npc, skill_pc;

	while(cmd.MoveNext() == true)
	{
		HOLY_WATER_DATA data;
		
		cmd.GetRec("a_item_index", itemIndex);
		cmd.GetRec("a_type", data.type);
		cmd.GetRec("a_physical_npc", physical_npc);
		cmd.GetRec("a_physical_pc", physical_pc);
		cmd.GetRec("a_range_npc", range_npc);
		cmd.GetRec("a_range_pc", range_pc);
		cmd.GetRec("a_magic_npc", magic_npc);
		cmd.GetRec("a_magic_pc", magic_pc);
		cmd.GetRec("a_skill_npc", skill_npc);
		cmd.GetRec("a_skill_pc", skill_pc);
		
		data._data.push_back(physical_npc);
		data._data.push_back(range_npc);
		data._data.push_back(magic_npc);
		data._data.push_back(skill_npc);
		data._data.push_back(physical_pc);
		data._data.push_back(range_pc);
		data._data.push_back(magic_pc);
		data._data.push_back(skill_pc);

		_map.insert(HOLY_WATER_MAP::value_type(itemIndex, data));
	}
}

int HolyWaterData::getDamage(CPC* opc, CCharacter* df, const CMagicProto* magic, int damage)
{
	//��Ȳ�� ������ ���
	//1. PC���ݰ� NPC���� 2���� �з�
	int damageType;	
	
	if(IS_PC(df) == true)
		damageType = getToPcDamageType(opc, magic);
	
	else if(IS_NPC(df) == true)
		damageType = getToNpcDamageType(opc, magic);
	
	else
		return damage;

	if(damageType <= -1)
		return damage;

	//������ ���
	HOLY_WATER_DATA _data;
	HOLY_WATER_MAP::iterator it;
	it = _map.find(opc->holy_water_item->m_itemProto->getItemIndex());

	if(it == _map.end())
		return damage;

	//type == 0�̸� rate
	if(it->second.type == 0)
		damage += (float)damage * (float)it->second._data[damageType] / 100;
	
	//type == 1�̸� addtion
	else if(it->second.type == 1)
		damage += it->second._data[damageType];

	opc->m_inventory.decreaseItemCount(opc->holy_water_item, 1);

	return damage;
}

int HolyWaterData::getToPcDamageType(CPC* opc, const CMagicProto* magic)
{
	//2. ���� ���Ÿ�, ����, ��ų ���ݿ� ���� �з�		
	//getDamage �Լ� �� �ϱ����� �������� ���� �������� ������ �Ǵ� �� �� ȣ���ϵ��� �Ѵ�.
	int damageType = -1;

	if(magic != NULL)
	{
		damageType = SKILL_PC;		
	}
	else
	{
		CItem* pItem = opc->m_wearInventory.getWearItem(WEARING_WEAPON);
		if (pItem == NULL)
			return -1;

		switch(pItem->m_itemProto->getItemSubTypeIdx())
		{
			//����
		case IWEAPON_NIGHT:
		case IWEAPON_BIGSWORD:
		case IWEAPON_AXE:
		case IWEAPON_SHORTGUM:
		case IWEAPON_TWOSWORD:
			damageType = PHYSICAL_PC;
			break;
			// ���Ÿ�
		case IWEAPON_CROSSBOW:
		case IWEAPON_BOW:
			damageType = RANGE_PC;
			break;
			// ����
		case IWEAPON_STAFF:
		case IWEAPON_SHORTSTAFF:
		case IWEAPON_WAND:
		case IWEAPON_SCYTHE:
		case IWEAPON_POLEARM:
		case IWEAPON_SOUL:
			damageType = MAGIC_PC;
			break;
		default:
			damageType = -1;
			break;
		}
	}
	
	return damageType;
}

int HolyWaterData::getToNpcDamageType(CPC* opc, const CMagicProto* magic)
{
	int damageType = -1;

	if(magic != NULL)
	{
		damageType = SKILL_NPC;
	}
	else
	{
		CItem* item = opc->m_wearInventory.getWearItem(WEARING_WEAPON);
		if(item == NULL)
		{
			damageType = -1;
			return damageType;
		}

		switch(item->m_itemProto->getItemSubTypeIdx())
		{
			//����
		case IWEAPON_NIGHT:
		case IWEAPON_BIGSWORD:
		case IWEAPON_AXE:
		case IWEAPON_SHORTGUM:
		case IWEAPON_TWOSWORD:
			damageType = PHYSICAL_NPC;
			break;
			// ���Ÿ�
		case IWEAPON_CROSSBOW:
		case IWEAPON_BOW:
			damageType = RANGE_NPC;
			break;
			// ����
		case IWEAPON_STAFF:
		case IWEAPON_SHORTSTAFF:
		case IWEAPON_WAND:
		case IWEAPON_SCYTHE:
		case IWEAPON_POLEARM:
		case IWEAPON_SOUL:
			damageType = MAGIC_NPC;
			break;
		default:
			damageType = -1;
			break;
		}
	}

	return damageType;
}
