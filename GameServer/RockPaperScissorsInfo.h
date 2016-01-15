#ifndef __ROCK_PAPER_SCISSORS_INFO_H__
#define __ROCK_PAPER_SCISSORS_INFO_H__

class CRockPaperScissorsInfo
{
public:
	CRockPaperScissorsInfo();
	~CRockPaperScissorsInfo();

	/**
	* ���� ����
	**/
	void SetWinCount(char cnt);
	char GetWinCount();
	void IncreaseWinCount();

	void SetGameCount(char cnt);
	char GetGameCount();
	void IncreaseGameCount();

	void SetStart(bool bStart);
	bool IsStart();

	void SetTotalCount(int cnt);
	int GetTotalCount();
	void IncreaseTotalCount();

	void SetCanPlay(bool bPlay);
	bool CanPlay();

private:
	char    m_nWinCount;				// ���� �¼�
	char    m_nGameCount;				// ���� ���� ���������� �õ� ��
	bool    m_bStart;					// ���� ���� ����
	int     m_nTotalCount;				// ���� ���� ��
	bool    m_bCanPlay;					// ���� ���� ����
};

#endif
