#ifndef __SYNDICATEMANGER_H__
#define __SYNDICATEMANGER_H__

#include <map>
#include <vector>
#include <deque>
#include <string>
#include <memory.h>

#define SYNDICATE_HISTORY_MAX		(20)

class CPC;
typedef struct __tagSyndicateHistory
{
	int syndicateType;			// ���� Ÿ��
	int time;					// �ڽ��� Ȱ�� �ð�
	int grade;					// ����
	int type;					// Ȱ�� Ÿ�� enum���� ���� - �±�, ����, ����, Ż��, �¸�, �й�
	int targetSyndicateType;	// ������ ��� ���� Ÿ��
	std::string targetName;
} SYNDICATE_HISTORY;

class SyndicateHistoryManager
{
public:
	explicit SyndicateHistoryManager(CPC* pc);
	~SyndicateHistoryManager();

	void	initDBLoading(int syndicateType, SYNDICATE_HISTORY& historys);
	// �����丮 �α�
	void	joinHistory(int syndicateType);
	void	removeHistory(int syndicateType);
	void	degradeHistory(int syndicateType, int grade);
	void	promotionHistory(int syndicateType, int grade);
	void	battleWinHistory(int syndicateType, int targetSyndicateType, const char* targetName);
	void	battleLoseHistory(int syndicateType, int targetSyndicateType, const char* targetName);

	void	sendHistory(int syndicateType);

	void	save(std::vector< std::string > & vec);

private:
	void	pushHistory(SYNDICATE_HISTORY& data);

private:
	typedef std::deque<SYNDICATE_HISTORY> HISTORY_DATA_ELE;
	typedef std::map< int, HISTORY_DATA_ELE > HISTORY_DATAS;
	HISTORY_DATAS historys_;
	CPC* _owner;
};

typedef struct __tagSyndicateData
{
	int				syndicateType;			// Ÿ��
	LONGLONG		syndicate_point_k;
	LONGLONG		syndicate_point_d;
	int				syndicate_join_bit;
	int				syndicate_grade_k;
	int				syndicate_grade_d;

	__tagSyndicateData():syndicateType(0), syndicate_point_k(0), syndicate_point_d(0), syndicate_grade_d(0), syndicate_grade_k(0), syndicate_join_bit(0) {}
} SYNDICATE_DATA;


class SyndicateManager
{
public:
	explicit SyndicateManager(CPC* pc);
	~SyndicateManager();

	CPC* _owner;

	void	initHistoryDBLoading(int syndicateType, SYNDICATE_HISTORY& historys);
	bool	isFirstJoin(int syndicateType);

	void	joinSyndicate(int syndicateType);
	void	removeSyndicate(CItem* item);

	void	setSyndicateType(int syndicateType);
	void	setSyndicateGrade(int syndicateType, int syndicateGrade);

	//�⿩��
	void		setSyndicatePoint(LONGLONG point);
	LONGLONG	getSyndicatePoint(int syndicateType);

	//�⿩��
	void	increaseSyndicatePoint(LONGLONG point);
	void	decreaseSyndicatePoint(LONGLONG point);
	void	resetSyndicatePoint();

	//��� ����
	int		getSyndicateGrade(int syndicateType);

	//���� ���
	void	syndicateAddJewel(int vItemIndex);
	void	syndicateDelJewel(int count);

	bool	isKing();

	void	save(std::vector<std::string>& vec);
	void	send();					//update userinfo
	void	sendRemove();
	void	sendUserInfo();			//response userinfo

	void	applySyndicateSkill(int syndicateType);

	SyndicateHistoryManager historyManager_;
	SYNDICATE_DATA		syndicateData_;

private:
	void	changeGrade(int syndicateType);
	int		getContributionPointByJoin(int syndicateType);
	void	sendJoinCreateJewel(int syndicateType);
	void	sendLeaveCreateJewel(int syndicateType);
};
#endif // __SYNDICATEMANGER_H__