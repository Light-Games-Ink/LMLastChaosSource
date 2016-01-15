#ifndef __ASERVER_H__
#define __ASERVER_H__

#include <set>
#include <map>
#include "../ShareLib/rnsocketiohandler.h"
#include "../ShareLib/rnsocketioservice.h"
#include "../ShareLib/DBCmd.h"
#include "ConfigFile.h"
#include "Zone.h"
#include "Item.h"
#include "JewelData.h"
#include "Skill.h"
#include "Quest.h"
#include "Option.h"
#include "SSkill.h"
#include "Guild.h"
#include "Friend.h"
#include "RareOptionProto.h"
#include "MonsterCombo.h"
#include "ExtremeCube.h"
#include "PacketTime.h"
#include "LuckyDrawBox.h"

#include "MessengerInGame.h"
#include "ConnectorInGame.h"
#include "HelperInGame.h"
#include "SubHelperInGame.h"

#include "PCList.h"

#include "RewardEvnetItemManager.h"

#include "TitleSystem.h"

#ifdef LACARETTE_SYSTEM
#include "LacaRette.h"
#endif

#include "Expedition.h"

#include "SetItemProto.h"

#include "TrigerSet.h"

#include "Factory.h"

#include "Affinity.h"

#include "RaidData.h"

#ifdef DEV_EVENT_PROMOTION2
#include "Promotion2Event.h"
#endif

#include "MonsterMercenary.h"

#ifdef XTRAP
#ifdef XTRAP_DUMP
#include	"HexaDump4XTrap.h"
#endif //XTRAP_DUMP
#include	"Xtrap_S_Interface.h"
#define		CSFILE_NUM				2			// ����� ������ ����
#define		XTRAP_CS4_BUFSIZE_MAP	13000		// ������ ������ ����
#endif //XTRAP

#include "WarGround.h"

#include <set>

#include "Reward.h"

#include "EventAutomation.h"
#include "DropItem.h"

#if defined (INTERGRATION_SERVER)
#include "IntergrationServer.h"
#endif

#include "RockPaperScissorsGame.h"
#include "PuzzleGame.h"

#include "Fortune_system.h"

#include "SyndicateInfoDataManager.h"
#include "GPSDataManager.h"

#define		RESET_RAID_SAVEFILE ".setraidinittime"

#define MAKE_LONGLONG_KEY(x, y)			(((LONGLONG)(x) << 32) | (y))

#ifdef DEV_EVENT_AUTO
class CKingsBirthdayDropItem
{
public:
	CKingsBirthdayDropItem()
		: m_nDropItemIndex(0), m_nDropCount(0), m_nDropProb(0), m_nPoint(0), m_strType()
	{}
	~CKingsBirthdayDropItem() {}
	void setDropItemIndex(int val)
	{
		m_nDropItemIndex = val;
	}
	void setDropCount(int val)
	{
		m_nDropCount = val;
	}
	void setDropProb(int val)
	{
		m_nDropProb = val;
	}
	void setPoint(int val)
	{
		m_nPoint = val;
	}
	void setType(const char* str)
	{
		m_strType.Format(str);
	}
	int getDropItemIndex()
	{
		return m_nDropItemIndex;
	}
	int getDropItemCount()
	{
		return m_nDropCount;
	}
	int getDropItemProb()
	{
		return m_nDropProb;
	}
	int getPoint()
	{
		return m_nPoint;
	}
	const char* getType()
	{
		return m_strType;
	}
private:
	int m_nDropItemIndex;
	int m_nDropCount;
	int m_nDropProb;
	int m_nPoint;
	CLCString m_strType;
};

class CEventKingsBirthdayRewardItem
{
public:
	CEventKingsBirthdayRewardItem()
		: m_nItemIndex(0)
	{}
	~CEventKingsBirthdayRewardItem() {}
	void setItemIndex(int val)
	{
		m_nItemIndex = val;
	}
	int getItemIndex()
	{
		return m_nItemIndex;
	}
	void setRewardItem(CConfigFile& eventConfig)
	{
		setItemIndex(atoi(eventConfig.Find("TLD_KB_HEART_REWARD", "HeartRewardIndex")));
	}
	void giveRewardItem(CPC* pc, int giveCount);
private:
	int m_nItemIndex;
};

class CEventKingsBirthdayFlagReward
{
public:
	CEventKingsBirthdayFlagReward()
		: m_nCount(0)
	{}
	~CEventKingsBirthdayFlagReward()
	{
		//if(m_flagReward)
		//{
		//	delete[] m_flagReward;
		//}
		if(m_flagReward.size())
		{
			std::vector<CEventKingsBirthdayRewardItem*>::iterator it;
			std::vector<CEventKingsBirthdayRewardItem*>::iterator itEnd = m_flagReward.end();
			for(it = m_flagReward.begin(); it != itEnd; it++)
			{
				if(*it)
				{
					delete *it;
					*it = NULL;
				}
			}
			m_flagReward.clear();
		}
	}
	void setCount(int val)
	{
		m_nCount = val;
	}
	int getCount()
	{
		return m_nCount;
	}
	void setFlagReward(CConfigFile& eventConfig);
	CEventKingsBirthdayRewardItem* getFlagRewardItem(int job);
	void giveFlagRewardItem(CPC* pc);
private:
	int m_nCount;
	//CEventKingsBirthdayRewardItem* m_flagReward;
	std::vector<CEventKingsBirthdayRewardItem*> m_flagReward;
};

class CEventKingsBirthdayDropItemManager
{
public:
	CEventKingsBirthdayDropItemManager()
		: m_nDropItemCount(0)
	{}
	~CEventKingsBirthdayDropItemManager()
	{
		//if(m_DropItem)
		//{
		//	delete[] m_DropItem;
		//}
		if(m_DropItem.size())
		{
			std::vector<CKingsBirthdayDropItem*>::iterator it;
			std::vector<CKingsBirthdayDropItem*>::iterator itEnd = m_DropItem.end();
			for(it = m_DropItem.begin(); it != itEnd; it++)
			{
				if(*it)
				{
					delete *it;
					*it = NULL;
				}
			}
			m_DropItem.clear();
		}
	}
	void setDropItem(CConfigFile& eventConfig);
	int getDropItemCount()
	{
		return m_nDropItemCount;
	}
	void setDropItemCount(int val)
	{
		m_nDropItemCount = val;
	}
	CKingsBirthdayDropItem* getDropItem(int arrayIndex);
	CItem* getItem(int prob);
	int calcPoint(CPC* pc);
	void deleteDropItem(CPC* pc);
	void deleteFlagItem(CPC* pc);
private:
	int m_nDropItemCount;
	//CKingsBirthdayDropItem* m_DropItem;
	std::vector<CKingsBirthdayDropItem*> m_DropItem;
};

class CLevelSection
{
public:
	CLevelSection()
		: m_nMinLevel(0), m_nMaxLevel(0), m_nProb(0)
	{}
	~CLevelSection() {}
	void setMinLevel(int val)
	{
		m_nMinLevel = val;
	}
	void setMaxLevel(int val)
	{
		m_nMaxLevel = val;
	}
	void setProb(int val)
	{
		m_nProb = val;
	}
	int getMinLevel()
	{
		return m_nMinLevel;
	}
	int getMaxLevel()
	{
		return m_nMaxLevel;
	}
	int getProb()
	{
		return m_nProb;
	}
private:
	int m_nMinLevel;
	int m_nMaxLevel;
	int m_nProb;
};

class CEventKingsBirthdayDropProbTable
{
public:
	CEventKingsBirthdayDropProbTable()
		: m_nCount(0), m_firstPriority(0), m_secondPriority(0)
	{}
	~CEventKingsBirthdayDropProbTable()
	{
		//if(m_levelSection)
		//delete[] m_levelSection;

		if(m_levelSection.size())
		{
			std::vector<CLevelSection*>::iterator it;
			std::vector<CLevelSection*>::iterator itEnd = m_levelSection.end();
			for(it = m_levelSection.begin(); it != itEnd; it++)
			{
				if(*it)
				{
					delete *it;
					*it = NULL;
				}
			}
			m_levelSection.clear();
		}
	}
	void setProbTable(CConfigFile& eventConfig);
	int getProb(CNPC* npc);
	void setCount(int val)
	{
		m_nCount = val;
	}
	void setFirstPriority(int val)
	{
		m_firstPriority = val;
	}
	void setSecondPriority(int val)
	{
		m_secondPriority = val;
	}
	int getFirstPriority()
	{
		return m_firstPriority;
	}
	int getSecondPriority()
	{
		return m_secondPriority;
	}
	int getCount()
	{
		return m_nCount;
	}

private:
	int m_nCount;
	//CLevelSection* m_levelSection;
	std::vector<CLevelSection*> m_levelSection;
	int m_firstPriority;
	int m_secondPriority;
};

class CEventKingsBirthday
{
public:
	CEventKingsBirthday()
		:m_bEventStart(false), m_bEventItemDelete(false)
	{}
	~CEventKingsBirthday() {}
	void setEventStart(bool b)
	{
		m_bEventStart = b;
	}
	void setEventDeleteItem(bool b)
	{
		m_bEventItemDelete = b;
	}
	bool getEventStart()
	{
		return m_bEventStart;
	}
	bool getEventDeleteItem()
	{
		return m_bEventItemDelete;
	}
	void LoadDropItemManager(CConfigFile& eventConfig)
	{
		m_DropItemManager.setDropItem(eventConfig);
	}
	void LoadHeartItem(CConfigFile& eventConfig)
	{
		m_HeartRewardItem.setRewardItem(eventConfig);
	}
	void LoadFlagItem(CConfigFile& eventConfig)
	{
		m_FlagRewardItem.setFlagReward(eventConfig);
	}
	void LoadItemDropProbTable(CConfigFile& eventConfig)
	{
		m_ItemDropProbTable.setProbTable(eventConfig);
	}
	void Load(CConfigFile& eventConfig);
	CEventKingsBirthdayDropItemManager&		getDropItemManager()
	{
		return m_DropItemManager;
	}
	CEventKingsBirthdayRewardItem&			getHeartRewardItem()
	{
		return m_HeartRewardItem;
	}
	CEventKingsBirthdayFlagReward&			getFlagRewardItem()
	{
		return m_FlagRewardItem;
	}
	CEventKingsBirthdayDropProbTable&		getItemProbTable()
	{
		return m_ItemDropProbTable;
	}
private:
	bool m_bEventStart;
	bool m_bEventItemDelete;
	CEventKingsBirthdayDropItemManager	m_DropItemManager;
	CEventKingsBirthdayRewardItem		m_HeartRewardItem;
	CEventKingsBirthdayFlagReward		m_FlagRewardItem;
	CEventKingsBirthdayDropProbTable	m_ItemDropProbTable;
};
class CZoneDropProb
{
public:
	CZoneDropProb()
		: m_nZoneIndex(-1), m_nMultiple(1)
	{}
	~CZoneDropProb() {}
	void setZoneIndex(int val)
	{
		m_nZoneIndex = val;
	}
	void setMultiple(int val)
	{
		m_nMultiple = val;
	}
	int getZoneIndex()
	{
		return m_nZoneIndex;
	}
	int getMultiple()
	{
		return m_nMultiple;
	}
private:
	int m_nZoneIndex;
	int m_nMultiple;
};
class CDropProbTable
{
public:
	CDropProbTable()
		: m_nLevelSectionCount(0), m_nZoneCount(0), m_underLevel(0), m_division(1), m_upperLevel(0), m_multiple(1)
	{}
	~CDropProbTable()
	{
		if(m_levelSection.size())
		{
			std::vector<CLevelSection*>::iterator lit;
			std::vector<CLevelSection*>::iterator litEnd = m_levelSection.end();
			for(lit = m_levelSection.begin(); lit != litEnd; lit++)
			{
				delete *lit;
				*lit = NULL;
			}
			m_levelSection.clear();
		}
		if(m_zoneDropProb.size())
		{
			std::vector<CZoneDropProb*>::iterator zit;
			std::vector<CZoneDropProb*>::iterator zitEnd = m_zoneDropProb.end();
			for(zit = m_zoneDropProb.begin(); zit != zitEnd; zit++)
			{
				delete *zit;
				*zit = NULL;
			}
			m_zoneDropProb.clear();
		}
	}
	void Load(CConfigFile& eventConfig);
	void setLevelSectionCount(int val)
	{
		m_nLevelSectionCount = val;
	}
	int getLevelSectionCount()
	{
		return m_nLevelSectionCount;
	}
	void setZoneCount(int val)
	{
		m_nZoneCount = val;
	}
	int getZoneCount()
	{
		return m_nZoneCount;
	}
	//void setFirstPriority(int val){m_firstPriority = val;}
	//int getFirstPriority(){return m_firstPriority;}
	//void setSecondPriority(int val){m_secondPriority = val;}
	//int getSecondPriority(){return m_secondPriority;}
	void setUnderLevel(int val)
	{
		m_underLevel = val;
	}
	void setUpperLevel(int val)
	{
		m_upperLevel = val;
	}
	void setDivision(int val)
	{
		m_division = val;
	}
	void setMultiple(int val)
	{
		m_multiple = val;
	}
	int getUnderLevel()
	{
		return m_underLevel;
	}
	int getUpperLevel()
	{
		return m_upperLevel;
	}
	int getDivision()
	{
		return m_division;
	}
	int getMultiple()
	{
		return m_multiple;
	}
	int getProb(CPC*pc, CNPC* npc);
private:
	int m_nLevelSectionCount;
	int m_nZoneCount;
	std::vector<CLevelSection*> m_levelSection;
	std::vector<CZoneDropProb*> m_zoneDropProb;
	int m_underLevel;
	int m_division;
	int m_upperLevel;
	int m_multiple;
};

class CDropItem
{
public:
	CDropItem()
		: m_nItemIndex(-1), m_nDropCount(0)
	{}
	~CDropItem() {}
	void setItemIndex(int val)
	{
		m_nItemIndex = val;
	}
	int getIetmIndex()
	{
		return m_nItemIndex;
	}
	void setDropCount(int val)
	{
		m_nDropCount = val;
	}
	int getDropCount()
	{
		return m_nDropCount;
	}
private:
	int m_nItemIndex;
	int m_nDropCount;
};

class CDropItemManager
{
public:
	CDropItemManager()
		: m_nCount(0) {}
	~CDropItemManager()
	{
		if(m_DropItem.size())
		{
			std::vector<CDropItem*>::iterator it;
			std::vector<CDropItem*>::iterator itEnd = m_DropItem.end();
			for(it = m_DropItem.begin(); it != itEnd; it++)
			{
				delete *it;
				*it = NULL;
			}
			m_DropItem.clear();
		}
	}
	void setCount(int val)
	{
		m_nCount = val;
	}
	int getCount()
	{
		return m_nCount;
	}
	void Load(CConfigFile& eventConfig);
	CItem* getItem();
private:
	int m_nCount;
	std::vector<CDropItem*> m_DropItem;
};

class CItemDropEvent
{
public:
	CItemDropEvent()
		: m_bEventStart(false)
	{}
	~CItemDropEvent() {}
	void Load(CConfigFile& eventConfig);
	void LoadItemDropManager(CConfigFile& eventConfig);
	void LoadDropProbTable(CConfigFile& eventConfig);
	void setEnable(bool b)
	{
		m_bEventStart = b;
	}
	bool getEnable()
	{
		return m_bEventStart;
	}
	CDropItemManager& getDropItemManager()
	{
		return m_DropItemManager;
	}
	CDropProbTable& getDropProbTable()
	{
		return m_DropProbTable;
	}
private:
	bool m_bEventStart;
	CDropItemManager m_DropItemManager;
	CDropProbTable m_DropProbTable;
};
#endif // DEV_EVENT_AUTO

#define MAX_GECHO_SIZE 10
class CGrobalEcho
{
private:
	unsigned int	remainTime[MAX_GECHO_SIZE];
	unsigned int	repeatTime[MAX_GECHO_SIZE];
	unsigned int	checkClock[MAX_GECHO_SIZE];
	CNetMsg::SP		msg[MAX_GECHO_SIZE];

	int	FindIndex( );

public:
	CGrobalEcho( );
	void Set( CNetMsg::SP& _msg, unsigned int _repeatTime , unsigned int _time );
	void Print( );
};

typedef struct _tagGMIPData
{
	char	prefix[20];
	int		from;
	int		to;
} GMIPDATA;

class COXQuizData
{
	int				m_nIndex;			// ���� ��ȣ
	CLCString		m_strQuiz;			// ���� ����
	bool			m_bAnswerOX;		// true : O, false : X

public:
	COXQuizData(int nIndex, const char* strQuiz, bool bAnswerOX)
		: m_strQuiz(256)
	{
		m_nIndex = nIndex;
		m_strQuiz = strQuiz;
		m_bAnswerOX = bAnswerOX;
	}

	int GetQuizIndex() const
	{
		return m_nIndex;
	}
	const char* GetQuizString() const
	{
		return (const char*)m_strQuiz;
	}
	bool GetAnswer() const
	{
		return m_bAnswerOX;
	}
};

class CGoldenBallEventData
{
	int			m_nStatus;			// ���� ���� ����

	CLCString	m_strTeam1;			// A�� �̸�
	CLCString	m_strTeam2;			// B�� �̸�
	int			m_nTeam1Score;		// A�� ����
	int			m_nTeam2Score;		// B�� ����

	int			m_nEndYear;			// ���� �ñ� : ��
	int			m_nEndMonth;		// ���� �ñ� : ��
	int			m_nEndDay;			// ���� �ñ� : ��
	int			m_nEndHour;			// ���� �ñ� : ��
	int			m_nEndMinute;		// ���� �ñ� : ��

	time_t		m_tEndTime;			// ���� �ñ� : time_t
	time_t		m_tVoteEndTime;		// ���� ���� �ð� : time_t

public:
	CGoldenBallEventData()
		: m_strTeam1(GOLDENBALL_TEAM_NAME_LENGTH + 1)
		, m_strTeam2(GOLDENBALL_TEAM_NAME_LENGTH + 1)
	{
		m_nStatus = GOLDENBALL_STATUS_NOTHING;

		m_nTeam1Score = 0;
		m_nTeam2Score = 0;

		m_nEndYear = 0;
		m_nEndMonth = 0;
		m_nEndDay = 0;
		m_nEndHour = 0;
		m_nEndMinute = 0;

		m_tEndTime = 0;
		m_tVoteEndTime = 0;
	}

	int GetStatus()
	{
		return m_nStatus;
	}

	const char* GetTeam1Name()
	{
		return m_strTeam1;
	}
	const char* GetTeam2Name()
	{
		return m_strTeam2;
	}
	int GetTeam1Score()
	{
		return m_nTeam1Score;
	}
	int GetTeam2Score()
	{
		return m_nTeam2Score;
	}

	int GetEndYear()
	{
		return m_nEndYear;
	}
	int GetEndMonth()
	{
		return m_nEndMonth;
	}
	int GetEndDay()
	{
		return m_nEndDay;
	}
	int GetEndHour()
	{
		return m_nEndHour;
	}
	int GetEndMinute()
	{
		return m_nEndMinute;
	}

	time_t GetEndTime()
	{
		return m_tEndTime;
	}
	time_t GetVoteEndTime()
	{
		return m_tVoteEndTime;
	}

	void Vote(const char* strTeam1, const char* strTeam2, int nYear, int nMonth, int nDay, int nHour, int nMin)
	{
		m_nStatus = GOLDENBALL_STATUS_VOTE;

		m_strTeam1 = strTeam1;
		m_strTeam2 = strTeam2;
		m_nTeam1Score = 0;
		m_nTeam2Score = 0;

		m_nEndYear = nYear;
		m_nEndMonth = nMonth;
		m_nEndDay = nDay;
		m_nEndHour = nHour;
		m_nEndMinute = nMin;

		struct tm tmEndTime;
		memset(&tmEndTime, 0, sizeof(tmEndTime));
		tmEndTime.tm_year	= nYear - 1900;
		tmEndTime.tm_mon	= nMonth - 1;
		tmEndTime.tm_mday	= nDay;
		tmEndTime.tm_hour	= nHour;
		tmEndTime.tm_min	= nMin;
		tmEndTime.tm_isdst	= -1;
		m_tEndTime = mktime(&tmEndTime);
		m_tVoteEndTime = m_tEndTime;
	}

	void Gift(const char* strTeam1, int nTeam1Score, const char* strTeam2, int nTeam2Score, int nYear, int nMonth, int nDay, int nHour, int nMin, time_t timeEndVote)
	{
		m_nStatus = GOLDENBALL_STATUS_GIFT;

		m_strTeam1 = strTeam1;
		m_strTeam2 = strTeam2;
		m_nTeam1Score = nTeam1Score;
		m_nTeam2Score = nTeam2Score;

		m_nEndYear = nYear;
		m_nEndMonth = nMonth;
		m_nEndDay = nDay;
		m_nEndHour = nHour;
		m_nEndMinute = nMin;

		struct tm tmEndTime;
		memset(&tmEndTime, 0, sizeof(tmEndTime));
		tmEndTime.tm_year	= nYear - 1900;
		tmEndTime.tm_mon	= nMonth - 1;
		tmEndTime.tm_mday	= nDay;
		tmEndTime.tm_hour	= nHour;
		tmEndTime.tm_min	= nMin;
		tmEndTime.tm_isdst	= -1;
		m_tEndTime = mktime(&tmEndTime);
		m_tVoteEndTime = timeEndVote;
	}

	void EndVote()
	{
		m_nStatus = GOLDENBALL_STATUS_VOTE_END;
	}

	void End()
	{
		m_nStatus = GOLDENBALL_STATUS_NOTHING;
	}
};

class CChatGroup
{
	int m_makeCharIndex;
	int m_chatIndex;

public:

	std::set<CLCString> m_charNameList;

	CChatGroup(int makeCharIndex, int chatIndex);
	~CChatGroup();

	int GetMakeCharIndex()
	{
		return m_makeCharIndex;
	}
	int GetChatIndex()
	{
		return m_chatIndex;
	}

	void SendGroup(MSG_EX_MESSENGER_TYPE subtype, int chatColor, CLCString charName, CLCString chat);
	bool CheckSameName(CLCString charName);
};


class CMoonStoneRewardData
{
public:
	typedef struct __reward
	{
		int		rewardindex;
		int		rewardcount;
		int		rewardflag;
		float	prob;
	} REWARD;

	CMoonStoneRewardData();
	~CMoonStoneRewardData();

	int	m_nMoonStoneIndex;
	int	m_nTotalProb;

	typedef std::map<int, REWARD*> map_reward_t;
	map_reward_t	m_listReward;

	bool Load(int idx);
};

class CMoonStoneReward
{
public:
	CMoonStoneReward();
	~CMoonStoneReward();

	typedef std::map<int, CMoonStoneRewardData*> map_t;
	int	m_nRewardDataCount;
	CMoonStoneRewardData* m_pReward;
	map_t					map_;

	bool	Load();
	CMoonStoneRewardData*	Find(int moonstoneidx)
	{
		map_t::iterator it = map_.find(moonstoneidx);
		return (it != map_.end()) ? it->second : NULL;
	}
};

typedef std::map<int, CParty*>	map_listparty_t;
typedef std::map<int, CExpedition*>	map_listexped_t;
typedef std::map<int, CPartyMatchMember*> map_listPartyMatchMember_t;
typedef std::map<int, CPartyMatchParty*> map_listPartyMatchParty_t;
typedef std::map<LONGLONG, CChatGroup*> map_chatList_t;
typedef std::map<int, COXQuizData*> map_listOXQuiz_t;
typedef std::vector<CNPC*>	vec_listNpcDropItemAtTime_t;
typedef std::map<int, CZone*> map_zone_t;

class CServer : public rnSocketIOHandler
{
public:
	friend class ServerTimerPerHundredMSecond;

public:
	CServer();
	~CServer();

	bool barunsongames_flag;		// �系 �����̸� true

	int m_hardcore_flag_in_gameserver;			// ���̰��� ����ϴ� hardcore server, 0 - no, 1 - yes

	////////////
	// ���� ����
	MessengerInGame* m_messenger;		// �޽��� ����
	ConnectorInGame* m_connector;		// ���� ���� ����
	HelperInGame* m_helper;			// ���� ����
	SubHelperInGame* m_subHelper;			// ���� ����

	int m_nGuildMarkTable[3];

	////////////
	// ���� ����
	char*		m_serverpath;		// ��� ���
	CConfigFile	m_config;			// ���� ����
#ifdef DEV_EVENT_AUTO
	CConfigFile m_eventConfig;		// Event Config
	CEventKingsBirthday m_fathersDay;
	CItemDropEvent m_DropEvent;
#endif

	int			m_national;			// ����
	int			m_serverno;			// ���� ��ȣ
	int			m_subno;			// ���� ��ȣ
	CLCString	m_serverAddr;
	int			m_serverPort;
#ifdef GER_LOG
	char*		m_hostname;			// hostname ����.
#endif

	int			m_maxplayers;		// ��� ���� �ִ� �÷��̾� ��
	bool		m_nameserverisslow; // �� ���� ���ϱ�?
	bool		m_bShutdown;			// �ǵ���
	int			m_pulseReboot;		// ����Ʈ���� ���� �ð�
	int			m_pulseShutdown;	// �˴ٿ���� ���� �ð�
	int			m_pulseEndGame;		// ��ü ���� �ٿ���� ���� �ð�
	int			m_clientversionMin;	// Ŭ���̾�Ʈ ����
	int			m_clientversionMax;	// Ŭ���̾�Ʈ ����
	bool		m_bOnlyLocal;		// �ܺ� ���� ��� ����
	int			m_pulseProcHeartBeatPerSec;		// HeartBeat()���� ���� ó���� ���� ����

	LONGLONG	m_statisticsItemBuy;
	LONGLONG	m_statisticsItemSell;
	LONGLONG	m_statistics152;
	LONGLONG	m_statistics153;
	LONGLONG	m_statistics154;
	LONGLONG	m_statistics155;
	LONGLONG	m_statistics156;
	LONGLONG	m_statistics157;
	LONGLONG	m_statistics158;
	LONGLONG	m_statistics159;
	LONGLONG	m_statistics160;
	LONGLONG	m_statistics161;
	LONGLONG	m_statistics162;
	LONGLONG	m_statistics163;
	LONGLONG	m_statistics197;
	LONGLONG	m_statistics198;
	LONGLONG	m_statistics199;
#define STATISTICS(x, v)		gserver->m_statistics##x += (v)

	LONGLONG	m_taxItem;			// ���ֿ��� �� ���� : ���� �Ǹ�,����
	LONGLONG	m_taxProduceCastle;	// ���ֿ��� �� ���� : ����
	int			m_taxSavePulse;		// ���� ���� �ֱ� : �� ���� ����
	int			m_taxDivGuild;		// ��忡 ���� ���� �ߴ��� ���� : ���� �� �ش� ������ ����, -1�̸� ���� ���� ���� ����

	LONGLONG	m_taxItemDratan;			// ���ֿ��� �� ���� : ���� �Ǹ�,����, ���� �����
	LONGLONG	m_taxProduceCastleDratan;	// ���ֿ��� �� ���� : ����

	bool		m_bLoadPartyInfo;	// ���۷κ��� ���� ��Ƽ ������ ��� �޾Ҵ��� ����

	struct timeval	m_tvLoop;			// ���� �ð� ������
	int				m_nLoop;			// ���� ī��Ʈ
	int				m_bLoop;			// ���� ���� ����
	CDescriptor*	m_descLoop;			// ��û ��ũ����
	CDescriptor*	m_observer;			// ������
	CDescriptor*	m_chatmonitor[20];	// ä�� �����

#ifdef GMTOOL
	CDescriptor**	m_gmtool;
	int				m_nGMToolCount;
	void			GMToolCharPrePlay(CDescriptor* d);			// �������� �������� ��
#endif // GMTOOL

	///////////////////
	// ��ü ��Ƽ ����Ʈ
	map_listparty_t			m_listParty;	// ������ ��ü ��Ƽ ����Ʈ

	// ��ü ������ ����Ʈ
	map_listexped_t			m_listExped;	// ������ ��ü ������ ����Ʈ

	///////////////////
	// ��Ƽ ��Ī ���̺�
	map_listPartyMatchMember_t		m_listPartyMatchMember;		// �Ϲ� ��Ī
	map_listPartyMatchParty_t		m_listPartyMatchParty;		// ��Ƽ ��Ī

	// ���ο� �޽��� ä�ù�
	map_chatList_t					m_chatList;
	// 1 : �ɹ��� �߰� 0 : ä�ù� ���� -1 : ����
	int		addChatGroup(int& m_makeCharIndex, int& chatIndex,  CLCString charName);
	void	delChatGroup(int m_makeCharIndex, int chatIndex, CLCString charName);
	void	SendChatGroup(MSG_EX_MESSENGER_TYPE subtype, int m_makeCharIndex, int chatIndex, int chatColor, CLCString charName, CLCString chat);
	CChatGroup*	FindChatGroup(int makeCharIndex, int chatIndex);
	void	discCharChatGroup(CLCString charName);
	int		GetMaxChatIndexPerPC(int makeCharIndex);

	// ���ӽð�
	int getNowSecond()
	{
		return (int)m_nowseconds;
	}

	///////////
	// ������ ����
	CItemList			m_itemProtoList;		// ��ü ������ ����Ʈ
	CCatalogList		m_catalogList;

	CJewelDataList				m_jewelDataList;

	std::set<int>		m_limitCT;

	CSetItemProtoList	m_setItemProtoList;		// ��Ʈ ������ ����Ʈ

	CFactoryProtoList	m_factoryItemProtoList;		// ���� ������ ����Ʈ

	RewardEvnetItemManager m_rewardItemList;	// �⼮ �̺�Ʈ ���� ������ ����Ʈ

	//////////
	// ƽ ����
	time_t		m_nowseconds;			// unix time (ServerTimerPerSecond::operate()���� �� ����)
	int			m_pulse;			// ƽ ī���� (1/10�� ���� 1�� ������ : ServerTimerPerHundredMSecond::operate()���� �� ����)
	int			m_resetWeather;		// ���� ��ȭ : ���� 30��
	int			m_resetAdjustItem;	// Adjust Item �α� ��� : 1��

	//////////
	// DB ����
	MYSQL		m_dbchar;			// ĳ���� ������
	MYSQL		m_dbdata;			// ���� ������
	MYSQL&		m_dbcastle;			// ���� ������
	MYSQL&		m_dbTrigger;		// Ʈ���� ������

#ifdef USE_TENTER_BILLING
	MYSQL		m_dbcatal;			// ���� īŻ�α� ���
#endif

	MYSQL&		m_dbcharingame;		// ���ӿ��� ����� ĳ���� DB

	// �޽��� ����
	CNetMsg::SP	m_messengerConnMsg;	// �޽��� ���� �޽���

	// ������ ����
	CNetMsg::SP	m_connectorConnMsg;	// ������ ���� �޽���

	// ���� ���� �޽���
	CNetMsg::SP	m_helperConnMsg;

	CNetMsg::SP	m_subHelperConnMsg;

	//////////
	// �� ����
	int			m_numZone;			// ���� ����
	CZone*		m_zones;			// �� �迭
	map_zone_t	m_zones_map;

	//////////////
	// NPC ����
	CNPCProtoList m_npcProtoList;	// NPC �⺻ ����Ʈ

	////////////
	// ��ų ����
	CMagicProtoList		m_magicProtoList;
	CSkillProtoList		m_skillProtoList;

	//////////////
	// Quest ����
	CQuestProtoList m_questProtoList;	// Quest Proto List

	//////////////
	// Option ����
	COptionProtoList m_optionProtoList;	// Option Proto List
	CRareOptionProtoList m_rareOptionList; // ���� �ɼ�

	/////////////////////
	// Special Skill ����
	CSSkillProtoList m_sSkillProtoList;	// Special Skill Proto List
	/////////////
	// ��帮��Ʈ
	CGuildList		m_guildlist;

	CAPetProtoList	m_pApetlist;

	CLuckyDrawBox m_LuckyDrawBox;

	CMonsterMercenary m_MonsterMercenary;
	//////////////
	// �̺�Ʈ
	// 050120 : ����ġ, ����, SP, ����ǰ 2�� �̺�Ʈ
	bool		m_bDoubleEvent;
#ifdef NEW_DOUBLE_GM_ZONE
	int		m_bDoubleEventZone;
#endif // NEW_DOUBLE_GM_ZONE

#ifdef EVENT_DROPITEM
	bool	m_bDropItem;
	int		m_bDropItemNpcIdx;
	int		m_bDropItemIdx;
	int		m_bDropProb;
#endif // EVENT_DROPITEM

#ifdef DOUBLE_PET_EXP_AUTO
	time_t		m_tPetExpEventStart;
	time_t		m_tPetExpEventEnd;
#endif
#ifdef UPGRADE_EVENT_AUTO
	time_t		m_tUpgradeEventStart;
	time_t		m_tUpgradeEventEnd;
#endif
	int			m_bDoubleNasPercent;
	int			m_bDoubleNasGetPercent;
	int			m_bDoubleExpPercent;
	int			m_bDoubleSpPercent;
	int			m_bDoubleProducePercent;
	int			m_bDoubleProduceNum;
#ifdef NEW_DOUBLE_GM_AUTO
	int			m_bDoubleNasPercentBackup;
	int			m_bDoubleNasGetPercentBackup;
	int			m_bDoubleExpPercentBackup;
	int			m_bDoubleSpPercentBackup;
	int			m_bDoubleProducePercentBackup;
	int			m_bDoubleProduceNumBackup;
	int			m_iDoubleGMStart[6];
	int			m_iDoubleGMEnd[6];
#endif // NEW_DOUBLE_GM_AUTO
#ifdef NEW_DOUBLE_EVENT_AUTO_TIME
	int			m_iDoubleEventStart[6];
	int			m_iDoubleEventEnd[6];
#endif // NEW_DOUBLE_EVENT_AUTO_TIME
#ifdef EVENT_ITEMDROP_AUTO
	int			m_iEventItemdropStart[6];
	int			m_iEventItemdropEnd[6];
#endif // EVENT_ITEMDROP_AUTO
	int			m_CollectBugEventPercent;

	bool		m_bDoubleExpEvent;
	int			m_expPercent;
	bool		m_bLattoEvent;
	bool		m_bNewYearEvent;
	bool		m_bValentineEvent;
	bool		m_bWhiteDayEvent;

	bool		m_bpSiteGive;
	int			m_bpSiteCount;

	// ������, ���� ����� ���� ����
	int			m_itemDropProb;
	int			m_moneyDropProb;
//0704
	int			m_aNotice[5]; //�̺�Ʈ�� ���� ����.
	void		addNotice(int _pos, int _index);

	int			m_nMoonStoneNas;

	int			m_nItemDropEventRate;
#ifdef EVENT_ITEMDROP_AUTO
	int			m_nItemDropEventRateBackup;
#endif

	bool		m_bEventOX;					// OX ���� Ȱ��ȭ, start���� true�� �ǰ�, end���� false
	time_t		m_timeEventOXStart;			// OX ���� �̺�Ʈ ���� �ð�
	// m_bEventOX�� true�̰� m_timeEventOXStart �����̸� ���� ����
	map_listOXQuiz_t	m_listOXQuiz;		// ���� ����Ʈ
	int			m_nOXQuizSeqNo;				// ���� ���� ��ȣ, start���� 1�� �ǰ� �� �������� 1�� �����Ѵ�
	bool		m_bOXQuizNoticeStart;		// ���� ���� �˸� ����, start���� false�ǰ� ���� �˸��� ���� true ��
	time_t		m_timeOXQuizLimit;			// ���� ���� �ð�
	int			m_nOXQuizRealNo;			// ���� �������� ���� ��ȣ, ������ -1
	int			m_nOXQuizGMCharIndex;		// ���� ��� �� GM

	bool		m_bRecommend;				// ��õ ���� ���� ����
	int			m_pulseRecommendMoonstone;	// ��õ ���� ������ ���� �ð�

	bool		m_bNonPK;

#ifdef FREE_PK_SYSTEM
	bool		m_bFreePk;
#endif

#ifdef MAL_DISABLE_PKPENALTY
	bool		m_bDisablePKPaenalty;
#endif // MAL_DISABLE_PKPENALTY

	// ��� ��ŷ�� ����ġ ������Ʈ �ֱ�
	int			m_nPulseSaveGuildPoint;

#ifdef  DOUBLE_ITEM_DROP
	bool		m_bDoubleItemEvent;
	int			m_bDoubleItemPercent;

#endif // DOUBLE_ITEM_DROP

#ifdef DOUBLE_PET_EXP
	bool		m_bDoublePetExpEvent;
	int			m_PetExpPercent;
#endif // DOUBLE_PET_EXP

#ifdef DOUBLE_ATTACK
	bool		m_bDoubleAttackEvent;
	int			m_AttackPercent;
#endif // DOUBLE_ATTACK

#ifdef NEW_DOUBLE_EVENT_AUTO
	bool		m_bDoubleEventAuto;
	bool		m_bDoubleEventAutoOn;
#endif // NEW_DOUBLE_EVENT_AUTO

	int m_nDVDCharAttackRate;
	int m_nDVDCharDefanseRate;
	int m_nDVDCharMoveSpeedRate;

	unsigned int m_unEventXmasTreePoint;

	int m_nExpLimit;
	int m_nSpLimit;
#ifdef GM_EXP_LIMIT_AUTO
	int m_nExpLimitStart;
	int m_nExpLimitEnd;
	int m_nSpLimitStart;
	int m_nSpLimitEnd;
	int m_nGMExpLimitStart[6];
	int m_nGMExpLimitEnd[6];
	bool m_bExpLimit;
#endif // GM_EXP_LIMIT_AUTO

	CGoldenBallEventData	m_clGoldenBall;

	struct tm m_tRealSystemTime;

	bool m_bTimeTable[24];			// ��ÿ� �����ϴ��� �������ִ� time flag
	int* m_iZoneExp;				// ����� ����ġ % ����.
	int m_iTimeInterval;			// ��ð����� ����Ÿ������ ����.
	int m_iStartTime;				// ����ð����κ��� ���� ����� ���۽ð�
	int m_iEndTime;					// ����ð����κ��� ���� ����� ����ð�
	bool m_bDungeonTimeToggle;		// ��� ON/OFF TOGGLE
	bool m_bIsTime;					// �ڵ����� ����� �� ���� ����Ÿ������ Ȯ��.
	int m_iZoneCount;

	CGrobalEcho		gecho;
	std::map<int, CNPC*>		m_npc_ctTime;
	std::vector<int>			m_npc_ctTime_erase;

	void NpcCtTimeCount();

	bool DropProbLoad();
	bool m_bCashShopLock;

	virtual void operate(rnSocketIOService* service);

	/////////////////
	// ���� ȯ�� ����
	char* GetServerPath();			// ��� ���
	bool LoadSettingFile();			// ���� ���� �б�
	bool LoadSettings();			// ���� �б�
	bool ConnectDB();				// DB ����
	void DisconnectDB(bool bchar);	// DB ���� ����, bchar�� false�̸� user�� data, true�̸� char
	bool InitGame();				// ���� �ʱ�ȭ
	int GetMaxPlayers();			// ��� ���� �ִ� �÷��̾� �� ���ϱ�
	CZone* FindZone(int zone);			// �� ã��
	// zone�� x, z�� ��ġ���� ����� ���� ã��
	CZone* FindNearestZone(int zone, float x, float z, int* nearZone, int* nearZonePos, int syndicateType = 0);

#ifdef XTRAP
	unsigned char	m_XTrapMap[CSFILE_NUM][XTRAP_CS4_BUFSIZE_MAP];
	bool			InitXTrap();
#endif //XTRAP

	void SetHeaders();						// ����/�޽��� ��� ����

	////////////
	// ���� ����
	void ProcessBilling(CNetMsg::SP& msg);		// ���� ó��

	//////////////
	// �޽��� ����
	void ProcessMessengerMessage(CNetMsg::SP& msg);	// �޽��� �޽��� ó��
	void ProcessEventMessage(CNetMsg::SP& msg);		// �̺�Ʈ �޽��� ó��

	//////////////
	// ���� ����
	void ProcessHelperMessage(CNetMsg::SP& msg);	// ���� �޽��� ó��

	///////////////
	// ���� ���� ( �ŷ������ )
	void ProcessSubHelperMessage( CNetMsg::SP& msg );

	/////////////////
	// ���� ���� ����
	void HeartBeat();		// 1�ʸ��� ����Ǵ� �Լ�

	void CloseSocket(CDescriptor* d);		// ��ũ���� ����, bForce: 15�� ������ ���� ����

	/////////////////
	// ��� ó�� ����
	void CommandInterpreter(CDescriptor* d, CNetMsg::SP& msg); // �� ó��

	//////////////
	// ĳ���� ����
	void CharPrePlay(CDescriptor* d);			// ĳ���� �غ�
	void DecreaseTimeForPC(CPC* ch);			// ĳ���� ���� �ð� ����

	void MoonStoneEndProcess(CPC* ch);

	// ���� ���� : �Ǹ�/����
	void AddTaxItem(LONGLONG val)
	{
		if (val > 0 && m_taxItem + val > 0) m_taxItem += val;
	}
	// ���� ���� : ����
	void AddTaxProduceCastle(LONGLONG val)
	{
		if (val > 0 && m_taxProduceCastle + val > 0) m_taxProduceCastle += val;
	}
	// ���� �ʱ�ȭ
	void ResetTax()
	{
		m_taxItem = m_taxProduceCastle = 0;
	}
	void ResetTaxDratan()
	{
		m_taxItemDratan = m_taxProduceCastleDratan = 0;
	}
	// ���� ���� : �Ǹ�/����
	void AddTaxItemDratan(LONGLONG val)
	{
		if (val > 0 && m_taxItemDratan + val > 0) m_taxItemDratan += val;
	}
	// ���� ���� : ����
	void AddTaxProduceCastleDratan(LONGLONG val)
	{
		if (val > 0 && m_taxProduceCastleDratan + val > 0) m_taxProduceCastleDratan += val;
	}
	// ���� -> ������
	void SaveTax();
	// ���� -> ���
	void DivideTax();
	// ���� ���� ��� ����
	void ChangeTaxGuild();

	// ��Ƽ ����Ʈ ��� ã�� : ���� �ε��� ���
	CParty* FindPartyByBossIndex(int nBossIndex);

	// ��Ƽ ����Ʈ ��� ã�� : ��� �ε��� ���
	CParty* FindPartyByMemberIndex(int nMemberIndex, bool bIncludeRequest);

	// ��Ƽ ����Ʈ ��� ã�� : ���� �ε��� ���
	CExpedition* FindExpedByBossIndex(int nBossIndex);

	// ��Ƽ ����Ʈ ��� ã�� : ��� �ε��� ���
	CExpedition* FindExpedByMemberIndex(int nMemberIndex, bool bIncludeRequest);

	CPartyMatchMember* FindPartyMatchMemberByCharIndex(int nCharIndex);
	CPartyMatchParty* FindPartyMatchPartyByBossIndex(int nBossIndex);

	// OX Quiz Loading
	bool LoadOXQuiz();

	CMoonStoneReward m_moonstoneReward;

	// ���� �ŷ��� ���� �ڷ� ���
	void ProcStatistics();

private:

	// ���� ó��
	void ProcTax();

	// ��, ������ ��Ʈ��
	void ProcMobItemControl();

	// ���� ��ȭ
	void ChangeWeather();

	// loop count ó��
	void ProcLoopCount();

	// OX ���� ó��
	void ProcOXQuiz();

	// ��õ���� ������ ó��
	void ProcRecommendServerMoonstone();

	// ��纼 ���� ó��
	void ProcGoldenBall();

public:

	void ModifyDungeonTime(int flag);
	void CheckDungeonTime();
	void LoadDungeonTime(int count);
	void SaveDungeonTime(int count);
	void SetDungeonTime();
	void ProcMonsterCombo();
	CZone* m_comboZone;
	CMIssionCaseList m_missionCaseList;

#ifdef NEW_DOUBLE_EVENT_AUTO
	void NewDoubleEventAuto();
#endif // NEW_DOUBLE_EVENT_AUTO

#ifdef EXTREME_CUBE
	CExtremeCube m_extremeCube;
#endif // EXTREME_CUBE

	void ProcEndExped();		//������ ���� ó��

	void ProcEndParty();		//��Ƽ ���� ó��

	CTrigger_List	m_CTriggerDBInfo;						// DB���� ���� proto data
	int		GetTriggerSetTypeToZoneIndex(int zoneIndex);	// zone index�� ���� Ʈ���ż�Ʈ�� �����ϴ� �Լ�
	void	ProcTrigger();									// ���� �ʿ��� Ʈ���� üũ

	CAffinityProtoList	m_affinityProtoList;

	CTitleProtoList m_titleProtoList;

#ifdef LACARETTE_SYSTEM
	CLacarette m_lacarette;
#endif

#ifdef DEV_EVENT_PROMOTION2
	CPromotion2 m_promotion2;
#endif

	void CheckMobActivityAndAkanProc(CZone& Zone, CArea& Area);

#ifdef NEW_DOUBLE_GM_AUTO
	bool m_bIsDEtime;
	void SwapDoubleEventValue();
#endif
#ifdef EVENT_ITEMDROP_AUTO
	bool m_bIsItemdrop;
//	void CServer::SwapEventItemdropValue();
#endif
	void CheckEventTime();
	CRaidInfo m_RaidInfo;

#ifdef MEREC_CASTLE_TEST_GUILD_LEVEL_GM_SETTING
	int MeracCastleDebugGuildLevel;
#endif

	int GetCastllanZoneZum( int charIndex );

	void CheckRaidReset();
	void ResetRaid();
	time_t m_tRaidResetProcess;
	int m_nProcess;
	bool m_bCanEnterTheRaidDungeon;
	bool m_bIsCheckTime;
	int m_nResetDay;
	int m_nResethr;
	int m_nResetmin;

	vec_listNpcDropItemAtTime_t		m_listNpcDropItemAtTime;
	void			ItemDropAtTime();
	bool			m_bApplySkillToNPC;

public:
	CRoyalRumble m_RoyalRumble;

	Fortune_proto_list m_Fortunelist;
#ifdef SYSTEM_TREASURE_MAP
	bool bTreasureMapLinkZoneDrop;
	void writeTreasureMapSaveFile();
	void readTreasureMapSaveFile();
#endif

#ifdef REFORM_PK_PENALTY_201108 // ���ÿ� �ѹ� ���� ���� ��ġ ������ �ִ� �÷��̾�� pk ���� ����Ʈ ���� ����������Ѵ�.
	void CheckPkDisPosiotionPointReward();
#endif

	bool m_dratanCastleEnvCheck[3];
	void CheckDratanCastleDungeon(int num);
	void CheckDratanCastleDungeonReward();

#ifdef IMP_SPEED_SERVER
	bool	m_bSpeedServer;
#endif //IMP_SPEED_SERVER

#if defined (INTERGRATION_SERVER)
public:
	CIntergrationServer m_intergrationInfo;
#endif

	CRewardManager m_rewardMgr;

	CDropManager			m_dropItemMgr;
	CEventAutomationManager m_eventAutomationMgr;

	CEventInfo* getEventInfo(int _eventIndex)
	{
		return m_eventAutomationMgr.getEventInfo(_eventIndex);
	}
	CEventInfo* getActiveEventInfo(int _eventIndex)
	{
		return m_eventAutomationMgr.getActiveEventInfo(_eventIndex);
	}
	bool addActiveEvent(int _eventIndex, bool _dbUpdate=false)
	{
		return m_eventAutomationMgr.addActiveEvent(_eventIndex, _dbUpdate);
	}
	bool removeActiveEvent(int _eventIndex, bool _dbUpdate=false);
	bool isActiveEvent(int _eventIndex)
	{
		return m_eventAutomationMgr.isActiveEvent(_eventIndex);
	}
	std::map<int, CEventInfo*> * getActiveEvenList()
	{
		return m_eventAutomationMgr.getActiveEvenList();
	}
	std::map<int, CEventInfo> * getEvenList()
	{
		return m_eventAutomationMgr.getEvenList();
	}
	bool isActiveEventNpc(int _npcDBIndex, int & _eventIndex)
	{
		return m_eventAutomationMgr.isActiveEventNpc( _npcDBIndex, _eventIndex);
	}
	bool isEventNpc(int _npcDBIndex, int & _eventIndex, bool & _bEnable )
	{
		return m_eventAutomationMgr.isEventNpc(_npcDBIndex, _eventIndex, _bEnable );
	}
	void addNpcRegenInfo(CNPCRegenInfo* _pNpcRegenInfo)
	{
		m_eventAutomationMgr.addNpcRegenInfo(_pNpcRegenInfo);
	}
	void doEventDropItem(CNPC* npc, CPC* pc, CPC* tpc);

	bool GetOutDratanDungeon(CPC* pc);
public:
	void PingDB(void);		// DB Ping
private:
	int m_mobAISeq;
	int m_mobAIArea[PASSES_PER_SEC];
	int m_mobAIMobCnt[PASSES_PER_SEC];
public:
	void ClearMobAISeq(void);
	int AddMobAISeq(int regenCnt);
	void RemoveMobAISeq(int minseq, int regenCnt);

	CRockPaperScissorsGame* m_XmasRockPaperScissorsGame;
	CPuzzleGame* m_XmasPuzzleGame;

	int m_DratanCheckTime;

public:
	bool isRunMessenger()
	{
		return m_messenger->isRun();
	}
	bool isRunConnector()
	{
		return m_connector->isRun();
	}
	bool isRunHelper()
	{
		return m_helper->isRun();
	}
	bool isRunSubHelper()
	{
		return m_subHelper->isRun();
	}

	std::set<CPC*> m_kailuxUser;
	std::set<CPC*> m_dealermoonUser;
#ifdef TLD_EVENT_SONG
public:
	bool	tld_event;
	void	checkTldEvent();
#endif
};

extern CServer* gserver;

extern	bool    g_bUpgradeEvent;
extern	int		g_nUpgradeProb;

#endif
//
