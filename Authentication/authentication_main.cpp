#include "../ShareLib/bnf.h"
#include "../ShareLib/logsystem.h"

#include "ClientForClient.h"
#include "ClientForServer.h"

const int listen_port_for_server = 3000;
const int listen_port_for_client = 3001;

int main(int argc, char* argv[], char* envp[])
{
	// �α� �ʱ�ȭ
	LogSystem::setSubstitutedValue("logfile", "authentication");
	LogSystem::configureXml("./log.xml");

	// ���� ������ ���� listen
	if (bnf::instance()->CreateListen("0.0.0.0", listen_port_for_server, 10, ClientForServerListen::instance())
		== SessionBase::INVALID_SESSION_HANDLE)
	{
		puts("Authentication : can't bind listen session(server)");
		return 1;
	}

	// Ŭ���̾�Ʈ�� ���� listen
	if (bnf::instance()->CreateListen("0.0.0.0", listen_port_for_client, 0, ClientForClientListen::instance())
		== SessionBase::INVALID_SESSION_HANDLE)
	{
		puts("Authentication : can't bind listen session(client)");
		return 1;
	}

	bnf::instance()->Run();
}
