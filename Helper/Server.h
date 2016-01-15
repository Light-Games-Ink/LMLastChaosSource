#ifndef __ASERVER_H__
#define __ASERVER_H__

#include <map>
#include "../ShareLib/rnsocketiohandler.h"
#include "../ShareLib/rnsocketioservice.h"

#define MAKE_LONGLONG_KEY(x, y)			(((LONGLONG)(x) << 32) | (y))

#ifdef DEV_GUILD_STASH
//#include <map>
#endif // DEV_GUILD_STASH
#include "Party.h"
#include "ConfigFile.h"
#include "Descriptor.h"
#include "MsgList.h"
#include "Guild.h"
#include "Expedition.h"
#include "Syndicate.h"
#include "../ShareLib/DBCmd.h"

#ifdef DEV_GUILD_STASH
#include "GuildStash.h"
#endif // DEV_GUILD_STASH

class timerPerSecond : public rnSocketIOHandler
{
public:
	timerPerSecond() {}
	~timerPerSecond() {}

	virtual void operate(rnSocketIOService* service);
	static timerPerSecond* instance();
};

class timerPerMinute : public rnSocketIOHandler
{
public:
	timerPerMinute() {}
	~timerPerMinute() {}

	void Init();
	virtual void operate(rnSocketIOService* service);
	static timerPerMinute* instance();
};

class timerPerHour : public rnSocketIOHandler
{
public:
	timerPerHour() {}
	~timerPerHour() {}

	void Init();
	virtual void operate(rnSocketIOService* service);
	static timerPerHour* instance();
};

typedef std::map<LONGLONG, CPartyMatchMember*> map_partymatchmember_t;
typedef std::map<LONGLONG, CPartyMatchParty*> map_partymatchparty_t;
typedef std::map<LONGLONG, CExpedition*> map_expedition_t;
typedef std::map<LONGLONG, CParty*> map_party_t;

class timerPerMidnight : public rnSocketIOHandler
{
public:
	timerPerMidnight() {}
	~timerPerMidnight() {}

	void Init();
	virtual void operate(rnSocketIOService* service);
	static timerPerMidnight* instance();
};

class CServer : public rnSocketIOHandler
{
public:
	CServer();
	~CServer();

#ifdef PREMIUM_CHAR
	int			m_premiumchar_reset_jump_count_time;		// unix time
#endif

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

	unsigned int m_pulse;			// ƽ ī����

	CGuildList	m_guildlist;

	int			m_nMoonStoneNas;
	int			m_nGuildMarkTable[3];
	///////////////////
	// ��ü ��Ƽ ����Ʈ
	map_party_t					m_listParty;	// ������ ��ü ��Ƽ ����Ʈ

	// ��ü ������ ����Ʈ
	map_expedition_t			m_listExped;	// ������ ��ü ������ ����Ʈ

	map_partymatchmember_t		m_listPartyMatchMember;
	map_partymatchparty_t		m_listPartyMatchParty;

#ifdef DEV_GUILD_STASH
	std::map<int,CGuildStash*>		m_guildstash;
#endif // DEV_GUILD_STASH

	Syndicate					m_syndicate;

	//���� �׿��� ���� ���� ����
	int							m_castle_env_rate;
	int							m_castle_mob_rate;
	int							m_castle_tax_rate;
	int							m_castle_hunt_rate;

	///////////////////
	// �޽��� ��ȣ ����
	int GetNextSeqNo()
	{
		return m_seqGen++;
	}

	virtual void operate(rnSocketIOService* service);

	/////////////////
	// ���� ȯ�� ����
	char* GetServerPath();			// ��� ���
	bool LoadSettingFile();			// ���� ���� �б�
	bool InitGame();				// ���� �ʱ�ȭ
	void initTime();				// Server::Run()���� �ʱ�ȭ �ϴ� �κ�
	int GetMaxPlayers();			// ��� ���� �ִ� �÷��̾� �� ���ϱ�
	void SetServerTime();			// ���� �ð� ���� -> serverTime

	/////////////////
	// ���� ���� ����
	bool ConnectDB();
	void DisconnectDB();
	bool LoadSettings();

	void SendToAllGameServer(CNetMsg* msg)
	{
		SendToAllGameServer(CNetMsg::SP(msg));
	}
	void SendToAllGameServer(CNetMsg::SP msg);
	void SendToAllSubServer(CNetMsg* msg, int nSubNo)
	{
		SendToAllSubServer(CNetMsg::SP(msg), nSubNo);
	}
	void SendToAllSubServer(CNetMsg::SP msg, int nSubNo);

	/////////////////
	// ��� ó�� ����
	void CommandInterpreter(CDescriptor* d, CNetMsg::SP& msg); // �� ó��

	void HeartBeat();

	bool DeleteGuild(int guildindex);
	bool DeleteGuildMember(int guildindex, const char* guildname, int charindex, const char* charname, bool bKick);

	// ��Ƽ ����Ʈ ��� ã�� : ���� �ε��� ���
	CParty* FindPartyByBossIndex(int nSubNo, int nBossIndex);

	// ��Ƽ ����Ʈ ��� ã�� : ��� �ε��� ���
	CParty* FindPartyByMemberIndex(int nSubNo, int nMemberIndex, bool bIncludeRequest);

	// ��Ƽ ����Ʈ ��� ã�� : ���� �ε��� ���
	CExpedition* FindExpedByBossIndex(int nSubNo, int nBossIndex);

	// ��Ƽ ����Ʈ ��� ã�� : ��� �ε��� ���
	CExpedition* FindExpedByMemberIndex(int nSubNo, int nMemberIndex, bool bIncludeRequest);

	CPartyMatchMember* FindPartyMatchMemberByCharIndex(int nSubNo, int nCharIndex);
	CPartyMatchParty* FindPartyMatchPartyByBossIndex(int nSubNo, int nBossIndex);

	void SendGuildMemberList(CGuild* guild, CDescriptor* desc);
	void SendExtendGuildMemberList(CGuild* guild, CDescriptor* desc);

#ifdef PARTY_BUG_GER
	void PrintPartyMemberList(int nSubNo, int nFindCharIdx);
#endif // PARTY_BUG_GER

#ifdef EXTREME_CUBE_VER2
	int m_CubePointUpdateTime;
	void CubePointRanking(int RankingTime);
	void CheckCubeReward();
	int SetCurCubePointUpdateTime();
#endif // EXTREME_CUBE_VER2
#ifdef DEV_GUILD_MARK
#endif // DEV_GUILD_MARK
	void checkGuildProcess(time_t now);			// ��ü ��带 ���鼭 üũ�ؾ� �ϴ� �͵��� ��Ƶ� �Լ� : ���� �ѹ����鼭 �� üũ�ϵ���
	void checkGuildBattle(time_t, CGuild* p);	// ����� ���� üũ
#ifdef DEV_GUILD_MARK
	void checkGuildMarkTime(time_t now, CGuild* p);			// ��� ��ũ ���� üũ
#endif // DEV_GUILD_MARK
	void checkGuildMasterAutoKick(time_t now, CGuild* p);	// ��� ������ �ڵ� �߹� ���� üũ

	void saveCastleDungeonInfo();
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

#endif
//
