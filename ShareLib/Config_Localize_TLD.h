#ifdef LC_TLD

#define SERVER_AUTHENTICATION		// ���� �⵿ ���� �ý���

#define GIVE_ITEM_IN_CHANGE_JOB		// ������ ������ �����ϱ�
#define TLD_EVENT_SONG

////// ���� �ʵ� ���� //////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//#define		STASH_PASSWORD

#define ITEM_NAME_FIELD         "a_name_thai"
#define NPC_NAME_FIELD          "a_name_thai"
#define ITEM_PRICE_FIELD        "a_price"
#define EVENT_ENABLE_FIELD      "a_enable_tld"
#define EVENT_EXTRA_FILED       "a_extra_tld"

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

#define OX_QUIZ_COUNTRY_CODE    4

////// ��� �̺�Ʈ /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

////// ������Ʈ ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#define DURABILITY
#define CHARDEL_CHECKID                 // �ɸ��� ������ ������ȣ üũ

#define RECOMMEND_SERVER_POTION_INDEX   1393
#define RECOMMEND_SERVER_LEVEL          30
//#define CHARGE_ITEM_PERIOD              // �±� ���� ������ �Ⱓ ���� 110803 �ּ�
#define MAX_UPGRADE_PLUS    25          // ������ ���׷��̵� �ִ� �÷��� ��ġ
#define RANKER_NOTICE

#define PARTY_QUEST_ITEM_BUG_			// ��Ƽ ����Ʈ ������ ������ ���� �ʴ� ����
#define HUNT_QUEST_COMPLETE_FOR_OTHER_PARTYMEMBER	//��Ƽ���� �� ����Ʈ ���ֱ�(�� ����ָ� ī��Ʈ �ǰ� ��. ������ ��Ƽ���� ������ ���� �־�� ���� �Ǿ���)


#ifdef BILL_SERVER_VERSION
#undef BILL_SERVER_VERSION
#define BILL_SERVER_VERSION "TEnterBillV1.0"
#define USE_TENTER_BILLING
#endif
#define FREE_PK_SYSTEM          // ���ο� PK Server Open


#define NEW_DOUBLE_GM_SAVEFILE  ".doubleevent"      // DOUBLE EVENT ��ɾ� ����
#define DEFAULT_NAS_PERCENT     100     // ���� ���Ȯ�� ����
#define DEFAULT_NAS_GET_PERCENT 100     // ���� ���淮 ����
#define DEFAULT_EXP_PERCENT     100     // ����ġ ȹ�淮 ����
#define DEFAULT_SP_PERCENT      100     // sp ȹ�淮 ����
#define DEFAULT_PRO_PERCENT     100     // ���� ���� Ȯ�� ����
#define DEFAULT_PRO_GET_NUMBER  1       // ���� ���� ����

#define TLD_DOUBLE_EVENT    300 // �±��� ���� �̺�Ʈ ��� �� 300%���� ����
#define DOUBLE_ITEM_DROP    // ������ ��� 2�� �̺�Ʈ
#define DOUBLE_ITEM_DROP_SAVEFILE   ".doubleitemevent"      // DOUBLE EVENT ��ɾ� ����

// 070627 �±� ���̵� ���� �ý��� ������Ʈ
#define RAID_NPC_RAGE_SKILL             394     // ���̵� �� �г� ��ų �ε���
#define RAID_NPC_RAGE_SKILL_LEVEL       2       // ���̵� �� �г� ��ų ����
#define RAID_NPC_NOTICE_CELL_EXT        5
#define MAX_RARE_OPTION_SETTING         10      // ���� �ɼǴ� �ɼ� ���� ��
#define MAX_RARE_OPTION_ITEM            10      // �����۴� ���� �ɼ� ��

#define PULSE_MONSTER_RAID_MOVE     (14 * PULSE_REAL_MIN)   // ���� ���̵� �̵� �ð�


// �߰� ��������� 7��
#define DEFAULT_LIMIT_EXP           600

#define NEW_ACCERY_ADD              // ���ο� �Ǽ��縮 4��

#define PULSE_GUILD_POINT_UPDATE        (5 * 60)    // ��� ��ų�� ����ġ ���� ����


#define DISABLE_REFINE_SPECIAL_STONE    // ��� ���ü� ��ȯ �Ұ���



///////////////////////// 2011 / 08 �� ������Ʈ /////////////////////////
#define DROP_MAKE_DOCUMENT										// ���۹��� ���
#define MONSTER_COMBO_FIX										// �����޺� ����
#define CONTINENT_PARTY_RECALL									// ����� ��Ƽ����
#define NO_PERIOD_COMPOSITE										// �Ⱓ���� �ռ� ������
#define DYNAMIC_DUNGEON_BUG_FIX									// ���̳��� ���� ���� ����
#define ATTACK_LIST_BUG_FIX										// ���ø���Ʈ ����
#define RAIDBOX_DROP_COUNT_MODIFY								// ���̵� ��� �ڽ� ���� ����
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PackageII

#define NPC_PORTAL_SCROLL_DRATAN		// ���ź NPC��Ż ��ũ��


#define CHANGE_WARCASTLE_SETTING

#define DEV_EVENT_AUTO					// ����ź���� �̺�Ʈ
// PackageII
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// PackageIII


#define HANARO_EVENT

#define GUILD_CONTRIBUTE_EXP_LIMIT		50				// ��� ��� ����ġ�� max 50% ������ �����ϵ��� ����




///////////////////////// ���׼��� /////////////////////////


#define CASHITEM_EXPSP_LIMIT_BUG		// ����ġ ȹ�������� 300% �̻��� ��� ����ġ�� ����� ���� �ʴ� ���� ���� GPSS : LC-KR-P20100219-013
#define BATTLE_PARTY_BOSS_CHANGE		// ������Ƽ �� ��Ƽ�� ���濡 ���� ���� �˻�(�ǻ��) GPSS : LC-DE-P20100123-001

#define ASSIST_DECREASE_TIME_BUG_FIX	// ���� �ð� ����ȭ, ����(�׿� ��ų ����) ���� Ŭ���̾�Ʈ�� ���� �ð� �ȸ´� �� ����(�ǻ��).

#define _BATTLEGROUP_QUEST_BUG_PIX		// [2010-05-19] ������ ����Ʈ ���� ����(�ֿ���) : ������ �� ��Ÿ ģ ĳ���͸� ����Ʈ�� ������ ī��Ʈ �Ǵ� ���� ����.

#define TIME_IN_WARCASTLE_BUG_FIX		// ������ �ڷ�Ű�׽ý� ��ų �������� ����.
#define BUGFIX_USE_SKILL_TO_NPC			// ������Ʈ1009�� �Բ� ��. �ڼ��� ���׼����� �ڼ��� ������ �׸� ��ġ���..
#define BUGFIX_SPECIALBOX_DROPRATE      // ����� ���� ������ Ȯ�� ���μ��� ����



#define BUGFIX_DEAD_AGAIN_DEAD			// ������ �ͼ� �ѹ��� �״� ���� �������� 3�ʰ� �������� ��.

#define BUGFIX_CASHSHOP_PURCHASE_CONFIRM// ĳ�ü� ���� ����

#define BUGFIX_GUILD_SKILL_COOLTIME		// ��彺ų ��Ÿ�� ���ҿɼ� ����ȵǰ� ����



#define BUGFIX_ALLOW_EXPED_TYPE_SET		// ������ Ÿ�� ����ȭ
#define BUGFIX_CUBE_DELETE_MEMLIST		// ��� Ż�� �� ť�� �������Ʈ���� ����
#define BUGFIX_REMOVE_SPACE
#define BUGFIX_REFINE_SPECIAL_ITEM								// 128���� ������ ��ȯ�� 100% ������ü����� �ִ� ���� ����. ���� �Ȱ��� ó���ϵ��� ����.

// Package 4

#define ITEM_PICK_ZONE_IP_LOG
//2013.05.20 ���� Ÿ�� ����
#define DUNGEON_TIME_SAVEFILE	".dungeontime"	// ����Ÿ�� ���� ���� ����.
#define  DISCONNECT_HACK_CHARACTER		// �� ���α׷� ���� ����
///////////////////////// ���׼��� /////////////////////////
///////////////////////// 2011 / 08 �� ������Ʈ /////////////////////////

// -- LC_TLD

////// ��� ���� ���� ////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#define     GM_LEVEL_ECHO               3
#define     GM_LEVEL_GO_ZONE            3
#define     GM_LEVEL_GOTO               3
#define     GM_LEVEL_ITEMDROP           10
#define     GM_LEVEL_COUNT_PC           8
#define     GM_LEVEL_LOOPCOUNT          3
#define     GM_LEVEL_IMMORTAL           8
#define     GM_LEVEL_SHUTDOWN           3
#define     GM_LEVEL_REBOOT             3
#define     GM_LEVEL_COUNT_NPC          8
#define     GM_LEVEL_VISIBLE            3
#define     GM_LEVEL_GO_PC              3
#define     GM_LEVEL_GO_NPC             8
#define     GM_LEVEL_LEVELUP            9               // �±� 9������ ���� bw : 06526
#define     GM_LEVEL_KICK               3
#define     GM_LEVEL_COMON              3
#define     GM_LEVEL_SUMMON             9
#define     GM_LEVEL_SILENCE            3
#define     GM_LEVEL_WHERE_AM_I         3
#define     GM_LEVEL_QUEST_COMPLETE     8
#define     GM_LEVEL_STATPOINT          9
#define     GM_LEVEL_SKILLPOINT         8
#define     GM_LEVEL_SET_REGENSEC       8
#define     GM_LEVEL_NPC_REGEN          8
#define     GM_LEVEL_KILL_NPC           8
#define     GM_LEVEL_EXPUP              8
#define     GM_LEVEL_BLOODPOINT         8
#define     GM_LEVEL_ENDGAME            3
#define     GM_LEVEL_OBSERVE            8
#define     GM_LEVEL_CHATMONITOR        7
#define     GM_LEVEL_DOUBLE_EVENT       8
#define     GM_LEVEL_LATTO_EVENT        8
#define     GM_LEVEL_DOUBLE_EXP_EVENT   8
#define     GM_LEVEL_NEWYEAR_EVENT      8
#define     GM_LEVEL_VALENTINE_EVENT    8
#define     GM_LEVEL_WHITEDAY_EVENT     8
#define     GM_LEVEL_LETTER_EVENT       8
#define     GM_LEVEL_RELOAD_GMIP        8
#define     GM_LEVEL_SPEEDUP            3
#define     GM_LEVEL_FAMEUP             8
#define     GM_LEVEL_EVENT              8
#define     GM_LEVEL_PD4                8
#define     GM_LEVEL_KICK_ID            5
#define     GM_LEVEL_RESURRECTION       8
#define     GM_LEVEL_RECOMMEND          8
#define     GM_LEVEL_SETLIMIT           8
#define     GM_LEVEL_DISGUISE           8
#define     GM_LEVEL_LOGINSERVER        8
#define     GM_LEVEL_CALTIME            10
#define     GM_LEVEL_KICK_CHAR_INDEX    5
#define     GM_LEVEL_EVENTSHOW          5
#define     GM_LEVEL_EVENTSETTING       10
#define     GM_LEVEL_CHANCE_EVENT       9
#define     GM_LEVEL_GMCHATMONITOR      10
#define     GM_LEVEL_DOUBLE_ITEM_EVENT  8
#define     GM_LEVEL_DOUBLE_PET_EXP_EVENT   9
#define     GM_LEVEL_DOUBLE_ATTACK_EVENT    9
#define     GM_LEVEL_EVENT_DROPITEM     10
#define     GM_LEVEL_KICK_PC_MULRI      9
#define     GM_LEVEL_SILENCE_PC_MULTI   9
#define     GM_LEVEL_SET_EXPLIMIT       10
#define     GM_LEVEL_DOUBLE_EVENT_AUTO  10
#define     GM_LEVEL_UPGRADE_EVENT      10
#define     GM_LEVEL_KICK_GUILDMEMBER   10
#define     GM_LEVEL_APPLY_SKILL        10
#define		GM_LEVEL_STATE_FLAG			10
#define		GM_LEVEL_DATE				1
#define		GM_LEVEL_PLAY_TRIGGER		10
#define		GM_LEVEL_BATTLE_START		10
#define		GM_LEVEL_BATTLE_STOP		10
#define		GM_LEVEL_RESET_RAID			8
#define		GM_LEVEL_SET_SEAL_EXP		10
#define		GM_LEVEL_END_SUBHELPER		10
#define		GM_LEVEL_LCBALL_RESET		10
#define		GM_LEVEL_ECHONPC			9
#define		GM_LEVEL_DT_EXPSET			10
#define     GM_LEVEL_ITEMGET			10
#define		GM_LEVEL_ITEMGETRANGE		10
#define		GM_LEVEL_JOBRESET			10
#define		GM_LEVEL_JOBCHANGE			10


#define SYSTEM_TREASURE_MAP
#define GM_TREASURE_MAP_SAVE_FILE		 ".treasuremapsave"
#define SYSTEM_TREASURE_MAP_LINKZONE_DROP						// ���� ���� ���� ���
#define SYSTEM_TREASURE_MAP_GAMIGO// ���� ã�� �ý���

#define ENABLE_TUTORIAL
#endif // LC_TLD
