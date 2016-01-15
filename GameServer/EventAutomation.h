#ifndef __EVENT_AUTOMATION_H__
#define __EVENT_AUTOMATION_H__

#include "NPCRegenInfo.h"
#include "mysql.h"

class CEventInfo
{
public:

	CEventInfo() : m_index(-1), m_enable(false), m_notice(false)
	{	}
	~CEventInfo()
	{
		m_rewardList.clear();
		m_npcList.clear();
		m_dropList.clear();
		m_npcRegenList.clear();
	}

	void setEventIndex(int _index)
	{
		m_index = _index;
	}
	void setEnable(bool _enable)
	{
		m_enable = _enable;
	}
	void setNotice(bool _notice)
	{
		m_notice = _notice;
	}
	void addRewardIndex(int _rewardIndex)
	{
		m_rewardList.push_back(_rewardIndex);
	}

	const int  getEventIndex()
	{
		return m_index;
	}
	const bool getEnable()
	{
		return m_enable;
	}
	const bool getNotice()
	{
		return m_notice;
	}
	const int  getRewardIndexCount()
	{
		return (int)m_rewardList.size();
	}
	const int  getRewardIndex(int _pos)
	{
		if( _pos < 0 || m_rewardList.size() == 0 || (int)m_rewardList.size() <= _pos )
			return -1;

		return m_rewardList[_pos];
	}

	void addNpc(int _npcDBIndex)
	{
		m_npcList.push_back(_npcDBIndex);
	}
	int getNpcCount()
	{
		return (int)m_npcList.size();
	}
	bool isEventNpc(int _npcDBIndex)
	{
		int i, listSize;
		if( m_npcList.size() == 0)
			return false;
		else
			listSize = (int)m_npcList.size();

		for(i = 0; i < listSize; i++)
		{
			if( getNpcDBIndex(i) == _npcDBIndex)
				return true;
		}
		return false;
	}

	int getNpcDBIndex(int _pos)
	{
		if( _pos < 0 || m_npcList.size() == 0 || (int)m_npcList.size() <= _pos )
			return -1;

		return m_npcList[_pos];
	}

	void addDrop(int _npcDBIndex)
	{
		m_dropList.push_back(_npcDBIndex);
	}
	int getDropCount()
	{
		return (int)m_dropList.size();
	}
	int getDropIndex(int _pos)
	{
		if( _pos < 0 || m_dropList.size() == 0 || (int)m_dropList.size() <= _pos )
			return -1;

		return m_dropList[_pos];
	}

	void addNpcRegenInfo(CNPCRegenInfo* _pNpcRegen)
	{
		m_npcRegenList.push_back(_pNpcRegen);
	}
	void setRegenDisable()
	{
		setRegen(false);
	}
	void setRegenEnable()
	{
		setRegen(true);
	}

private:

	void setRegen( bool _bRegen);

private:
	int m_index;
	bool m_enable;
	bool m_notice;
	// ���� �ε��� ����Ʈ
	std::vector<int> m_rewardList;
	// ���� NPC �ε��� ����Ʈ
	std::vector<int> m_npcList;
	// ��� �ε��� ����Ʈ
	std::vector<int> m_dropList;
	// ���� ���� ����Ʈ
	std::vector<CNPCRegenInfo*> m_npcRegenList;
};

class CEventAutomationManager
{
public:

	CEventAutomationManager()
	{}
	~CEventAutomationManager()
	{
		m_eventList.clear();
		m_activeList.clear();
	}

	bool loadEvent();

	// �̺�Ʈ ã��
	CEventInfo* getEventInfo(int _eventIndex);
	// �������� �̺�Ʈ ã�ƿ���
	CEventInfo* getActiveEventInfo(int _eventIndex);

	// �������� �̺�Ʈ�� ����ϱ�
	bool addActiveEvent(int _eventIndex, bool _dbUpdate=false);

	// �������� ��Ͽ��� �̺�Ʈ ����
	bool removeActiveEvent(int _eventIndex, bool _dbUpdate=false);

	// �������� �̺�Ʈ ���� Ȯ��
	bool isActiveEvent(int _eventIndex);

	// �̺�Ʈ ���� npc Ȯ��
	bool isEventNpc(int _npcDBIndex, int & _eventIndex, bool & _bEnabel );

	// �������� �̺�Ʈ npc���� Ȯ��
	bool isActiveEventNpc(int _npcDBIndex, int & eventIndex );

	// npc���� ���� �ֱ�
	void addNpcRegenInfo(CNPCRegenInfo* _pNpcRegenInfo);

	std::map<int, CEventInfo*> * getActiveEvenList();
	std::map<int, CEventInfo> * getEvenList();

	int getActiveEventCount()
	{
		return (int)m_activeList.size();
	}
private:
	bool dbUpdate(int _eventIndex, bool _enable);

	// �̺�Ʈ ��ü ����Ʈ // first : eventIndex
	std::map<int, CEventInfo> m_eventList;
	// �������� �̺�Ʈ ����Ʈ // first : eventIndex
	std::map<int, CEventInfo*> m_activeList;
};

#endif
