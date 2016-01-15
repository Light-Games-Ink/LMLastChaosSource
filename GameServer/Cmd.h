#ifndef __ACMD_H__
#define __ACMD_H__

#include <map>

class CPC;

typedef void (* LCCMDPROC)(CPC*, CNetMsg::SP& msg);

class CCmd
{
public:
	CCmd();
	~CCmd();

	int				m_cmdType;
	int				m_minLevel;
	bool			m_bCanDoWhileWarp;		// ������ ����� �� �ִ� �޽����ΰ�?
	bool			m_bCanDoWhileDisable;	// disable ���¿����� ����� �� �ִ� �޽��� �ΰ�?
	LCCMDPROC		m_cmdFunc;
};

class CCmdList
{
public:
	CCmdList();
	~CCmdList();

	void AddMessage();
	void AddExMessage();

	bool Add(int cmdType, LCCMDPROC cmdFunc, int minlevel, bool bCanDoWhileWarp, bool bCanDoWhileDisable);
	bool Find(int cmdType);
	void Run(int cmd_num, CPC* ch, CNetMsg::SP& msg);

private:
	typedef std::map<int, CCmd*> map_t;
	map_t	map_;
};

#endif
//
