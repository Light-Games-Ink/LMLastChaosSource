#include "stdhdrs.h"

#include "Log.h"
#include "Character.h"
#include "Server.h"
#include "CmdMsg.h"
#include "doFunc.h"

#ifdef ENABLE_SUBJOB

void do_ExFuncSubJob( CPC* ch, CNetMsg::SP& msg )
{
	unsigned char subtype;
	RefMsg(msg) >> subtype;

	switch(subtype)
	{
	case MSG_EX_SUBJOB_REGIST:
		do_ExSubJobRegist( ch, msg );
		break;
	}
}

void do_ExSubJobRegist( CPC* ch, CNetMsg::SP& msg )
{
	int subjob = 0;

	RefMsg(msg) >> subjob;

	int			needMinLevel	= 50;
	int			needMaxLevel	= 55;
	int			needFame		= 5;
	int			needSP			= 2000 * 10000 ;
	LONGLONG	needMoney		= 100000000; // 1��

	if( !ch )
		return;

	if( ch->m_level < needMinLevel || ch->m_level > needMaxLevel )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SubJobErrorMsg(rmsg, 1 );			// ������ ���� ����
		SEND_Q( rmsg, ch->m_desc );
		return;
	}

	if( ch->m_skillPoint < needSP )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SubJobErrorMsg(rmsg, 3 );			// ���õ� ���ڶ�
		SEND_Q( rmsg, ch->m_desc );
		return;
	}

	if( ch->m_fame < needFame )
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SubJobErrorMsg(rmsg, 2 );			// ��ġ ���ڶ�
		SEND_Q( rmsg, ch->m_desc );
		return;
	}

	// �� ����
	if (ch->m_inventory.getMoney() < needMoney)
	{
		CNetMsg::SP rmsg(new CNetMsg);
		SubJobErrorMsg(rmsg, 4 );			// ��������
		SEND_Q( rmsg, ch->m_desc );
		return ;
	}
	else
	{
		ch->m_inventory.decreaseMoney(needMoney);
	}

	// ���õ� ����
	ch->m_skillPoint -= needSP ;
	// ��ġ ����
	ch->m_fame -= needFame;

	// ���ε��
	ch->m_jobSub |= subjob ;
	ch->CalcStatus(true);

	// �����޼���
	CNetMsg::SP rmsg(new CNetMsg);
	SubJobMsg(rmsg, MSG_EX_SUBJOB_ERROR_SUCESS );
	RefMsg(rmsg) << subjob;
	SEND_Q( rmsg, ch->m_desc );

	return;
}

#endif // ENABLE_SUBJOB
//