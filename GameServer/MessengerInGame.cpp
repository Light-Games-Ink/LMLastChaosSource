#include "stdhdrs.h"

#include "../ShareLib/bnf.h"
#include "Server.h"
#include "MessengerInGame.h"

#define RECONNECT_TIMER_FOR_MESSENGER		(10 * 1000)

MessengerInGameConnectTimer* MessengerInGameConnectTimer::instance()
{
	static MessengerInGameConnectTimer __instance;
	return &__instance;
}

void MessengerInGameConnectTimer::operate( rnSocketIOService* service )
{
	// �� Ÿ�̸� ����
	bnf::instance()->RemoveSession(service);

	// �޽��� ������ ����
	MessengerInGame::instance()->connect();
}

//////////////////////////////////////////////////////////////////////////

void MessengerInGame::connect()
{
	// �޽��� ������ ���� �õ�
	host_ = gserver->m_config.Find("Messenger Server", "IP");
	port_ = atoi(gserver->m_config.Find("Messenger Server", "Port"));
	bnf::instance()->CreateAsyncConnect(host_, port_, this, CONNECT_TYPE_FOR_CNETMSG);
}

void MessengerInGame::WriteToOutput( CNetMsg::SP& msg )
{
	if (msg->m_mtype == MSG_UNKNOWN)
		return;

	if (service_ == NULL)
		return;

	service_->deliver(msg);
}

void MessengerInGame::operate( rnSocketIOService* service )
{
	CNetMsg::SP msg(service->GetMessage());

	gserver->ProcessMessengerMessage(msg);
}

void MessengerInGame::onClose( rnSocketIOService* service )
{
	bnf::instance()->RemoveSession(service);
	service_ = NULL;

	// ������ Ÿ�̸� ����
	session_handle handle = bnf::instance()->CreateMSecTimer(RECONNECT_TIMER_FOR_MESSENGER,
							(void *)MessengerInGameConnectTimer::instance());

	LOG_FATAL("%%%%%%%%%% Disconnected from Messenger Server (%s : %d) %%%%%%%%%%\n\n",
			  host_.c_str(), port_);
}

void MessengerInGame::onConnect( rnSocketIOService* service )
{
	service_ = service;

	// ���� �� ���� �޽��� ����
	this->WriteToOutput(gserver->m_messengerConnMsg);

	LOG_INFO("connected to Messenger server (%s : %d)", host_.c_str(), port_);
}

void MessengerInGame::onConnectFail( rnSocketIOService* service )
{
	LOG_INFO("Can't connect to Messenger server (%s : %d)", host_.c_str(), port_);
}

MessengerInGame* MessengerInGame::instance()
{
	static MessengerInGame __instance;
	return &__instance;
}