#ifdef LC_KOR

////// ���� �ʵ� ���� //////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
#define PREMIUM_CHAR			// �����̾�ĳ���� (by ���̰�)
#define SERVER_AUTHENTICATION		// ���� �⵿ ���� �ý���

#define		TLD_EVENT_SONG
#define		HARDCORE_SERVER
#define		STASH_PASSWORD
#define DURABILITY		// ��� ������

#define GIVE_ITEM_IN_CHANGE_JOB		// ������ ������ �����ϱ�

#define ITEM_NAME_FIELD         "a_name"
#define NPC_NAME_FIELD          "a_name"
#define ITEM_PRICE_FIELD        "a_price"
#define EVENT_ENABLE_FIELD      "a_enable_kor"
#define EVENT_EXTRA_FILED       "a_extra_kor"

#define ENABLE_SUBJOB							// ����

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

#define OX_QUIZ_COUNTRY_CODE    1

////// ��� �̺�Ʈ /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

////// ������Ʈ �ý��� /////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#define CHARDEL_CHECKID                 // �ɸ��� ������ ������ȣ üũ
#define EMPAS_LOGIN                     // ���Ľ� �α���

#define RECOMMEND_SERVER_POTION_INDEX   1393
#define RECOMMEND_SERVER_LEVEL          60          // 2008 �����̺�Ʈ ���� 30 -> 60���� ����
#define DROP_MAKE_DOCUMENT              // ���ۼ� ���
// User_db�� t_count_external_id ���̺��� �ε����� �����ϰ� ���߾�� ��.
#define MAX_UPGRADE_PLUS    25          // ������ ���׷��̵� �ִ� �÷��� ��ġ

#define RAID_NPC_RAGE_SKILL             394     // ���̵� �� �г� ��ų �ε���
#define RAID_NPC_RAGE_SKILL_LEVEL       2       // ���̵� �� �г� ��ų ����
#define RAID_NPC_NOTICE_CELL_EXT        5
#define MAX_RARE_OPTION_SETTING         10      // ���� �ɼǴ� �ɼ� ���� ��
#define MAX_RARE_OPTION_ITEM            10      // �����۴� ���� �ɼ� ��

#define PULSE_MONSTER_RAID_MOVE     (14 * PULSE_REAL_MIN)   // ���� ���̵� �̵� �ð�

#define PARTY_QUEST_ITEM_BUG_			// ��Ƽ ����Ʈ ������ ������ ���� �ʴ� ����
#define HUNT_QUEST_COMPLETE_FOR_OTHER_PARTYMEMBER	//��Ƽ���� �� ����Ʈ ���ֱ�(�� ����ָ� ī��Ʈ �ǰ� ��. ������ ��Ƽ���� ������ ���� �־�� ���� �Ǿ���)


#define PULSE_GUILD_POINT_UPDATE        (5 * 60)    // ��� ��ų�� ����ġ ���� ����


#define NEW_DOUBLE_GM_SAVEFILE  ".doubleevent"      // DOUBLE EVENT ��ɾ� ����
#define DEFAULT_NAS_PERCENT     100     // ���� ���Ȯ�� ����
#define DEFAULT_NAS_GET_PERCENT 100     // ���� ���淮 ����
#define DEFAULT_EXP_PERCENT     100     // ����ġ ȹ�淮 ����
#define DEFAULT_SP_PERCENT      100     // sp ȹ�淮 ����
#define DEFAULT_PRO_PERCENT     100     // ���� ���� Ȯ�� ����
#define DEFAULT_PRO_GET_NUMBER  1       // ���� ���� ����
#define NEW_DOUBLE_GM_ZONE
#define NEW_DOUBLE_EVENT_AUTO
#define TLD_DOUBLE_EVENT    300 // �±��� ���� �̺�Ʈ ��� �� 300%���� ����
#define DOUBLE_ITEM_DROP    // ������ ��� 2�� �̺�Ʈ
#define DOUBLE_ITEM_DROP_SAVEFILE   ".doubleitemevent"      // DOUBLE EVENT ��ɾ� ����


#define PET_TURNTO_NPC_ITEM             // PET NPC ���� ������ �߰�// �� ���� ������ ( ������ �߰��� ItemUse�� �ε��� ���� )

#define DEFAULT_LIMIT_EXP               250

#define EVENT_PCBANG_2ND

#define HANARO_EVENT		59		// �ϳ��� �̺�Ʈ 2009   - 3ä�η� ������� ��


#define MONSTER_AI              // ���� A.I
#define NO_CHATTING             // ä�� ����
#define EXTREME_CUBE            // �ͽ�Ʈ�� ť��


////////////////////  Ȯ����  ////////////////////
////////////////////  Ȯ����  ////////////////////

#define MONSTER_COMBO_FIX

//////////////////// 2�� 4�� ������Ʈ ////////////////////
//
#define CONTINENT_PARTY_RECALL							// ����� ��Ƽ���� ������ �߰�
#define NO_PERIOD_COMPOSITE								// ���վ����� �� �Ⱓ�� ���� ������ �߰�
//
#define DYNAMIC_DUNGEON_BUG_FIX							// �׿��� �������� ��ų ��� �� ���� ü���� ���� �Ǿ����� ��ó�� �������� ����. maxHP reset���� ���� �׷��� ó�� ����.
#define ATTACK_LIST_BUG_FIX								// GPSS Code : LC-KR-P20091201-012
//////////////////// 2�� 4�� ������Ʈ ////////////////////
////////////////////////////// ��� �߰� /////////////////////////////
#define DUNGEON_TIME_SAVEFILE	".dungeontime"						// ����Ÿ�� ���� ���� ����.
#define LACARETTE_SYSTEM											// ��ī�� �ý��� (������2)
#define IP_NATION_CHECK												// �α��� ����, ������ ������ ��� �������� Ȯ���ϰ� t_users�� �����.
//
//
// �̺�Ʈ ���� �� ���۽ð� ����ð���								//
// �Է��Ͽ� �ڵ����� ����� �� �ֵ��� ��.							//
#define NEW_DOUBLE_GM_AUTO											//
//
#ifdef NEW_DOUBLE_EVENT_AUTO										//
#define NEW_DOUBLE_EVENT_AUTO_TIME									//
#ifdef NEW_DOUBLE_EVENT_AUTO_TIME									//
#define NEW_DOUBLE_EVENT_AUTO_SAVEFILE ".doubleeventautotime"		//
#endif																//
#endif																//
//
#define EVENT_ITEMDROP_AUTO											//
#ifdef EVENT_ITEMDROP_AUTO											//
#define EVENT_ITEMDROP_AUTO_SAVEFILE ".eventitemdropauto"			//
#endif																//
//
#define GM_EXP_LIMIT_AUTO											//
#ifdef GM_EXP_LIMIT_AUTO											//
#define GM_EXP_LIMIT_AUTO_SAVEFILE ".explimitsave"					//
#endif																//
// �̺�Ʈ ���� �� ���۽ð� ����ð���								//
// �Է��Ͽ� �ڵ����� ����� �� �ֵ��� ��.							//
//
#define JUMIN_DB_CRYPT												// ��DB�� �ֺ��ȣ ��ȣȭ�� ���� �ֹι�ȣ üũ ����.(2010/09/16)
//
//
//
#define UPGRADE_EVENT_AUTO											// ���׷��̵� �̺�Ʈ �ڵ�ȭ(2010/10/07)
#define UPGRADE_EVENT_AUTO_SAVEFILE		".upgradeevent"				//
//
#define NPC_PORTAL_SCROLL_DRATAN									// ĳ���� ������ ���ź NPC��ũ�� ����.

////////////////////////// UPDATE 1009 ///////////////////////////////
//
//
//
//
//
//
////////////////////////// UPDATE 1009 ///////////////////////////////
////////////////////////////// ��� �߰� /////////////////////////////

//////////////////// ���̸��������� ///////////////////////

//////////////////// ���̸��������� ///////////////////////
//////////////// MiniPackage(110310) ////////////////////
#define DISCONNECT_HACK_CHARACTER
#define RAIDBOX_DROP_COUNT_MODIFY
//////////////// MiniPackage(110310) ////////////////////
///////////////////////// UPDATE 1104 ////////////////////////////
#ifdef EXTREME_CUBE
#define EXTREME_CUBE_VER2									// ť��2
#endif // EXTREME_CUBE
#define SYSTEM_MONSTER_MERCENARY_CARD_EX
#define SYSTEM_MONSTER_MERCENARY_CARD_LOW_LEVEL_SUPPORT
///////////////////////// UPDATE 1104 ////////////////////////////
///////////////////////// UPDATE 1105 ////////////////////////////
#define SYSTEM_TREASURE_MAP
#define SYSTEM_TREASURE_MAP_GAMIGO// ���� ã�� �ý���
#define GM_TREASURE_MAP_SAVE_FILE    ".treasuremapsave"
#define SYSTEM_TREASURE_MAP_LINKZONE_DROP                       // ���� ���� ���� ���
#define DEV_LETS_PARTYTIME
#define CHANGE_WARCASTLE_SETTING				// ���� �� PVP ���� ����
//#endif
///////////////////////// 110615 UPDATE ////////////////////////////
/////////////// ���� ���� ////////////////

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
/////////////// ���� ���� ////////////////
//////// UPDATE 1106 //////// 42���� ����

#define REFORM_PK_PENALTY_201108



//////// UPDATE 1106 ////////
#define DEV_EVENT_AUTO    // �̺�Ʈ ���� ���Ϸ� �ε�

#define DEV_GUILD_MARK          // ��� ��ũ
#define DEV_GUILD_STASH         // ��� â��

#define NOTICE_SYSTEM           // ���� �ý���

#define GUILD_REMOTE_INVITE             // ��� ���� ���� �ý���

//////// UPDATE 121Q-1 (12�� 1�� 30�� ) ////////


////// ��� ���� ���� ////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#define     GM_LEVEL_ECHO               8
#define     GM_LEVEL_GO_ZONE            3
#define     GM_LEVEL_GOTO               3
#define     GM_LEVEL_ITEMDROP           10
#define     GM_LEVEL_COUNT_PC           10
#define     GM_LEVEL_LOOPCOUNT          10
#define     GM_LEVEL_IMMORTAL           3
#define     GM_LEVEL_SHUTDOWN           9
#define     GM_LEVEL_REBOOT             9
#define     GM_LEVEL_COUNT_NPC          9
#define     GM_LEVEL_VISIBLE            3
#define     GM_LEVEL_GO_PC              3
#define     GM_LEVEL_GO_NPC             3
#define     GM_LEVEL_LEVELUP            8
#define     GM_LEVEL_KICK               5
#define     GM_LEVEL_COMON              8
#define     GM_LEVEL_SUMMON             8
#define     GM_LEVEL_SILENCE            5
#define     GM_LEVEL_WHERE_AM_I         3
#define     GM_LEVEL_QUEST_COMPLETE     7
#define     GM_LEVEL_STATPOINT          7
#define     GM_LEVEL_SKILLPOINT         7
#define     GM_LEVEL_SET_REGENSEC       8
#define     GM_LEVEL_NPC_REGEN          8
#define     GM_LEVEL_KILL_NPC           1
#define     GM_LEVEL_EXPUP              8
#define     GM_LEVEL_BLOODPOINT         8
#define     GM_LEVEL_ENDGAME            9
#define     GM_LEVEL_OBSERVE            9
#define     GM_LEVEL_CHATMONITOR        5
#define     GM_LEVEL_DOUBLE_EVENT       9
#define     GM_LEVEL_LATTO_EVENT        9
#define     GM_LEVEL_DOUBLE_EXP_EVENT   9
#define     GM_LEVEL_NEWYEAR_EVENT      9
#define     GM_LEVEL_VALENTINE_EVENT    9
#define     GM_LEVEL_WHITEDAY_EVENT     9
#define     GM_LEVEL_LETTER_EVENT       9
#define     GM_LEVEL_RELOAD_GMIP        10
#define     GM_LEVEL_SPEEDUP            3
#define     GM_LEVEL_FAMEUP             9
#define     GM_LEVEL_EVENT              9
#define     GM_LEVEL_PD4                9
#define     GM_LEVEL_KICK_ID            5
#define     GM_LEVEL_RESURRECTION       9
#define     GM_LEVEL_RECOMMEND          9
#define     GM_LEVEL_SETLIMIT           9
#define     GM_LEVEL_DISGUISE           9
#define     GM_LEVEL_LOGINSERVER        9
#define     GM_LEVEL_CALTIME            10
#define     GM_LEVEL_KICK_CHAR_INDEX    5
#define     GM_LEVEL_EVENTSHOW          5
#define     GM_LEVEL_EVENTSETTING       9
#define     GM_LEVEL_CHANCE_EVENT       9
#define     GM_LEVEL_GMCHATMONITOR      10
#define     GM_LEVEL_DOUBLE_ITEM_EVENT  9
#define     GM_LEVEL_DOUBLE_PET_EXP_EVENT   9
#define     GM_LEVEL_DOUBLE_ATTACK_EVENT    9
#define     GM_LEVEL_EVENT_DROPITEM     9
#define     GM_LEVEL_KICK_PC_MULRI      9
#define     GM_LEVEL_SILENCE_PC_MULTI   9
#define     GM_LEVEL_SET_EXPLIMIT       9
#define     GM_LEVEL_DOUBLE_EVENT_AUTO  9
#define     GM_LEVEL_UPGRADE_EVENT      9
#define     GM_LEVEL_KICK_GUILDMEMBER   9
#define     GM_LEVEL_APPLY_SKILL        9
#define		GM_LEVEL_STATE_FLAG			10
#define		GM_LEVEL_DATE				1
#define		GM_LEVEL_PLAY_TRIGGER		10
#define		GM_LEVEL_BATTLE_START		9
#define		GM_LEVEL_BATTLE_STOP		9
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

#define ASSIST_DECREASE_SKILL_MP
#define ASSIST_REDUCE_SKILL
#define SKILL_TYPE_NO_COOL_TIME
#define RUS_MD5									// ���þ� bg_user �н����� md5�� ����
// ALTER TABLE `bg_user` CHANGE COLUMN `passwd` `passwd` VARCHAR(32) NOT NULL DEFAULT '0' AFTER `user_id`;
// UPDATE bg_user SET passwd = MD5(passwd);
#define EVENT_PACKAGE_ITEM                      // ��Ű�� �ø����� �̿��� ������ ����
#define CHECKIP_USA
#define MEREC_CASTLE_TEST_GUILD_LEVEL_GM_SETTING

#ifdef BILL_SERVER_VERSION
#undef BILL_SERVER_VERSION
#define BILL_SERVER_VERSION "TEnterBillV1.0"
#define USE_TENTER_BILLING
#endif

#define DOUBLE_PET_EXP				// �� ����ġ �̺�Ʈ �ڵ�ȭ

#ifdef DOUBLE_PET_EXP
#define DOUBLE_PET_EXP_AUTO
#define DOUBLE_PET_EXP_SAVEFILE	".doublepetexp"
#endif

#define EVENT_EUR2_PROMOTION

#define JUNO_RENEWAL_MESSAGEBOX					// ���Ͽ�, �ű�ĳ���� �̵� �� ���� ���ٰ� ����ũ�� ����.

#define EX_ROGUE	// EX �α� ĳ����
#define EX_MAGE			// EX������ ĳ���� �߰�.

#define EGEHA_ZONE_PAYMENT              500000          // ������ �����

#define ENABLE_TUTORIAL
#endif // LC_KOR