#ifdef LC_RUS

#define JUNO_RENEWAL_MESSAGEBOX					// ���Ͽ�, �ű�ĳ���� �̵� �� ���� ���ٰ� ����ũ�� ����.
#define SERVER_AUTHENTICATION		// ���� �⵿ ���� �ý���
#define GIVE_ITEM_IN_CHANGE_JOB		// ������ ������ �����ϱ�

////// ���� �ʵ� ���� //////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#define ITEM_NAME_FIELD         "a_name_rus"
#define NPC_NAME_FIELD          "a_name_rus"
#define ITEM_PRICE_FIELD        "a_price"
#define EVENT_ENABLE_FIELD      "a_enable_rus"
#define EVENT_EXTRA_FILED       "a_extra_rus"

////// IP �ڵ� ���� ���� ///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#define IP_BLOCK_AUTO
#if defined (IP_BLOCK_AUTO )
#define IP_BLOCKNOKICK_AUTO     // ������ ����� �϶� ű���� �ʰ� �α׸� ����
#endif // #if defined (IP_BLOCK_AUTO )

////// �ؿ� ���� ���� ���� /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

////// OX QUIZ ���� �ڵ� ///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#define OX_QUIZ_COUNTRY_CODE    17

////// ��� �̺�Ʈ /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

////// ������Ʈ ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#define ENABLE_SUBJOB							// ����
#define XTRAP
#define DURABILITY
#define PREMIUM_CHAR
#define RUS_BCRYPT										// ���þ� Botan bcrypt ����
#define ASSIST_DECREASE_SKILL_MP
#define ASSIST_REDUCE_SKILL
#define SKILL_TYPE_NO_COOL_TIME
#define EX_ROGUE	// EX �α� ĳ����
#define DROP_MAKE_DOCUMENT                              // ���ۼ� ���

#define MAX_UPGRADE_PLUS                25              // ������ ���׷��̵� �ִ� �÷��� ��ġ

#define PULSE_GUILD_POINT_UPDATE        (5 * 60)        // ��� ��ų�� ����ġ ���� ����


#define RECOMMEND_SERVER_POTION_INDEX   1393
#define RECOMMEND_SERVER_LEVEL          60              // 2008 �����̺�Ʈ ���� 30 -> 60���� ����

#define PARTY_QUEST_ITEM_BUG_			// ��Ƽ ����Ʈ ������ ������ ���� �ʴ� ����
#define HUNT_QUEST_COMPLETE_FOR_OTHER_PARTYMEMBER	//��Ƽ���� �� ����Ʈ ���ֱ�(�� ����ָ� ī��Ʈ �ǰ� ��. ������ ��Ƽ���� ������ ���� �־�� ���� �Ǿ���)



// �߰� ��������� 7��

#define DEFAULT_LIMIT_EXP 250


#define RANDOM_ZUNO									   // �ֳ� ���� ������ ��ġ �߰�

#define DEFAULT_NAS_PERCENT             50				// ���� ���Ȯ�� ����
#define DEFAULT_NAS_GET_PERCENT         100				// ���� ���淮 ����
#define DEFAULT_EXP_PERCENT             100				// ����ġ ȹ�淮 ����
#define DEFAULT_SP_PERCENT              100				// sp ȹ�淮 ����
#define DEFAULT_PRO_PERCENT             80				// ���� ���� Ȯ�� ����
#define DEFAULT_PRO_GET_NUMBER          1				// ���� ���� ����


#define CHAT_BLOCK_USA									// ��ġ��
#define CHAT_BLOCK_USA_NEXT_TIME	30	// ���� ��ĥ�� �ִ� �ð�
#define CHAT_BLOCK_USA_LEVEL		1 // ��ġ�� ������ ����

#define EVENT_PACKAGE_ITEM								// ��Ű�� �ø����� �̿��� ������ ����
#define EVENT_PACKAGE_ITEM_DBLOG



#define MONSTER_AI										// ���� AI

#define SETTING_IF_INNER_IP								// �ܺ� �����ǿ� ���� �����ǰ� ������ ��� ...yhj

#define RAID_NPC_RAGE_SKILL				394				// ���̵� �� �г� ��ų �ε���
#define RAID_NPC_RAGE_SKILL_LEVEL		2				// ���̵� �� �г� ��ų ����
#define RAID_NPC_NOTICE_CELL_EXT		5
#define MAX_RARE_OPTION_SETTING			10				// ���� �ɼǴ� �ɼ� ���� ��
#define MAX_RARE_OPTION_ITEM			10				// �����۴� ���� �ɼ� ��
#define PULSE_MONSTER_RAID_MOVE		(14 * PULSE_REAL_MIN)	// ���� ���̵� �̵� �ð�

#define WARCASTLE_SUBNUMBER_MEMBER_VALUE				// �޶�ũ ������ ���� �������� �� üũ�ϱ� ���� �ӽ� ��ġ
#define EGEHA_ZONE_PAYMENT				500000			// ������ �����
#define NEW_ACCERY_ADD									// ���ο� �Ǽ��縮 4��
#define MAL_DISABLE_PKPENALTY							// �������� ����(2ä��)

#define HANARO_EVENT									// �ϳ��� �̺�Ʈ 2009

#define PET_TURNTO_NPC_ITEM

//-----------    Update 13
#define EXTREME_CUBE					// �ͽ�Ʈ�� ť��
#define TRADE_AGENT_LOG_SPEED_CHECK		// �ŷ����� �α� �߰�


////////////////////  Ȯ����  ////////////////////
#define RESTRICT_PVP_SKILL         3		// ������ ��ų ��Ģ : ���� Ÿ���� �Ǿ� ���� ���, PK���, ������� ��� �κ�/������� �Ұ���
// ������ ���ܿ��� �κ�������Ƽ�� �Ƚ������� �־�� �ϴ� define
#define NO_PERIOD_COMPOSITE					// ���վ����� �� �Ⱓ�� ���� ������ �߰�

#define _COMPOSITE_ITEM_UPGRADE_BUG_FIX

//#define QUEST_EXTEND_REPEAT_DAY				// ���� ����Ʈ
////////////////////  Ȯ����  ////////////////////

#define LACARETTE_SYSTEM						// ��ī�� �ý��� (������2)

#define DUNGEON_TIME_SAVEFILE	".dungeontime"	// ����Ÿ�� ���� ���� ����.





#define DEV_SYSTEM_SKILL_EXTEND_2ND			// ĳ���� ��ų Ȯ�� 2��





//-- End of Update 1009 --//

// -- Mini Package 2011 -- //

#define RAIDBOX_DROP_COUNT_MODIFY		// ����� ���� ��� ��Ģ ����
// -- End of Mini Package 2011 -- //

// test server [110513], live server [110517]


// test server [110519], live server [110531]
// ---------------- Update 1104 -----------------------
#ifdef EXTREME_CUBE
#define EXTREME_CUBE_VER2									// ť��2
#endif // EXTREME_CUBE
#define SYSTEM_MONSTER_MERCENARY_CARD_EX
#define SYSTEM_MONSTER_MERCENARY_CARD_LOW_LEVEL_SUPPORT
// ---------------- Update 1104 -----------------------

// test server [110608], live server [110614]
#define CHANGE_WARCASTLE_SETTING				// ���� �� PVP ���� ����

// ---------------- Update 1105 -----------------------
// test server : [110615], live server [110628]
#define DEV_LETS_PARTYTIME
#define SYSTEM_TREASURE_MAP										// ���� ã�� �ý���
#define SYSTEM_TREASURE_MAP_GAMIGO
#define GM_TREASURE_MAP_SAVE_FILE		 ".treasuremapsave"
#define SYSTEM_TREASURE_MAP_LINKZONE_DROP
// ---------------- Update 1105 -----------------------

// ---------------- Event ------------------
#define UPGRADE_EVENT_AUTO			// ���׷��̵� �̺�Ʈ �ڵ�ȭ
#define UPGRADE_EVENT_AUTO_SAVEFILE		".upgradeevent"

// --------------- BugFix -------------------
#define BUGFIX_USE_SKILL_TO_NPC




#define DISCONNECT_HACK_CHARACTER			// �� ���α׷� ���� ����

#define BUGFIX_MERCENARY_GETOWNER_NULL_SERVERDOWN	// �뺴�� ������ ���� �� ���� ũ���ø� ���� ���� ����



#define BUGFIX_GUILD_SKILL_COOLTIME		// ��彺ų ��Ÿ�� ���ҿɼ� ����ȵǰ� ����

// test server [110513], live server [110517]


// test server [110519], live server [110531]
#define BUGFIX_HUGE_SKILL_ABSTIME

// test server [110601], live server [110607]

//////////////////// UPDATE 1106 ////////////////////

#define REFORM_PK_PENALTY_201108
#ifdef REFORM_PK_PENALTY_201108
#define REFORM_PK_PENALTY_BATTLE_201108	 // ����/��� �г�Ƽ
#define REFORM_PK_PENALTY_SHOP_201108	 // ���� �̿� �г�Ƽ
#endif


//////////////////// UPDATE 1106 ////////////////////

#define NO_CHATTING

//////// UPDATE 121Q (12�� 4�� 26�� ) ////////

#define DEV_GUILD_MARK		// ��� ��ũ
#define DEV_GUILD_STASH		// ��� â��

#define NOTICE_SYSTEM           // ���� �ý���

#define GUILD_REMOTE_INVITE		// ��� ���� ���� �ý���

//////// UPDATE 121Q-1 (12�� 4�� 26�� ) ////////

#define EX_MAGE
////// ��� ���� ���� ////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#define     GM_LEVEL_ECHO               7
#define     GM_LEVEL_GO_ZONE            3
#define     GM_LEVEL_GOTO               5
#define     GM_LEVEL_ITEMDROP           10
#define     GM_LEVEL_COUNT_PC           10
#define     GM_LEVEL_LOOPCOUNT          10
#define     GM_LEVEL_IMMORTAL           5
#define     GM_LEVEL_SHUTDOWN           10
#define     GM_LEVEL_REBOOT             10
#define     GM_LEVEL_COUNT_NPC          9
#define     GM_LEVEL_VISIBLE            5
#define     GM_LEVEL_GO_PC              3
#define     GM_LEVEL_GO_NPC             3
#define     GM_LEVEL_LEVELUP            8
#define     GM_LEVEL_KICK               7
#define     GM_LEVEL_COMON              7
#define     GM_LEVEL_SUMMON             8
#define     GM_LEVEL_SILENCE            1
#define     GM_LEVEL_WHERE_AM_I         3
#define     GM_LEVEL_QUEST_COMPLETE     8
#define     GM_LEVEL_STATPOINT          7
#define     GM_LEVEL_SKILLPOINT         7
#define     GM_LEVEL_SET_REGENSEC       8
#define     GM_LEVEL_NPC_REGEN          8
#define     GM_LEVEL_KILL_NPC           8
#define     GM_LEVEL_EXPUP              8
#define     GM_LEVEL_BLOODPOINT         7
#define     GM_LEVEL_ENDGAME            9
#define     GM_LEVEL_OBSERVE            9
#define     GM_LEVEL_CHATMONITOR        7
#define     GM_LEVEL_DOUBLE_EVENT       9
#define     GM_LEVEL_LATTO_EVENT        9
#define     GM_LEVEL_DOUBLE_EXP_EVENT   9
#define     GM_LEVEL_NEWYEAR_EVENT      9
#define     GM_LEVEL_VALENTINE_EVENT    9
#define     GM_LEVEL_WHITEDAY_EVENT     9
#define     GM_LEVEL_LETTER_EVENT       9
#define     GM_LEVEL_RELOAD_GMIP        10
#define     GM_LEVEL_SPEEDUP            7
#define     GM_LEVEL_FAMEUP             7
#define     GM_LEVEL_EVENT              8
#define     GM_LEVEL_PD4                7
#define     GM_LEVEL_KICK_ID            7
#define     GM_LEVEL_RESURRECTION       9
#define     GM_LEVEL_RECOMMEND          8
#define     GM_LEVEL_SETLIMIT           9
#define     GM_LEVEL_DISGUISE           9
#define     GM_LEVEL_LOGINSERVER        10
#define     GM_LEVEL_CALTIME            10
#define     GM_LEVEL_KICK_CHAR_INDEX    7
#define     GM_LEVEL_EVENTSHOW          5
#define     GM_LEVEL_EVENTSETTING       10
#define     GM_LEVEL_CHANCE_EVENT       9
#define     GM_LEVEL_GMCHATMONITOR      10
#define     GM_LEVEL_DOUBLE_ITEM_EVENT  9
#define     GM_LEVEL_DOUBLE_PET_EXP_EVENT   9
#define     GM_LEVEL_DOUBLE_ATTACK_EVENT    9
#define     GM_LEVEL_EVENT_DROPITEM     10
#define     GM_LEVEL_KICK_PC_MULRI      7
#define     GM_LEVEL_SILENCE_PC_MULTI   9
#define     GM_LEVEL_SET_EXPLIMIT       8
#define     GM_LEVEL_DOUBLE_EVENT_AUTO  10
#define     GM_LEVEL_UPGRADE_EVENT      10
#define     GM_LEVEL_KICK_GUILDMEMBER   7
#define     GM_LEVEL_APPLY_SKILL        10
#define		GM_LEVEL_STATE_FLAG			10
#define		GM_LEVEL_DATE				1
#define		GM_LEVEL_PLAY_TRIGGER		10
#define		GM_LEVEL_BATTLE_START		10
#define		GM_LEVEL_BATTLE_STOP		10
#define		GM_LEVEL_RESET_RAID			10
#define		GM_LEVEL_SET_SEAL_EXP		10
#define		GM_LEVEL_END_SUBHELPER		10
#define		GM_LEVEL_LCBALL_RESET		10
#define		GM_LEVEL_ECHONPC			9
#define		GM_LEVEL_DT_EXPSET			10
#define     GM_LEVEL_ITEMGET			10
#define		GM_LEVEL_ITEMGETRANGE		10
#define		GM_LEVEL_JOBRESET			10
#define		GM_LEVEL_JOBCHANGE			10

#endif // LC_RUS

