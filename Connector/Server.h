#ifndef __ASERVER_H__
#define __ASERVER_H__

#include <map>
#include "../ShareLib/rnsocketiohandler.h"
#include "../ShareLib/rnsocketioservice.h"
#include "ConfigFile.h"
#include "Descriptor.h"
#include "User.h"
#include "BillingClient.h"
#include "WaitPlayerList.h"

/*
typedef struct __tagLimitCtSell
{
	int			ct_id;
	int			sell;
} LIMIT_CT_CELL;
*/

class CLimitSell
{
	int m_ctid;
	int m_sell;
	bool m_bChange;

public:
	CLimitSell(int ctid, int sell, bool change = true)
	{
		m_ctid = ctid;
		m_sell = sell;
		m_bChange = change;
	}

	int GetIndex()
	{
		return m_ctid;
	}
	int GetSell()
	{
		return m_sell;
	}
	bool IsChange()
	{
		if( m_bChange )
		{
			m_bChange = false;
			return true;
		}
		return false;
	}
	void IncreaseSell()
	{
		m_sell++;
		m_bChange = true;
	}
	void moreSell(int sell)
	{
		m_sell = m_sell - sell;
		if( m_sell < 0 ) m_sell = 0;
	}

	static int CompByIndex(CLimitSell* p1, CLimitSell* p2)
	{
		return p1->GetIndex() - p2->GetIndex();
	}
};

class ServerAliveTimer : public rnSocketIOHandler
{
public:
	ServerAliveTimer() {}
	~ServerAliveTimer() {}

	virtual void operate(rnSocketIOService* service);
	static ServerAliveTimer* instance();
};

class HeartBeatTimer : public rnSocketIOHandler
{
public:
	HeartBeatTimer() {}
	~HeartBeatTimer() {}

	virtual void operate(rnSocketIOService* service);
	static HeartBeatTimer* instance();
};

class CServer : public rnSocketIOHandler
{
public:
	CServer();
	~CServer();

	////////////
	// ���� ����
	CDescriptor* m_desclist;		// ����� ���� ����Ʈ

	////////////
	// ���� ����
	int			m_hardcore_flag_in_connector;	// ���̰��� ����ϴ� hardcore server, 0 - no, 1 - yes
	char*		m_serverpath;		// ��� ���
	CConfigFile	m_config;			// ���� ����
	CBillingClient m_billing;		// ���� Ŭ���̾�Ʈ
	int			m_serverno;			// ���� ��ȣ
	int			m_maxSubServer;		// ���� ���� �ִ� ����
	int			m_maxplayers;		// ��� ���� �ִ� �÷��̾� ��
	bool		m_bshutdown;		// ���� ���� ���� ����

	typedef std::map<int, CLimitSell*> map_limitsell_t;
	map_limitsell_t m_limitSell;
	bool		m_changeLimit;		// �����ǸŸ� 5�и��� ��� �����Ұ��ΰ�.

	CWaitPlayerList	m_waitplayer;

	int			m_nEventGomdori2007LastUpdate;		// ������ ������Ʈ �ð� : ���� ��
	int			m_nEventGomdori2007LastRead;		// ������ ���� �ð� : ���� ��
	int			m_nEventGomdori2007Count;			// ����Ʈ ��
	int*		m_nEventGomdori2007Status;			// ������
	int			m_nEventGomdori2007StatusCurServer;	// ���� �������� ���� ��

	bool			m_bEventXmasTimerStart;				// Ÿ�̸� �۵� ����
	int				m_nEventXmasTreeOldTime;			// �ð� ���̸� ���ϱ� ���� ����
	int				m_nEventXmasTreeNoticeTime;			// Ʈ�� ������ ��� ���� �ð� : ���� ��
	int				m_nEventXmasTreeLastUpdate;			// ������ ������Ʈ �ð� : ���� ��
	int				m_nEventXmasTreeLastRead;			// ������ ���� �ð� : ���� ��
	unsigned int	m_unEventXmasTreePoint;				// ���� ������ ���� ����Ʈ

	/////////////////
	// ��� �ֱ� ����
	time_t		m_pulseSaveCount;	// t_connect_count ���� �ð�
	time_t		m_pulseSaveMax;		// t_connect_max ���� �ð�

	//////////
	// DB ����
	MYSQL		m_dbuser;			// ����� ������
	MYSQL		m_dbauth;			// ����� ���� ������

	//////////////
	// User ����Ʈ
	CUserList*	m_userList;			// ���� ����Ʈ

	virtual void operate(rnSocketIOService* service);

	/////////////////
	// ���� ȯ�� ����
	char* GetServerPath();			// ��� ���
	bool LoadSettingFile();			// ���� ���� �б�
	bool ConnectDB();				// DB ����
	bool InitGame();				// ���� �ʱ�ȭ
	int GetMaxPlayers();			// ��� ���� �ִ� �÷��̾� �� ���ϱ�

	void Close();										// ���� ����
	void HeartBeat();									// 1�ʿ� 10���� �Ź� ƽ���� ����Ǵ� �Լ�
	void SavePlayersCount();							// �÷��̾� ���� �����ͺ��̽��� ����

	void CloseSocket(CDescriptor* d);			// ��ũ���� ����
	void SendAllGameServer(CNetMsg::SP& msg);

	///////////////////////
	// ���Ӽ��� ���� �ֱ��� ���
	void TimerToGameServer();

	/////////////////
	// ��� ó�� ����
	void CommandInterpreter(CDescriptor* d, CNetMsg::SP& msg); // �� ó��

	void ProcessLogout(CUser* user);					// �α׾ƿ� ó��
	void InitUserData();

#ifdef PASSWORD_ENCRYPT_SHA256
	char m_salt[1024];							// salt string
	char m_hash[65];							// hash string
#endif // PASSWORD_ENCRYPT_SHA256
//#if defined(BCRYPT_USA) && !defined(CIRCLE_WINDOWS)
//	bool m_bBcrypted;
//#endif
};

#ifndef __SERVER_CPP__
extern CServer gserver;
#endif

#endif
//
