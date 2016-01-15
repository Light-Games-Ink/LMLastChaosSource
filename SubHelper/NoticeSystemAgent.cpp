#include "NoticeSystemAgent.h"

#if defined (NOTICE_SYSTEM)
#include "CmdMsg.h"
#include "Server.h"


CNoticeSystemAgent::CNoticeSystemAgent(void)
{
	m_noticeContainer.clear();
}

CNoticeSystemAgent::~CNoticeSystemAgent(void)
{
	std::deque<CUserNotice *>::iterator itr = m_noticeContainer.begin();
	std::deque<CUserNotice *>::iterator itrEnd = m_noticeContainer.end();
	for ( ; itr != itrEnd; ++itr )
	{
		delete (*itr);
	}

	m_noticeContainer.clear();
}

static CNoticeSystemAgent __instance;
CNoticeSystemAgent* CNoticeSystemAgent::Instance()
{
	return &__instance;
}

void CNoticeSystemAgent::reg(int _characterIndex, CLCString _name, CLCString _contents)
{
	CUserNotice * _notice = new CUserNotice;

	_notice->setCharIndex(_characterIndex);
	_notice->setName(_name);
	_notice->setContents(_contents);

	push_back(_notice);
}

void CNoticeSystemAgent::push_back( CUserNotice * _notice)
{
	m_noticeContainer.push_back(_notice);
}

void CNoticeSystemAgent::push_front( CUserNotice * _notice)
{
	m_noticeContainer.push_front(_notice);
}

CUserNotice* CNoticeSystemAgent::pop(void)
{
	std::deque<CUserNotice *>::iterator itr;

	itr = m_noticeContainer.begin();

	return (*itr);
}

void CNoticeSystemAgent::pop_front(void)
{
	m_noticeContainer.pop_front();
}

int CNoticeSystemAgent::getSize(void)
{
	return m_noticeContainer.size();
}

void CNoticeSystemAgent::operate( rnSocketIOService* service )
{
	// �� 11�ʸ���

	if ( getSize() <= 0 )
		return;

	// ť�� �� �� ������ ��
	CUserNotice* pNotice = pop();

	// ť�� �� �� ������ ����
	pop_front();

	// �� ī���� ����
	pNotice->increaseVieCount();

	// Ŭ���̾�Ʈ�� ������
	CNetMsg::SP msg(new CNetMsg);
	SubHelperUserNoticePrint(msg, pNotice->getName(), pNotice->getContents(), pNotice->getViewCount());
	gserver.SendToAllGameServer(msg);

	// �� ī���Ͱ� 3ȸ �̻��̸� �޸� ����
	if ( pNotice->getViewCount() >= 3 )
	{
		delete pNotice;
	}
	// TODO :: �� ī���Ͱ� 3ȸ �̸��̸� Ǫ�� ��
	else
	{
		push_back(pNotice);
	}
}

#endif // NOTICE_SYSTEM
