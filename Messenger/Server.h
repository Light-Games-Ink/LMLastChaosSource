#ifndef __ASERVER_H__
#define __ASERVER_H__

#include "../ShareLib/rnsocketiohandler.h"
#include "../ShareLib/rnsocketioservice.h"
#include "ConfigFile.h"
#include "Descriptor.h"
#include "MsgList.h"

#define FILENAME_RECOMMEND		".recommend"

#define FILENAME_GOLDENBALL		".goldenball"

typedef struct _tagGoldenBallData
{
	int		nStatus;
	char	strTeam1[GOLDENBALL_TEAM_NAME_LENGTH + 1];
	char	strTeam2[GOLDENBALL_TEAM_NAME_LENGTH + 1];
	int		nTeam1Score;
	int		nTeam2Score;
	int		nYear;
	int		nMonth;
	int		nDay;
	int		nHour;
	int		nMin;
	time_t	timeEndVote;
} GOLDEN_BALL_DATA;

class CServer : public rnSocketIOHandler
{
public:
	CServer();
	~CServer();

	////////////
	// ���� ����
	CDescriptor* m_desclist;		// ����� ���� ����Ʈ

	////////////////////////////
	// �޽��� ����Ʈ
	CMsgList	m_msgList;
	int			m_seqGen;

	////////////
	// ���� ����
	char*		m_serverpath;		// ��� ���
	CConfigFile	m_config;			// ���� ����

	int			m_maxplayers;		// ��� ���� �ִ� �÷��̾� ��
	bool		m_nameserverisslow; // �� ���� ���ϱ�?

	int			m_nRecommendServer;	// ���� ��õ ����

	GOLDEN_BALL_DATA	m_sGoldenBallData;	// ��纼 ������

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

	/////////////////
	// ��� ó�� ����
	virtual void operate(rnSocketIOService* service);
	void CommandInterpreter(CDescriptor* d, CNetMsg::SP& msg); // �� ó��
	void ProcTimeout(CMsgListNode* msgNode);
};

#ifndef __SEVER_CPP__
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

#endif
