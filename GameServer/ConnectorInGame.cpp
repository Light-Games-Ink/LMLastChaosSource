#include "stdhdrs.h"

#include "../ShareLib/bnf.h"
#include "Server.h"
#include "ConnectorInGame.h"

#define RECONNECT_TIMER_FOR_MESSENGER		(10 * 1000)

ConnectorInGameConnectTimer* ConnectorInGameConnectTimer::instance()
{
	static ConnectorInGameConnectTimer __instance;
	return &__instance;
}

void ConnectorInGameConnectTimer::operate( rnSocketIOService* service )
{
	// �� Ÿ�̸� ����
	bnf::instance()->RemoveSession(service);

	// �޽��� ������ ����
	ConnectorInGame::instance()->connect();
}

//////////////////////////////////////////////////////////////////////////

void ConnectorInGame::connect()
{
	// �޽��� ������ ���� �õ�
	host_ = gserver->m_config.Find("Connector Server", "IP");
	port_ = atoi(gserver->m_config.Find("Connector Server", "Port"));
	bnf::instance()->CreateAsyncConnect(host_, port_, this, CONNECT_TYPE_FOR_CNETMSG);
}

void ConnectorInGame::WriteToOutput( CNetMsg::SP& msg )
{
	if (msg->m_mtype == MSG_UNKNOWN)
		return;

	if (service_ == NULL)
		return;

	service_->deliver(msg);
}

void ConnectorInGame::operate( rnSocketIOService* service )
{
	CNetMsg::SP msg(service->GetMessage());

	gserver->ProcessBilling(msg);
}

void ConnectorInGame::onClose( rnSocketIOService* service )
{
	LOG_FATAL("%%%%%%%%%% Disconnected from Connector Server (%s : %d) %%%%%%%%%%\n",
			  host_.c_str(), port_);

	bnf::instance()->RemoveSession(service);
	service_ = NULL;

	// ������ Ÿ�̸� ����
	session_handle handle = bnf::instance()->CreateMSecTimer(RECONNECT_TIMER_FOR_MESSENGER,
							(void *)ConnectorInGameConnectTimer::instance());
}

void ConnectorInGame::onConnect( rnSocketIOService* service )
{
	service_ = service;

	// ���� �� ���� �޽��� ����
	this->WriteToOutput(gserver->m_connectorConnMsg);

	LOG_INFO("connected to Connector server (%s : %d)", host_.c_str(), port_);
}

void ConnectorInGame::onConnectFail( rnSocketIOService* service )
{
	LOG_INFO("Can't connected to Connector server (%s : %d)", host_.c_str(), port_);
}

ConnectorInGame* ConnectorInGame::instance()
{
	static ConnectorInGame __instance;
	return &__instance;
}