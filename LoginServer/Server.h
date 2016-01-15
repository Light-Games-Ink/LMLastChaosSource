#ifndef __LOGIN_SERVER_H__
#define __LOGIN_SERVER_H__

#include "../ShareLib/rnsocketiohandler.h"
#include "../ShareLib/rnsocketioservice.h"
#include "ConfigFile.h"
#include "Descriptor.h"
#include "../ShareLib/DBCmd.h"
#include "WaitPlayerList.h"

#if defined (INTERGRATION_SERVER)
#include "IntergrationServer.h"
#endif

typedef struct _tagAutoBlock
{
	char	ip[HOST_LENGTH + 1];
	char	id[MAX_ID_NAME_LENGTH + 1];

	struct _tagAutoBlock* prev;
	struct _tagAutoBlock* next;
} AUTO_BLOCK_INFO;

//////////////////////////////////////////////////////////////////////////
class CServerTimer : public rnSocketIOHandler
{
public:
	CServerTimer() {}
	~CServerTimer() {}

	virtual void operate(rnSocketIOService* service);

	static CServerTimer* instance();
};
//////////////////////////////////////////////////////////////////////////
class CServerTimerPerMinute : public rnSocketIOHandler
{
public:
	CServerTimerPerMinute() {}
	~CServerTimerPerMinute() {}

	void Init();
	virtual void operate(rnSocketIOService* service);

	static CServerTimerPerMinute* instance();
};
//////////////////////////////////////////////////////////////////////////

class CServer : public rnSocketIOHandler
{
public:
	CServer();
	~CServer();

	// ������ ����
	CDescriptor** m_connector;		// ������ ����
	int			m_nConnector;		// ������ ���� ��

	////////////
	// ���� ����
	char*		m_serverpath;		// ��� ���
	CConfigFile	m_config;			// ���� ����
	int			m_subno;			// �α��� ���� ���� ��ȣ
	int			m_clientversionMin;	// Ŭ���̾�Ʈ ����
	int			m_clientversionMax;	// Ŭ���̾�Ʈ ����
	bool		m_bOnlyLocal;		// �ܺ� ���� ��� ����
	unsigned int m_nInternalIPFilter;	// ���� ���ӿ� IP ����
	unsigned int m_nInternalSubnetMask;	// ���� ���ӿ� ����� ����ũ
	int			m_national;			// ����

	int			m_maxplayers;		// ��� ���� �ִ� �÷��̾� ��
	bool		m_bshutdown;			// ����Ʈ

#ifdef IP_BLOCK_AUTO
	/////////////////
	// �ڵ� �� ����
	AUTO_BLOCK_INFO*	m_autoBlockTable;	// �� ���̺�
	int					m_autoBlockPulse;	// �ֱ� ������ �ð�
#endif

	int					m_nRecommendServer;

	//////////
	// ƽ ����
	int			m_pulse;			// ƽ ī����

#if defined (LC_USA)
	int			m_pulseSaveTableCount;
#endif

	//////////
	// DB ����
	MYSQL		m_dbuser;			// ����� ������
	MYSQL		m_dbdata;			// ������
	MYSQL		m_dbAuth;			// 2��4�� ���� ���� DB

#ifdef LOCAL_LOGIN_LOG
	MYSQL		m_dbLocalLog;		// �α��� DB�α�
#endif //LOCAL_LOGIN_LOG

	bool		ConnectDB();
	void		DisconnectDB();

	/////////////////
	// ���� ȯ�� ����
	char* GetServerPath();			// ��� ���
	bool LoadSettingFile();			// ���� ���� �б�
	bool InitGame();				// ���� �ʱ�ȭ
	void CheckInternalIP(const char* strData);			// ���� ���ο��� IP �Ľ�

	////////////
	// ������ ����
	void ProcConnector(CNetMsg::SP& msg);				// ������ �޽��� ó��

	void ProcMessenger(CNetMsg::SP& msg);				// �޽��� �޽��� ó��

#if defined (LC_USA)
	void WriteDBPlayerCount();
	void BlockIPLog( const char* user_ip );
#endif

#ifdef PASSWORD_ENCRYPT_SHA256
	char m_salt[1024];							// salt string
	char m_hash[65];							// hash string
#endif // PASSWORD_ENCRYPT_SHA256

#if defined (INTERGRATION_SERVER)
public:
	CIntergrationServer m_intergrationInfo;
#endif
//#if defined(BCRYPT_USA) && !defined(CIRCLE_WINDOWS)
//	bool m_bBcrypted;
//#endif
private:
	bool m_bHttps_Auth;	// ���̰� https ������ �� �������� ���� ����. newStobm.bin ���Ͽ� HttpsAuth=FALSE�� �Է��ϸ� ���� ������ �����Ѵ�.(QA������ ����)
public:
	bool DoHttpsAuth()
	{
		return m_bHttps_Auth;
	}

public:
	virtual void operate(rnSocketIOService* service);
};

#ifndef __LOGIN_SERVER_CPP__
extern CServer gserver;
#endif

#endif
