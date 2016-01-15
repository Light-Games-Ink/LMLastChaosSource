#ifndef __GM_CMD_LIST_H__
#define __GM_CMD_LIST_H__

#include <boost/unordered_map.hpp>

//////////////////////////////////////////////////////////////////////////

typedef void (*GMFunction) (CPC* pc, const char* arg, std::vector<std::string>&);

class CGMCmd
{
public:
	CGMCmd(std::string command, int level, GMFunction fun);
	~CGMCmd(void);

	// ����
	void run(CPC* pc, const char* arg, std::vector<std::string>& vec);

	// get command
	std::string& getCommand(void);

	// get level
	int getLevel(void) const;

	// get function
	GMFunction getFunction(void) const;

private:
	std::string m_command;		// GM Command	: ��ɾ�
	int m_level;				// GM Level		: ���� ���� GM ����
	GMFunction m_function;		// GM Function	: ���� �Լ� ������
};

//////////////////////////////////////////////////////////////////////////

class CGMCmdList
{
private:
	CGMCmdList(void);

public:
	typedef boost::unordered_map<std::string, CGMCmd*> map_t;

	~CGMCmdList(void);

	// GMCmd �ε�
	void load(void);

	// ����
	void run(CPC* pc, const char* arg);

	static CGMCmdList* getInstance(void);

private:
	// GMCmd �߰�
	void add(CGMCmd* gmcmd);

	// �˻�
	CGMCmd* find(std::string& command);

	// ���� �������� Ȯ��
	bool canRun(CGMCmd* gmcmd, CPC* pc);

private:
	static CGMCmdList* m_instance;		// singleton instacne

	map_t	m_GMCmdList;	// GMGmd List
};

#define gGMCmdList CGMCmdList::getInstance()

#endif
