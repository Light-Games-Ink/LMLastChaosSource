#include "stdhdrs.h"

#include "../ShareLib/bnf.h"
#include "Server.h"
#include "CmdMsg.h"
#include "HelperInGame.h"

#define RECONNECT_TIMER_FOR_MESSENGER		(10 * 1000)

HelperInGameConnectTimer* HelperInGameConnectTimer::instance()
{
	static HelperInGameConnectTimer __instance;
	return &__instance;
}

void HelperInGameConnectTimer::operate( rnSocketIOService* service )
{
	// �� Ÿ�̸� ����
	bnf::instance()->RemoveSession(service);

	// �޽��� ������ ����
	HelperInGame::instance()->connect();
}

//////////////////////////////////////////////////////////////////////////

void HelperInGame::connect()
{
	// �޽��� ������ ���� �õ�
	host_ = gserver->m_config.Find("Helper Server", "IP");
	port_ = atoi(gserver->m_config.Find("Helper Server", "Port"));
	bnf::instance()->CreateAsyncConnect(host_, port_, this, CONNECT_TYPE_FOR_CNETMSG);
}

void HelperInGame::WriteToOutput( CNetMsg::SP& msg )
{
	if (msg->m_mtype == MSG_UNKNOWN)
		return;

	if (service_ == NULL)
		return;

	service_->deliver(msg);
}

void HelperInGame::operate( rnSocketIOService* service )
{
	CNetMsg::SP msg(service->GetMessage());

	gserver->ProcessHelperMessage(msg);
}

void HelperInGame::onClose( rnSocketIOService* service )
{
	LOG_FATAL("%%%%%%%%%% Disconnected from Helper Server (%s : %d) %%%%%%%%%%\n\n",
			  host_.c_str(), port_);

	bnf::instance()->RemoveSession(service);
	service_ = NULL;

	// ������ Ÿ�̸� ����
	session_handle handle = bnf::instance()->CreateMSecTimer(RECONNECT_TIMER_FOR_MESSENGER,
							(void *)HelperInGameConnectTimer::instance());
}

void HelperInGame::onConnect( rnSocketIOService* service )
{
	service_ = service;

	// ���� �� ���� �޽��� ����
	this->WriteToOutput(gserver->m_helperConnMsg);

	CNetMsg::SP rmsg(new CNetMsg);
	HelperPartyDataInitReqMsg(rmsg);
	SEND_Q(rmsg, gserver->m_helper);

	LOG_INFO("connected to Helper server (%s : %d)", host_.c_str(), port_);
}

void HelperInGame::onConnectFail( rnSocketIOService* service )
{
	LOG_INFO("Can't connected to Helper server (%s : %d)", host_.c_str(), port_);
}

HelperInGame* HelperInGame::instance()
{
	static HelperInGame __instance;
	return &__instance;
}