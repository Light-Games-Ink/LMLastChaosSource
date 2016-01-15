#ifndef __FRIEND_H__
#define __FRIEND_H__

#include <map>
#include "../ShareLib/MemoryPoolBase.h"

class Friend;

//ģ���� ����ǥ��
typedef enum _tagFriendConditionType
{
	MSG_FRIEND_CONDITION_OFFLINE, //��������
	MSG_FRIEND_CONDITION_NORMAL,  //�Ϲݻ���
//		MSG_FRIEND_CONDITION_HUNTING, //�����
//		MSG_FRIEND_CONDITION_SELLING, //����
	MSG_FRIEND_CONDITION_EMPTY,   //�ڸ����
} MSG_FRIEND_CONDITION_TYPE;

class CFriendGroup
{
	int m_gIndex;
	CLCString m_gName;

public:
	CFriendGroup();
	CFriendGroup(int gIndex, CLCString gName): m_gName(MAX_GROUP_NAME_LENGTH+1)
	{
		m_gIndex = gIndex;
		m_gName = gName;
	}
	~CFriendGroup();

	void SetName(const char* gName)
	{
		m_gName = gName;
	}

	int			GetGroupIndex()
	{
		return m_gIndex;
	}
	const char* GetGroupName()
	{
		return m_gName;
	}
};

class CFriendMember
{
	friend class CFriend;

	int   m_nChaIndex;
	CLCString m_strFriendName;
//	int   m_nLastConnectDate; //�ֱ���������
	char  m_nJobType; //����
	int   m_nCondition; //����
//	int   m_nListIndex;
	int		m_groupIndex;  // 0�� ��Ÿ

//	bool  m_bChatOn;

	CPC*			m_pPc;

public:
	~CFriendMember();
	CFriendMember();

	int GetChaIndex()
	{
		return m_nChaIndex;
	}

	void SetChaIndex(int chaindex);
	void SetName(const char* name);
//	void SetFriendListIndex(int listindex);
	void SetCondition(int condition);
	void SetJob(int job);
	void SetGroup(int gIndex)
	{
		m_groupIndex = gIndex;
	}

	const char* GetName()
	{
		return (const char*)m_strFriendName;
	}
	//int GetFriendListIndex() { return m_nListIndex; }
	int GetCondition()
	{
		return m_nCondition;
	}
	int GetJob()
	{
		return m_nJobType;
	}

	int GetGroup()
	{
		return m_groupIndex;
	}

	void SetPC(CPC* pc)
	{
		m_pPc = pc;
	}
	CPC* GetPC()
	{
		return m_pPc;
	}
};

//ĳ�����ʿ� �����ؾ߰���...
class CFriend : public MemoryPoolBaseWithMutex<CFriend>
{
	//int m_nMyChaIndex; //�ʿ��������..
	CFriendMember* m_pFriendMember[FRIEND_MAX_MEMBER];
	//int m_aFriendMeChaIndex[FRIEND_MAX_MEMBER];
	//int m_nFriendMe;
	int	m_nFriendCnt;
	//int  m_aGroupChat[10]; //�׷�ä������ ĳ���� �ε���
	//int  m_nGroupChatMemberSum; //0�϶��� No�׷�ä��. 1�̻��� �׷�ä�� �ο�.
	typedef std::map<int, CFriendGroup*> map_t;
	map_t	m_groupList;
	int		m_chatColor;

public:
	CFriend();
	~CFriend();
	int AddFriend(int chaindex, const char* name, int job, int nCond, int m_groupIndex);
	void RemoveFriend(int chaindex);
	CFriendMember* FindFriendMember(int chaindex);
	CFriendMember* FindFriendMember(const char* name);
	int GetFriendSum()
	{
		return m_nFriendCnt;
	}
	void SetFriendCount(int count)
	{
		m_nFriendCnt = count;
	}
	int GetFriendCount()
	{
		return m_nFriendCnt;
	}
	CFriendMember* GetFriendMember(int index)
	{
		return (index < 0 || index >= FRIEND_MAX_MEMBER) ? NULL : m_pFriendMember[index];
	};
//	void SetFriendList(int index, int friendindex, const char* name, int job, int condition);
	void SendToAll(CNetMsg::SP& msg);
//	void SetMyCondition(int index, int condition);

	void SetChatColor(int charColor)
	{
		m_chatColor = charColor;
	}
	int GetChatColor()
	{
		return m_chatColor;
	}

	// -1 �̸� ���� �������� gIndex�� ����
	int		addGroup(const char* gName, int gIndex = -1 );
	// -1 �̸� ���� �������� gIndex�� ����
	int	delGroup(int gIndex);
	// -1 �̸� ���� �������� gIndex�� ����
	int	changeGroupName(int gIndex, const char* newName);
	// -1 �̸� ���� �������� gIndex�� ����
	int	moveGroupFriend(int gIndex, int charIndex);
	void	GetGroupIndexString(CLCString& gIndexList, CLCString& gNameList);
	int		GetGroupCount()
	{
		return m_groupList.size();
	}
};

#endif
//
