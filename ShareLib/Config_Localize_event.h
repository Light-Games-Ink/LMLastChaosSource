// ���ú� �̺�Ʈ ����.

#define GMTOOL

#define EVENT_DOUBLE                                // ���� �̺�Ʈ : ����ġ, SP, ����, ����ǰ 2��
//#define   EVENT_VALENTINE                         // �߷�Ÿ�� �̺�Ʈ
//#define       EVENT_WHITEDAY                      // ȭ��Ʈ���� �̺�Ʈ
//#define EVENT_CHANGEWEAPON                        // ���� ��ȯ �̺�Ʈ

#define EVENT_TREASUREBOX               1           // �������� �̺�Ʈ

//#define EVENT_TEACH                   2           // �İ��� �̺�Ʈ

// ģ��ã�� �̺�Ʈ
//#ifndef LC_KOR
//#define EVENT_SEARCHFRIEND_TIME       true        // �޸��ɸ� : ��ɽð� ������Ű��
//#endif

#ifdef LC_TLD
//#define   EVENT_CHILDERN_DAY                  28                  // �±� childern day �̺�Ʈ �İ��� �̺�Ʈ ����
#endif

#if defined(LC_USA)
#define EVENT_SUMMER_VACATION_2007_RESULT                   // �������� �̺�Ʈ ����
#endif

#if defined( EVENT_SUMMER_VACATION_2007_RESULT )
#define PAPER_FIGURE_GREEN_FLOWER       1
#define PAPER_FIGURE_RED_FROG           2
#define PAPER_FIGURE_RED_CRANE          3
#define PAPER_FIGURE_RED_DRAGON         4
#define PAPER_FIGURE_SILVER_FROG        5
#define PAPER_FIGURE_SILVER_CRANE       6
#define PAPER_FIGURE_SILVER_DRAGON      7
#define PAPER_FIGURE_GOLD_FROG          8
#define PAPER_FIGURE_GOLD_CRANE         9
#define PAPER_FIGURE_GOLD_DRAGON        10

#define GREEN_PAPER_ITEM                2367
#define RED_PAPER_ITEM                  2368
#define SILVER_PAPER_ITEM               2369
#define GOLD_PAPER_ITEM                 2370
#endif// defined( EVENT_SUMMER_VACATION_2007_RESULT )

//#if defined (LC_TLD)// �±� �ƹ����� �̺�Ʈ
//#define EVENT_RICHYEAR_CHANGE_2008
//#endif

#if defined (LC_TLD) || defined (LC_KOR)
#define     SAKURA_EVENT_2008_REWARD
#endif

#if defined(LC_KOR) || defined (LC_USA) || defined (LC_TLD) || defined(LC_GAMIGO) || defined(LC_MEX) || defined(LC_BILA)
#define CHAOSBALL                               55      // ī���� �� ��ȯ
#endif

#if defined(LC_MEX)
#define DEV_EVENT_PROMOTION2				69		// ���θ�� �̺�Ʈ :: �� ������ �Ѱ��� ������ ��� ���� �ϵ��� ����.
#endif

#if defined(LC_KOR) || defined (LC_BILA)

#define EVENT_USER_COMEBACK				70		// �޸� ���� �̺�Ʈ

#if defined (LC_KOR) || defined (LC_BILA)
#define EVENT_USER_COMEBACK_AUTO_REG_DAY 90		// �޸� ���� ������ 30��(���� 90�Ͽ��� 30�Ϸ� ����)
#endif

#endif

#if defined (LC_KOR) || defined(LC_GAMIGO) || defined(LC_RUS) || defined(LC_BILA) || defined(LC_USA)
#define EVENT_CHAR_BIRTHDAY					71			// ĳ���� ���� �̺�Ʈ
#endif // (LC_KOR)

#define EVENT_ARTIFACT						84			//���� ��ɲ� �̺�Ʈ

#define NOTICE_EVENT                                    // ���ӳ� �̺�Ʈ ����.

/////////////////////////////////////////////////////////////////////////////////////////////////
////////////// �̺�Ʈ �ڵ�ȭ �ý������� ������ �̺�Ʈ ���Դϴ�. ������� ������. ////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

#define PANDOR_REGEN_TIME                       PULSE_REAL_TENMIN    // �ǵ��� ���� ������ ���� ���  10�� �� ����

//#define EVENT_WORLDCUP_2010				63
//#define EVENT_WORLDCUP_2010_TOTO			64
#define EVENT_WORLDCUP_2010_TOTO_STATUS		65
#define EVENT_WORLDCUP_2010_TOTO_GIFT		66
#define EVENT_WORLDCUP_2010_ATTENDANCE		67