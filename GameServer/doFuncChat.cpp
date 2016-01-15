#include "stdhdrs.h"

#include "Log.h"
#include "Character.h"
#include "Server.h"
#include "CmdMsg.h"
#include "WarCastle.h"

/////////////////
// ä�� ���� �Լ�
void do_Chat(CPC* ch, CNetMsg::SP& msg)
{
	// ch�� ���� ���� ���� ��, ���϶�
	if (!IS_IN_CELL(ch))
		return ;

	msg->MoveFirst();

	unsigned char chatType;
	int sender_index;
	CLCString sender_name(MAX_CHAR_NAME_LENGTH + 1);
	CLCString receiver_name(MAX_CHAR_NAME_LENGTH + 1);
	CLCString chat(1000);
	int	nGroupTyp=-1;  	//������ �׷�1(0), �׷� 2(1),  �׷� 3(2), �׷� 4(3) , ��ü(-1)

	RefMsg(msg) >> chatType
				>> sender_index
				>> sender_name
				>> receiver_name
				>> chat
				>> nGroupTyp;

	sender_name.Trim();
	receiver_name.Trim();
	chat.Trim();

	CLCString tmp(1000);

	// ���� ���� ä���� ����
	if (chat.Length() < 1)
		return ;

	// ä�� ���� Check
	if (gserver->m_pulse - ch->m_silencePulse <= 0)
		return;

#ifdef NO_CHATTING

	switch (chatType)
	{
	case MSG_CHAT_SAY:		// �Ϲ�
		{
			if (ch->m_nflag & CHAT_FLAG_NO_SAY) return;
			tmp.Format("SAY: %s: %s", ch->GetName(), (const char*)chat);
		}
		break;
	case MSG_CHAT_SHOUT:	// ��ġ��
		{
			if (ch->m_nflag & CHAT_FLAG_NO_SHOUT) return;
			tmp.Format("SHOUT: %s: %s", ch->GetName(), (const char*)chat);
		}
		break;
	case MSG_CHAT_TRADE:	// �Ÿ�
		{
			if (ch->m_nflag & CHAT_FLAG_NO_TRADE) return;
			tmp.Format("TRADE: %s: %s", ch->GetName(), (const char*)chat);
		}
		break;
	case MSG_CHAT_WHISPER:	// �Ӹ�
		{
			if (ch->m_nflag & CHAT_FLAG_NO_WHISPER) return;
			tmp.Format("WHISPER: %s ==> %s: %s", ch->GetName(), (const char*)receiver_name, (const char*)chat);
		}
		break;
	case MSG_CHAT_PARTY:	// ��Ƽ
		{
			if (ch->m_nflag & CHAT_FLAG_NO_PARTY) return;
			tmp.Format("PARTYCHAT: %s: %s", ch->GetName(), (const char*)chat);
		}
		break;
	case MSG_CHAT_EXPEDITION:	// ������
		{
			if (ch->m_nflag & CHAT_FLAG_NO_EXPED) return;
			tmp.Format("EXPEDCHAT: %s: %s", ch->GetName(), (const char*)chat);
		}
		break;
	case MSG_CHAT_GUILD:	// ���
		{
			if (ch->m_nflag & CHAT_FLAG_NO_GUILD) return;
			tmp.Format("GUILDCHAT: %s: %s", ch->GetName(), (const char*)chat);
		}
		break;
	case MSG_CHAT_GM:		// GM �Ϲ�ä��
		{
			tmp.Format("GM SAY: %s: %s", ch->GetName(), (const char*)chat);
		}
		break;
	case MSG_CHAT_LORD:		// ���� ����
		{
			tmp.Format("LORD SAY: %s: %s", ch->GetName(), (const char*)chat);
		}
		break;

	case MSG_CHAT_CHANNEL_LEVEL:
		{
			if( ch->m_level < 50 ) return;		// 50���� ���� ä�� �Ұ�
			tmp.Format("CH_LEVEL SAY: %s: %s", ch->GetName(), (const char*)chat );
		}
		break;
	case MSG_CHAT_CHANNEL_TRADE:
		{
			if( ch->m_level < 10 )				return;		// 10���� ���� ä�� �Ұ�

			LONGLONG needMoney = chat.Length() * 10 ;

			if( ch->m_inventory.getMoney() >= needMoney )
			{
				ch->m_inventory.decreaseMoney(needMoney);
			}
			else
			{
				return;		// ������ �����Ͽ� ä�� �� �� ����
			}
			tmp.Format("CH_TRADE SAY: %s: %s", ch->GetName(), (const char*)chat );
		}
		break;
	default:
		return;
	}

	if (strlen(tmp) > 0 && ch->m_bChatMonitor)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SayMsg(rmsg, MSG_CHAT_NOTICE, 0, "", "", tmp);

		int i;
		for (i = 0; i < 20; i++)
		{
			if (gserver->m_chatmonitor[i])
				SEND_Q(rmsg, gserver->m_chatmonitor[i]);
		}
		GAMELOG << init("CHAT MONITOR", ch)
				<< tmp
				<< end;
	}

#else

	switch (chatType)
	{
	case MSG_CHAT_SAY:			// �Ϲ�
		tmp.Format("SAY: %s: %s", ch->GetName(), (const char*)chat);
		break;

	case MSG_CHAT_SHOUT:		// ��ġ��
		tmp.Format("SHOUT: %s: %s", ch->GetName(), (const char*)chat);
		break;

	case MSG_CHAT_TRADE:		// �Ÿ�
		tmp.Format("TRADE: %s: %s", ch->GetName(), (const char*)chat);
		break;

	case MSG_CHAT_WHISPER:		// �Ӹ�
		tmp.Format("WHISPER: %s ==> %s: %s", ch->GetName(), (const char*)receiver_name, (const char*)chat);
		break;

	case MSG_CHAT_PARTY:		// ��Ƽ
		tmp.Format("PARTYCHAT: %s: %s", ch->GetName(), (const char*)chat);
		break;
	case MSG_CHAT_EXPEDITION:	// ������
		tmp.Format("EXPEDCHAT: %s: %s", ch->GetName(), (const char*)chat);
		break;
	case MSG_CHAT_GUILD:		// ���
		tmp.Format("GUILDCHAT: %s: %s", ch->GetName(), (const char*)chat);
		break;

	case MSG_CHAT_GM:			// GM �Ϲ�ä��
		tmp.Format("GM SAY: %s: %s", ch->GetName(), (const char*)chat);
		break;

	case MSG_CHAT_LORD:			// ���� ����
		tmp.Format("LORD SAY: %s: %s", ch->GetName(), (const char*)chat);
		break;
	case MSG_CHAT_CHANNEL_LEVEL:
		{
			if( ch->m_level < 50 ) return;		// 50���� ���� ä�� �Ұ�
			tmp.Format("CH_LEVEL SAY: %s: %s", ch->GetName(), (const char*)chat );
		}
		break;
	case MSG_CHAT_CHANNEL_TRADE:
		{
			if( ch->m_level < 10 )				return;		// 10���� ���� ä�� �Ұ�

			LONGLONG needMoney = chat.Length() * 10 ;

			if( ch->m_inventory.getMoney() >= needMoney )
			{
				ch->m_inventory.decreaseMoney(needMoney);
			}
			else
			{
				return;		// ������ �����Ͽ� ä�� �� �� ����
			}
			tmp.Format("CH_TRADE SAY: %s: %s", ch->GetName(), (const char*)chat );
		}
		break;
	default:
		return ;
	}

	if (ch->m_bChatMonitor)
	{
		if (strlen(tmp) > 0)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SayMsg(rmsg, MSG_CHAT_NOTICE, 0, "", "", tmp);

			int i;
			for (i = 0; i < 20; i++)
			{
				if (gserver->m_chatmonitor[i])
					SEND_Q(rmsg, gserver->m_chatmonitor[i]);
			}

#ifndef LC_GAMIGO
			GAMELOG << init("CHAT MONITOR", ch)
					<< tmp
					<< end;
#endif
		}
	}
#endif // NO_CHATTING

#ifdef GMTOOL
	if(strlen(tmp) > 0 && ch->m_bGmMonitor)
	{
		switch (chatType)
		{
		case MSG_CHAT_SAY:			// �Ϲ�
		case MSG_CHAT_SHOUT:		// ��ġ��
		case MSG_CHAT_TRADE:		// �Ÿ�
		case MSG_CHAT_WHISPER:		// �Ӹ�
		case MSG_CHAT_GM:			// GM �Ϲ�ä��
		case MSG_CHAT_LORD:			// ���� ����
		case MSG_CHAT_GMTOOL:
			{
				CNetMsg::SP rmsg(new CNetMsg);
				MsgrNoticeGmChatMonitorMsg(rmsg, -1, 1, 1, 0, gserver->m_serverno, gserver->m_subno, -1, tmp, ch->m_index, ch->m_name, chatType);
				SEND_Q(rmsg, gserver->m_messenger);
			}
			break;
		case MSG_CHAT_PARTY:
		case MSG_CHAT_EXPEDITION:
		case MSG_CHAT_GUILD:
			break;
		default:
			return;
		}
	}
#endif // GMTOOL

	CDratanCastle  * pCastle = CDratanCastle::CreateInstance();

	switch (chatType)
	{
	case MSG_CHAT_SAY:			// �Ϲ�
		if (ch->IsInvisible())
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SysMsg(rmsg, MSG_SYS_CANTCHAT_INVISIBLE);
			SEND_Q(rmsg, ch->m_desc);
		}
#ifdef RANKER_NOTICE
		else if( ch->m_bRanker )
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SayMsg(rmsg, MSG_CHAT_RANKER, ch->m_index, ch->GetName(), "", chat);
			ch->m_pArea->SendToCell(rmsg, GET_YLAYER(ch), ch->m_cellX, ch->m_cellZ);
		}
#endif
		else
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SayMsg(rmsg, MSG_CHAT_SAY, ch->m_index, ch->GetName(), "", chat);
			ch->m_pArea->SendToCell(rmsg, GET_YLAYER(ch), ch->m_cellX, ch->m_cellZ);
		}
		break;

	case MSG_CHAT_SHOUT:		// ��ġ��
	case MSG_CHAT_TRADE:		// �Ÿ�
		{
#ifdef CHAT_BLOCK_USA
			if (gserver->m_pulse - ch->m_shoutPulse <= 0)
				return;

			if (ch->m_level < CHAT_BLOCK_USA_LEVEL)
				return;

			ch->m_shoutPulse = gserver->m_pulse + (CHAT_BLOCK_USA_NEXT_TIME * PULSE_REAL_SEC);
#endif
			{
				CNetMsg::SP rmsg(new CNetMsg);
				SayMsg(rmsg, (MSG_CHAT_TYPE)chatType, ch->m_index, ch->GetName(), "", chat);
				ch->m_pArea->SendToCell(rmsg, GET_YLAYER(ch), ch->m_cellX, ch->m_cellZ, CELL_EXT * 2);
			}
		}
		break;

	case MSG_CHAT_WHISPER:		// �Ӹ�
		{
			CNetMsg::SP rmsg(new CNetMsg);
			MsgrWhisperReqMsg(rmsg,
							  0,
							  gserver->m_serverno,
							  gserver->m_subno,
							  ch->m_index,
							  ch->GetName(),
							  receiver_name,
							  chat);
			SEND_Q(rmsg, gserver->m_messenger);
		}
		break;

	case MSG_CHAT_MESSENGER:		// �޽���.
		{
			CNetMsg::SP rmsg(new CNetMsg);
			MsgrFriendChatReqMsg(rmsg,
								 0,
								 gserver->m_serverno,
								 -1,
								 ch->m_index,
								 ch->GetName(),
								 receiver_name,
								 chat);
			SEND_Q(rmsg, gserver->m_messenger);
		}
		break;

	case MSG_CHAT_PARTY:		// ��Ƽ
		{
			if (ch->IsParty())
			{
				CNetMsg::SP rmsg(new CNetMsg);
				HelperPartyChatMsg(rmsg, ch->m_party->GetBossIndex(), ch->m_index, ch->GetName(), chat);
				SEND_Q(rmsg, gserver->m_helper);
			}
		}
		break;

	case MSG_CHAT_EXPEDITION:		// ������
		{
			if (ch->IsExped())
			{
				CNetMsg::SP rmsg(new CNetMsg);
				SayMsg(rmsg, MSG_CHAT_EXPEDITION, ch->m_index, ch->GetName(), "", chat, nGroupTyp);

				if(nGroupTyp == -1)	 //��ü
				{
					ch->m_Exped->SendToAllPC(rmsg,-1);
				}
				else				  //�׷�
				{
					ch->m_Exped->SendToPCInSameGroup(nGroupTyp,rmsg);
				}
			}
		}
		break;

	case MSG_CHAT_GUILD:		// ���
		{
			if (ch->m_guildInfo)
			{
				CNetMsg::SP rmsg(new CNetMsg);
				HelperGuildChat(rmsg,
								ch->m_guildInfo->guild()->index(),
								ch->m_index,
								ch->GetName(),
								chat);

				SEND_Q(rmsg, gserver->m_helper);
			}
		}
		break;

	case MSG_CHAT_GM:			// GM �Ϲ� ä��
		{
			CNetMsg::SP rmsg(new CNetMsg);
			SayMsg(rmsg, (MSG_CHAT_TYPE)chatType, ch->m_index, ch->GetName(), "", chat);
			ch->m_pArea->SendToCell(rmsg, GET_YLAYER(ch), ch->m_cellX, ch->m_cellZ);
		}
		break;

	case MSG_CHAT_LORD:			// ���� ����
		if (CWarCastle::CanLordChat(ch))
		{
			CNetMsg::SP rmsg(new CNetMsg);
			MsgrChatLordMsg(rmsg,
							ch->m_index,
							ch->GetName(),
							chat);

			SEND_Q(rmsg, gserver->m_messenger);
		}

		if(pCastle->CanLordChat(ch) == true)
		{
			CNetMsg::SP rmsg(new CNetMsg);
			MsgrChatLordMsg(rmsg,
							ch->m_index,
							ch->GetName(),
							chat);

			GAMELOG << init("CHATLOAD CHATMSG SEND TO MESSENGER:",ch)
					<< " chat"  << delim <<  chat
					<< end;

			SEND_Q(rmsg, gserver->m_messenger);
		}
		break;

#ifdef GMTOOL
	case MSG_CHAT_GM_WHISPER:	// ������ ���� �Ӹ�
		{
			int server, sub;
			RefMsg(msg) >> server
						>> sub;

			CNetMsg::SP rmsg(new CNetMsg);
			MsgrGMWhisperReqMsg(rmsg,
								0,
								server,
								sub,
								ch->m_index,
								ch->GetName(),
								receiver_name,
								chat);
			SEND_Q(rmsg, gserver->m_messenger);
		}
		break;
	case MSG_CHAT_GMTOOL:		// ������ �������� ���� �Ӹ�
		{
			CNetMsg::SP rmsg(new CNetMsg);
			MsgrGMWhisperReqMsg(rmsg, 0, 1, 1, chat, ch->m_index, ch->m_name, gserver->m_serverno, gserver->m_subno, receiver_name);
			SEND_Q(rmsg, gserver->m_messenger);
		}
		break;
#endif // GMTOOL

	case MSG_CHAT_CHANNEL_LEVEL:
	case MSG_CHAT_CHANNEL_TRADE:
		{
			CNetMsg::SP rmsg(new CNetMsg);
			MsgrChannelChatReqMsg( rmsg, gserver->m_serverno, ch->m_index,  ch->GetName(), (const char*) chat, chatType );
			SEND_Q(rmsg, gserver->m_messenger);
		}
		break;

	default:
		return ;
	}
}
