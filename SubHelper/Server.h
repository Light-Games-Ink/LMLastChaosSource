#ifndef __ASERVER_H__
#define __ASERVER_H__

#include "../ShareLib/rnsocketiohandler.h"
#include "../ShareLib/rnsocketioservice.h"
#include "ConfigFile.h"
#include "Descriptor.h"
#include "MsgList.h"
#include "RankingSystem.h"
#include "../ShareLib/DBCmd.h"

struct CopyItem
{
	int a_index;
	int a_type_idx;
	int a_subtype_idx;
	int a_flag;
	int a_level;
	int a_job_flag;
	std::string a_name;
	int a_num0;
	int a_num1;
};

class CServerTimer : public rnSocketIOHandler
{
public:
	CServerTimer() {}
	~CServerTimer() {}

	void operate( rnSocketIOService* service );
	static CServerTimer* instance();
};

class CServerTimerPerMinute : public rnSocketIOHandler
{
public:
	CServerTimerPerMinute() {}
	~CServerTimerPerMinute() {}

	void Init();
	void operate( rnSocketIOService* service );
	static CServerTimerPerMinute* instance();
};

class CServer : public rnSocketIOHandler
{
public:
	CServer();
	~CServer();

	////////////
	// ���� ����
	CDescriptor* m_desclist;		// ����� ���� ����Ʈ
	int			m_nDesc;			// ����Ʈ�� ���� ����

	////////////////////////////
	// �޽��� ����Ʈ
	CMsgList	m_msgList;
	int			m_seqGen;

	////////////
	// ���� ����
	char*		m_serverpath;		// ��� ���
	CConfigFile	m_config;			// ���� ����
	MYSQL		m_dbchar;
	int			m_serverno;

	int			m_maxplayers;		// ��� ���� �ִ� �÷��̾� ��
	bool		m_bshutdown;		// ���� ����

	virtual void operate(rnSocketIOService* service);

	///////////////////
	// �޽��� ��ȣ ����
	int GetNextSeqNo()
	{
		return m_seqGen++;
	}

	/////////////////
	// ���� ȯ�� ����
	char* GetServerPath();			// ��� ���
	bool LoadSettingFile();			// ���� ���� �б�
	bool InitGame();				// ���� �ʱ�ȭ
	int GetMaxPlayers();			// ��� ���� �ִ� �÷��̾� �� ���ϱ�

	bool ConnectDB();
	void DisconnectDB();
	bool LoadSettings();

	///////////////////////
	// Ŭ���̾�Ʈ ���� ����
	void SendToAllGameServer(CNetMsg::SP& msg);

	/////////////////
	// ��� ó�� ����
	void CommandInterpreter(CDescriptor* d, CNetMsg::SP& msg); // �� ó��

	void HeartBeat();

	CRankingSystemEX m_RankingSystem;

	MYSQL		m_dbData;
	std::list<int> m_tradeAgentBuyList;
	float		m_tradeAgentFee;			//�ŷ����� ������
	int			m_tradeAgentDeposit;			//�ŷ����� ������

	bool CopyToTradeAgentItemInfo();
};

#ifndef __SERVER_CPP__
extern CServer gserver;
#endif

//////////////////////////////////////////////////////////////////////////

class ClearMessageListTimer : public rnSocketIOHandler
{
public:
	ClearMessageListTimer() {}
	~ClearMessageListTimer() {}

	virtual void operate(rnSocketIOService* service);

	static ClearMessageListTimer* instance();
};

//////////////////////////////////////////////////////////////////////////
// �� ��
class CServerTimerPerHour : public rnSocketIOHandler
{
public:
	CServerTimerPerHour() {}
	~CServerTimerPerHour() {}

	void Init();
	virtual void operate(rnSocketIOService* service);

	static CServerTimerPerHour* instance();
};

#endif
//
