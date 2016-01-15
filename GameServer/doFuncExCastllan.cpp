#include "stdhdrs.h"

#include "Server.h"
#include "CmdMsg.h"
#include "doFunc.h"
#include "Log.h"

void do_ExFuncCastllan( CPC* ch, CNetMsg::SP& msg )
{
	unsigned char subType;

	RefMsg(msg) >> subType;

	// �޶�ũ ����
	// ���ź ����
	const static int CastllanItemTable7[JOBCOUNT][6]=
	{
		{6422, 6423, 6424, 6425, 6426, 4690}, // Ÿ��ź
		{6427, 6428, 6429, 6430, 6431, 4690}, // ����Ʈ
		{6432, 6433, 6434, 6435, 6436, 4690}, // ����
		{6437, 6438, 6439, 6440, 6441, 4690}, // ������
		{6442, 6443, 6444, 6445, 6446, 4690}, // �α�
		{6447, 6448, 6449, 6450, 6451, 4690}, // �Ҽ���
		{6452, 6453, 6454, 6455 ,6456, 4690}, // ����Ʈ ������
#ifdef EX_ROGUE
		{6442, 6443, 6444, 6445, 6446, 4690}, // EX �α�
#endif //EX_ROGUE
#ifdef EX_MAGE
		{6437, 6438, 6439, 6440, 6441, 4690}, // EX ������
#endif // EX_MAGE
	};

	const static int CastllanItemTable4[JOBCOUNT][6]=
	{
		{6181, 6182, 6183, 6184, 6185, 5968}, // Ÿ��ź
		{6186, 6187, 6188, 6189, 6190, 5968}, // ����Ʈ
		{6191, 6192, 6193, 6194, 6195, 5968}, // ����
		{6196, 6197, 6198, 6199, 6200, 5968}, // ������
		{6201, 6202, 6203, 6204, 6205, 5968}, // �α�
		{6206, 6207, 6208, 6209, 6210, 5968}, // �Ҽ���
		{6211, 6212, 6213, 6214 ,6215, 5968}, // ����Ʈ ������
#ifdef EX_ROGUE
		{6201, 6202, 6203, 6204, 6205, 5968}, // EX �α�
#endif //EX_ROGUE
#ifdef EX_MAGE
		{6196, 6197, 6198, 6199, 6200, 5968}, // EX ������
#endif // EX_MAGE
	};

	// �޶�ũ ���ֱ���
	// ���ź ���ֱ���
	const static int CastllanMemberItemTable7[JOBCOUNT][5]=
	{
		{6492, 6493, 6494, 6495, 6496}, // Ÿ��ź
		{6497, 6498, 6499, 6500, 6501}, // ����Ʈ
		{6502, 6503, 6504, 6505, 6506}, // ����
		{6507, 6508, 6509, 6510, 6511}, // ������
		{6512, 6513, 6514, 6515, 6516}, // �α�
		{6517, 6518, 6519, 6520, 6521}, // �Ҽ���
		{6522, 6523, 6524, 6525 ,6526}, // ����Ʈ ������
#ifdef EX_ROGUE
		{6512, 6513, 6514, 6515, 6516}, // EX �α�
#endif //EX_ROGUE
#ifdef EX_MAGE
		{6507, 6508, 6509, 6510, 6511}, // EX ������
#endif // EX_MAGE
	};

	const static int CastllanMemberItemTable4[JOBCOUNT][5] =
	{
		{6457, 6458, 6459, 6460, 6461}, // Ÿ��ź
		{6462, 6463, 6464, 6465, 6466}, // ����Ʈ
		{6467, 6468, 6469, 6470, 6471}, // ����
		{6472, 6473, 6474, 6475, 6476}, // ������
		{6477, 6478, 6479, 6480, 6481}, // �α�
		{6482, 6483, 6484, 6485, 6486}, // �Ҽ���
		{6487, 6488, 6489, 6490 ,6491}, // ����Ʈ ������
#ifdef EX_ROGUE
		{6477, 6478, 6479, 6480, 6481}, // EX �α�
#endif //EX_ROGUE
#ifdef EX_MAGE
		{6472, 6473, 6474, 6475, 6476}, // EX ������
#endif // EX_MAGE
	};

	const static int CastllanMemberTitleItemidx7 = 5964;
	const static int CastllanMemberTitleItemidx4 = 5963;

	int itemcount;
	const int *itemidx;
	char bCastellan;
	int zonenum;

	switch(subType)
	{
	case MSG_EX_CASTLLAN_GIVE_ITEM_REQ:					// ���ֺ���
		{
			if(!ch->CheckCastellanType(zonenum, bCastellan) || !bCastellan)
			{
				CNetMsg::SP rmsg(new CNetMsg);
				CastllanErrorMsg( rmsg, MSG_EX_CASTLLAN_ERROR_NOT_CASTLLAN );
				SEND_Q(rmsg, ch->m_desc);
				return ;
			}

			// �޶�ũ��������, ���ź��������
			itemcount = 6;
			if(zonenum == ZONE_MERAC && ch->m_pZone->m_index == ZONE_MERAC)
				itemidx = CastllanItemTable7[ch->m_job];
			else if(zonenum == ZONE_DRATAN && ch->m_pZone->m_index == ZONE_DRATAN)
				itemidx = CastllanItemTable4[ch->m_job];
			else
				return ;
		}
		break;
	case MSG_EX_CASTLLAN_GIVE_ITEM_GUILD_COSTUME_REQ:	// ���ֱ���ǻ󺸻�
		{
			if(!ch->CheckCastellanType(zonenum, bCastellan) || bCastellan)
			{
				CNetMsg::SP rmsg(new CNetMsg);
				CastllanErrorMsg( rmsg, MSG_EX_CASTLLAN_ERROR_NOT_CASTLLAN );
				SEND_Q(rmsg, ch->m_desc);
				return ;
			}

			// �޶�ũ���ֱ������, ���ź���ֱ������
			itemcount = 5;
			if(zonenum == ZONE_MERAC)
				itemidx = CastllanMemberItemTable7[ch->m_job];
			else if(zonenum == ZONE_DRATAN)
				itemidx = CastllanMemberItemTable4[ch->m_job];
			else
				return ;
		}
		break;
	case MSG_EX_CASTLLAN_GIVE_ITEM_GUILD_TITLE_REQ:		// ���ֱ��ȣĪ����
		{
			if(!ch->CheckCastellanType(zonenum, bCastellan) || bCastellan)
			{
				CNetMsg::SP rmsg(new CNetMsg);
				CastllanErrorMsg( rmsg, MSG_EX_CASTLLAN_ERROR_NOT_CASTLLAN );
				SEND_Q(rmsg, ch->m_desc);
				return ;
			}

			itemcount = 1;
			if(zonenum == ZONE_MERAC)
				itemidx = &CastllanMemberTitleItemidx7;
			else if(zonenum == ZONE_DRATAN)
				itemidx = &CastllanMemberTitleItemidx4;
			else
				return ;
		}
		break;
	default :
		return ;
	}

	int i;
	for(i=0; i<itemcount; i++)
	{
		CItemProto * pItem = gserver->m_itemProtoList.FindIndex( itemidx[i]);
		if( !pItem )
		{
			// ������ �������� ������ ����. ������ ������ error �޽��� ������.
			CNetMsg::SP rmsg(new CNetMsg);
			CastllanErrorMsg( rmsg, MSG_EX_CASTLLAN_ERROR_GIVE_ITEM_ERROR );
			SEND_Q(rmsg, ch->m_desc);
			return ;
		}

		if (ch->m_inventory.FindByDBIndex(itemidx[i]))
			continue ; // �̹� ���޹���.

		// �κ� üũ
		if( ch->m_inventory.getEmptyCount() < 1 )
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SysFullInventoryMsg(rmsg, 0);
			SEND_Q(rmsg, ch->m_desc);
			return ;
		}

		if( !ch->GiveItem(itemidx[i], 0, 0, 1 ) )
		{
			// �����ߴٸ� ���� �޽����� ������.
			CNetMsg::SP rmsg(new CNetMsg);
			CastllanErrorMsg( rmsg, MSG_EX_CASTLLAN_ERROR_GIVE_ITEM_ERROR );
			SEND_Q(rmsg, ch->m_desc);
			return ;
		}
	}
}
