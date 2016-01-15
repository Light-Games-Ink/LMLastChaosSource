#ifndef __ADESCRIPTOR_H__
#define __ADESCRIPTOR_H__

#include "../ShareLib/rnsocketiohandler.h"
#include "../ShareLib/rnsocketioservice.h"
#include "../ShareLib/MemoryPoolBase.h"

enum
{
	CLIENT_TYPE_CLIENT,		// ������
	CLIENT_TYPE_CONNECTOR,	// Connector server ���ӿ�
};

class CDescriptor;

//////////////////////////////////////////////////////////////////////////

class CDescriptorReconnectTimer : public rnSocketIOHandler
{
public:
	explicit CDescriptorReconnectTimer(CDescriptor* desc) : desc_(desc) {}
	~CDescriptorReconnectTimer() {}

	virtual void operate(rnSocketIOService* service);

private:
	CDescriptor*		desc_;
};

//////////////////////////////////////////////////////////////////////////

class CDescriptor : public rnSocketIOHandler
{
public:
	explicit CDescriptor(rnSocketIOService* service, int client_type);
	~CDescriptor();

	rnSocketIOService*		service_;
	int	client_type_;

	////////////
	// ���� ����
	CLCString	m_idname;			// ���� ���̵�
	CLCString	m_passwd;			// ��ȣ

	//////////////////////////////////////////////////////////////////////////
	// ������ ���� (�Ʒ��� ������ Connector server�� ����Ǵ� ���ǿ����� ����)
	int			m_hardcore_flag;	// ���̰��� ����ϴ� hardcore server, 0 - no, 1 - yes
	int			m_serverNo;			// ������ ��ȣ
	int			m_nMaxServer;		// ������ �� �ִ� ���� ���� ��
	int*		m_serverSubNo;		// ���� ��ȣ
	int*		m_playerNum;		// �������� ����� ��
	char**		m_ipAddr;			// ������ �ּ�
	int*		m_portNumber;		// ������ ��Ʈ ��ȣ

	std::string	connect_host_;			// ������ Connector server�� ����
	int			connect_port_;
	CDescriptorReconnectTimer reconnect_timer_;
	//////////////////////////////////////////////////////////////////////////

	////////////
	// ���� ����
	int			m_connected;				// CON_ ��� ���� ���� : ���� ����
	bool		m_bclosed;

	////////////
	// ��ũ ����
	CDescriptor* m_pPrev;					// ����Ʈ ��ũ
	CDescriptor* m_pNext;

	void WriteToOutput(CNetMsg::SP& msg);	// ����

	bool GetLogin(CNetMsg::SP& msg);		// ���� ���̵� ��� �˻�

#ifdef INTERNATIONAL_LOCAL_ACCESS_RESTRICTIONS_SPN
	bool IPBlockSouthAmerica(const char *u_IP);
#endif // INTERNATIONAL_LOCAL_ACCESS_RESTRICTIONS_SPN

public:
	bool isRun()
	{
		return (service_ != NULL);
	}

	virtual void operate(rnSocketIOService* service);
	virtual void onClose(rnSocketIOService* service);

	//////////////////////////////////////////////////////////////////////////
	// �Ʒ��� �Լ��� Connector server�� ����Ǵ� ���ǿ����� ����
	void setConnectInfo(std::string connect_host, int connect_port);
	void Connect();
	virtual void onConnect(rnSocketIOService* service);
	virtual void onConnectFail(rnSocketIOService* service);
};

#endif
