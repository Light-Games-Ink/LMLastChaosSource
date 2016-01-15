#ifndef __ROCKPAPER_SCISSORS_GAME_H__
#define __ROCKPAPER_SCISSORS_GAME_H__

#include "RockPaperScissorsProb.h"

class CRockPaperScissorsGame
{
public:
	CRockPaperScissorsGame(int rewardIdx, int probIdx);
	~CRockPaperScissorsGame(void);

	enum eWINNER
	{
		RESULT_WIN_USER		= 0,	// ���� �¸�
		RESULT_DRAW			= 1,	// ���º�
		RESULT_WIN_SERVER	= 2,	// ���� �¸�
	};

	/**
	* ���� ����
	* param1 : _in_	user choice
	* param2 : _out_ server choice
	* return value : eWINNER
	**/
	int Play(CPC* user, const unsigned char userChoice, unsigned char& serverChoice);

	/**
	* ��ü �������� ī��Ʈ
	* return value : �� �������� ��
	**/
	const int GetTotalStage(void) const;

	/**
	* ���� ����
	* param1 : ���� ���� ����
	* return value : true:����, false:����
	**/
	bool GiveReward(CPC* user);

	/**
	* Ȯ�� ����
	* In Game Ȯ�� ����
	* DB Ȯ�� ����
	* param1 : ������ ����
	* parma2 : ������ ���� �¸� Ȯ��
	* param3 : ������ ���� �¸� Ȯ��
	* param4 : ������ ��� Ȯ��
	* return value : true(����), false(����)
	**/
	bool ChangeProb(int level, unsigned char userProb, unsigned char serverProb, unsigned char drawProb);

	/**
	* Ȯ�� ���� ���
	* param1 : ���� ����
	* return value : Ȯ�� ����
	**/
	const CRockPaperScissorsProb* GetProb(int level);

private:
	/**
	* ���� ���ð� ���ϱ�
	* param1 : ����
	* param2 : ���� ���ð�
	**/
	unsigned char GetServerChoice(const eWINNER winner, const unsigned char userChoice);

	/**
	* Ȯ�� ���̺� �����ϱ�
	* param1 : Ȯ�� ���̺� �ε����� t_rps_prob�� a_system_idx �ʵ尪
	**/
	void SetProbTable(void);

	/**
	* Ȯ�� �ý��� �ε���
	**/
	int m_probIdx;

	/** ���� ������ ����Ʈ
	* Reward DB Index
	* ���� �������� �¸� Ƚ���� ���� �����ȴ�.
	**/
	int m_rewardIdx;

	/**
	* ��ü �������� ��
	**/
	int m_totalStageCount;

	/**
	* �¸� Ȯ��
	**/
	std::map<int, CRockPaperScissorsProb*> m_winProb;
};

#endif
