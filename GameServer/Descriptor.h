#ifndef __ADESCRIPTOR_H__
#define __ADESCRIPTOR_H__

#include "../ShareLib/rnsocketiohandler.h"
#include "../ShareLib/rnsocketioservice.h"
#include "../ShareLib/MemoryPoolBase.h"

#include "TimerItemManager.h"

#define AUTO_SAVE_SEQUENCT		(3)

class CDescriptor : public rnSocketIOHandler, public MemoryPoolBaseWithMutex<CDescriptor>
{
public:
	explicit CDescriptor(rnSocketIOService* service);

	~CDescriptor();

	////////////
	// ���� ����
	time_t		m_connect_time;					// ������ ����� �ð�

	LONGLONG	m_seq_index;
	bool		request_start_game_flag_;		// Ŭ�󸮾�Ʈ�� START_GAME ��Ŷ�� ����
	int			m_index;						// ���� �ε���
	CLCString	m_idname;						// ���� ���̵�
	CLCString	m_passwd;						// ��ȣ
	CLCString	m_ident;						// �ֹι�ȣ ���ڸ�
//	bool		m_bNovice;						//64�ð� �̳� �ʺ���.//0627
	int			m_userFlag;
//0704
	int			m_notice[MAX_NOTICE];			//�̺�Ʈ ���� (�ִ� 5��)

	bool		m_bCreate90Lv;					// 90Lv �̻� -> 100Lv �� ����
	bool		m_bNotCreateNightShadow;			// NS ���� ����
	bool		m_bIsNSCreateCardUse;

	CLCString		m_proSite;

#if defined(EVENT_PCBANG_2ND)
	int			m_location;
#endif

	////////////
	// ���� ����
	unsigned int m_nPrepareSeed;			// ó�� Ű�� �����ϱ� ���� Seed

	//////////
	// �� ����
	int			m_hackCheckPulse;			// �ֱ� �� �˻� ������ Ÿ��
	int			m_hackServerPulse;			// ���� �޽�		: ���� ĳ���� ���ÿ��� �����ǰ�, ���� �ѽð��� �ѹ� ����
	int			m_hackClientPulse;			// Ŭ���̾�Ʈ �޽�	:
	int			m_hackLastClientDiff;		// �ֱ� Ŭ���̾�Ʈ�� �޽� ����
	int			m_hackCount;				// �� �õ� ȸ��

	//////////
	// ƽ ����
	int			m_idletics;					// �Է¾��� ������ ƽ ��
	int			m_checktics;				// 1�ʴ� ��� �Էµ� ���� �����ϱ� ���� ����
	short int	m_commandcount;				// 1�ʴ� ��� �Էµ� ����

	////////////
	// ���� ����
	int			m_connected;				// CON_ ��� ���� ���� : ���� ����
	int			m_logined;					// �α��� ���
	MSG_LOGIN_TYPE m_playmode;				// login mode or play mode
	bool		m_bclosed;
	bool		m_bGoZone;					// �� �̵��ÿ� true�� ����
	bool		m_bGoZoneSent;				// �� �̵� �޽��� ���� ����
	CNetMsg::SP		m_msgGoZone;				// �� �̵� �޽���, m_bGoZoneSent�� false�� ������ �� ����

	////////////
	// ��ũ ����
	CPC*		m_pChar;					// linked to char
	CDescriptor* m_pPrev;					// ����Ʈ ��ũ
	CDescriptor* m_pNext;

	//////////////
	// �̺�Ʈ ����
#ifdef HANARO_EVENT
	CLCString m_hanaroCode;
#endif

	void WriteToOutput(CNetMsg::SP& msg);	// ����

	bool GetLogin(CNetMsg::SP& msg);			// ���� ���̵� ��� �˻�

	bool StartGame(CNetMsg::SP& msg);		// ���� ����
	bool CharNew(CNetMsg::SP& msg);			// ĳ���� ����
	bool CharDel(CNetMsg::SP& msg);			// ĳ���� ����

	//////////
	// �� ����
	void SetHackCheckPulse();			// �� �˻� �޽� ���� ���� �޽� ������ ����
	bool CheckHackPulse(int clientPulse);	// Ŭ���̾�Ʈ�� �޽� ������ ���޽� �˻�
	bool IncreaseHackCount(int n);			// �� ī��Ʈ ����, ���� �� �õ��� ���� ���� ó��

private:
	time_t m_tExtendCharacterSlotTime;
public:
	time_t getExtendCharacterSlotTime() const;
	void setExtendCharacterSlotTime(const time_t _time);

public:
	rnSocketIOService* service_;

	const char* getHostString()
	{
		return service_->ip().c_str();
	}
	void Close(std::string errmsg);
	virtual void operate(rnSocketIOService* service);
	virtual void onClose(rnSocketIOService* service);

public:
	void sendData_StartAndMoveZone();
	void returnToCharacterSelectMode();
	void sendStartData();					//���ӽ� ���� �ѹ��� �����ϴ� ������

#ifdef HARDCORE_SERVER
	void Make_1_Level();
#endif

	void sendSyndicateOutNeedNasInfo();

	// �ڵ� ����� ����ϴ� ����
public:
	unsigned int m_autoSaveSeq;

public:
	static unsigned int static_auto_save_sequence;
	static std::set<CDescriptor*> auto_set[AUTO_SAVE_SEQUENCT];

	static void makeAutoSaveSequence(CDescriptor* desc);
};

#endif
