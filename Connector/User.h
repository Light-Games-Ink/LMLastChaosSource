#ifndef __USER_H__
#define __USER_H__

class CUser
{
public:
	int				m_index;		// user �ε��� : ���� �ε���
	CLCString		m_name;			// user id
	CLCString		m_ip;			// ���� ������
	int				m_server;		// ������
	int				m_subnum;		// ���� ��ȣ
	int				m_zone;			// �� ��ȣ
	int				m_listindex;	// ����Ʈ������ �ε���
	int				m_state;		// ����
	int				m_timestamp;	// �÷��� �ð�
	CDescriptor*	m_descserver;	// ������ ������ ��ũ����
	int				m_checkTics;	// ���� üũ ƽ
	char			m_preopen_chk;	// �������� üũ

//0627
	int				m_userFlag;		// �ű԰�����, ���� �޾Ҵ���, ��������

	// ������ �����Ǵ� �κ�
	int				m_usertype;		// ����� Ÿ��(�Ϲ�/��Ű��/����/��Ÿ ���)
	int				m_timeremain;	// ���� ���� ���� �ð�
	int				m_paytype;		// ���� ���
	int				m_location;		// ���� ���(�Ϲ�/���ӹ�/��Ÿ ���)
	CLCString			m_proSite;

#ifdef CHARDEL_CHECKID
	CLCString		m_identification;
#endif

	CUser(int index, const char* name, int server, int subnum, const char* ip, const char* ident = NULL);
};

class CUserList
{
public:
	CUser**			m_users;
	int				m_count;
	int				m_last;
	int				m_playersPerZone[MAX_ZONES];


	CUserList();
	~CUserList();

	CUser* Add(CUser* user);
	bool Remove(CUser* user, bool bFree = true);
	CUser* Find(const char* name);
	CUser* FindByUserIndex(int userindex);
};

#endif
//
