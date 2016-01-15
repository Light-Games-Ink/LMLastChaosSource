#include "stdhdrs.h"

#include "../ShareLib/bnf.h"
#include "Server.h"
#include "SubHelperInGame.h"

#define RECONNECT_TIMER_FOR_MESSENGER		(10 * 1000)

SubHelperInGameConnectTimer* SubHelperInGameConnectTimer::instance()
{
	static SubHelperInGameConnectTimer __instance;
	return &__instance;
}

void SubHelperInGameConnectTimer::operate( rnSocketIOService* service )
{
	// �� Ÿ�̸� ����
	bnf::instance()->RemoveSession(service);

	// �޽��� ������ ����
	SubHelperInGame::instance()->connect();
}

//////////////////////////////////////////////////////////////////////////

void SubHelperInGame::connect()
{
	// �޽��� ������ ���� �õ�
	host_ = gserver->m_config.Find("SubHelper Server", "IP");
	port_ = atoi(gserver->m_config.Find("SubHelper Server", "Port"));
	bnf::instance()->CreateAsyncConnect(host_, port_, this, CONNECT_TYPE_FOR_CNETMSG);
}

void SubHelperInGame::WriteToOutput( CNetMsg::SP& msg )
{
	if (msg->m_mtype == MSG_UNKNOWN)
		return;

	if (service_ == NULL)
		return;

	service_->deliver(msg);
}

void SubHelperInGame::operate( rnSocketIOService* service )
{
	CNetMsg::SP msg(service->GetMessage());

	gserver->ProcessSubHelperMessage(msg);
}

void SubHelperInGame::onClose( rnSocketIOService* service )
{
	LOG_FATAL("%%%%%%%%%% Disconnected from SubHelper Server (%s : %d) %%%%%%%%%%\n\n",
			  host_.c_str(), port_);

	bnf::instance()->RemoveSession(service);
	service_ = NULL;

	// ������ Ÿ�̸� ����
	session_handle handle = bnf::instance()->CreateMSecTimer(RECONNECT_TIMER_FOR_MESSENGER,
							(void *)SubHelperInGameConnectTimer::instance());
}

void SubHelperInGame::onConnect( rnSocketIOService* service )
{
	service_ = service;

	// ���� �� ���� �޽��� ����
	this->WriteToOutput(gserver->m_subHelperConnMsg);

	LOG_INFO("connected to SubHelper server (%s : %d)", host_.c_str(), port_);
}

void SubHelperInGame::onConnectFail( rnSocketIOService* service )
{
	LOG_INFO("Can't connected to SubHelper server (%s : %d)", host_.c_str(), port_);
}

SubHelperInGame* SubHelperInGame::instance()
{
	static SubHelperInGame __instance;
	return &__instance;
}