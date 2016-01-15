#include "stdhdrs.h"

#include "RockPaperScissorsGame.h"
#include "Server.h"
#include "../ShareLib/DBCmd.h"

CRockPaperScissorsGame::CRockPaperScissorsGame(int rewardIdx, int probIdx)
	: m_probIdx(probIdx), m_rewardIdx(rewardIdx), m_totalStageCount(0)
{
	SetProbTable();
}

CRockPaperScissorsGame::~CRockPaperScissorsGame(void)
{
}

int CRockPaperScissorsGame::Play(CPC* user, const unsigned char userChoice, unsigned char& serverChoice)
{
	if (user == NULL || static_cast<int>(m_winProb.size()) <= 0)
		return -1;

	// ���������� ������ ������ ���ô�. ������� Result ������ ����˴ϴ�.
	eWINNER Resut;

	// 1. ���� �̱� Ƚ���� ���սô�. �ֳĸ�, �̱� Ƚ���� ���� �̱� Ȯ���� �޶����ϱ��.
	int WinCount = static_cast<int>(user->m_RockPaperScissorsInfo.GetWinCount());
	CRockPaperScissorsProb* probTable = m_winProb[WinCount+1];

	if (probTable == NULL)
		return -1;

	// 2. �̹��� �̱� Ȯ���� ���غ��ô�.
	int WinProb = probTable->GetUserWinProb();

	// 3. �� Ȯ���� ���غ��ô�.
	int LoseProb = probTable->GetServerWinProb();

	// 4. �� �ܿ��� ���� ����Դϴ�.

	// 5. ���и� ������ ���� �������� ���غ��ô�.
	int value = GetRandom(1, 100);

	if (user->getRockPaperScissorsFlag() && user->m_admin == 10)
	{
		WinProb = 100;
		LoseProb = 1;
		value = 1;
	}

	// 6. ���и� Ȯ���� ���ô�.
	if (WinProb >= value)
	{
		// ������ �̰���ϴ�.
		user->m_RockPaperScissorsInfo.IncreaseWinCount();
		Resut = RESULT_WIN_USER;
	}
	else if (LoseProb+WinProb <= value)
	{
		// ������ �̰���ϴ�.
		user->m_RockPaperScissorsInfo.SetStart(false);
		Resut = RESULT_WIN_SERVER;
	}
	else
	{
		// �����ϴ�.
		Resut = RESULT_DRAW;
		serverChoice = userChoice;
	}

	// ��� �޽��� ������.
	serverChoice = GetServerChoice(Resut, userChoice);
	return static_cast<int>(Resut);
}

const int CRockPaperScissorsGame::GetTotalStage(void) const
{
	return m_totalStageCount;
}

bool CRockPaperScissorsGame::GiveReward(CPC* user)
{
	if (user == NULL || m_rewardIdx < 0 || user->m_RockPaperScissorsInfo.GetGameCount() < 1)
	{
		return false;
	}

	int winCount = static_cast<int>(user->m_RockPaperScissorsInfo.GetWinCount());

	if( !gserver->m_rewardMgr.setRewardData( user, m_rewardIdx, -1, winCount ) || !gserver->m_rewardMgr.doReward(user, m_rewardIdx) )
	{
		// ������ ���� ����
		return false;
	}

	// ������ ���� ���� �޽���
	return true;
}

unsigned char CRockPaperScissorsGame::GetServerChoice(const eWINNER winner, const unsigned char userChoice)
{
	unsigned char serverChoice;
	switch (winner)
	{
	case RESULT_WIN_USER:
		{
			if (userChoice == 0)	// ������ �������
				serverChoice = 2;	// ������ ��
			else
				serverChoice = userChoice - 1;
		}
		break;
	case RESULT_WIN_SERVER:
		{
			if (userChoice == 2)	// ������ �����
				serverChoice = 0;	// ������ ����
			else
				serverChoice = userChoice + 1;
		}
		break;
	case RESULT_DRAW:
		{
			serverChoice = userChoice;
		}
		break;
	}

	return serverChoice;
}

void CRockPaperScissorsGame::SetProbTable(void)
{
	CDBCmd cmd;
	cmd.Init(&gserver->m_dbcharingame);

	CLCString query(1024);
	query.Format("SELECT * FROM t_rps_prob WHERE a_system_idx = %d", m_probIdx);

	cmd.SetQuery(query);
	if (!cmd.Open() || cmd.m_nrecords == 0)
		return;

	m_totalStageCount = cmd.m_nrecords;

	while (cmd.MoveNext())
	{
		int level=0;
		unsigned char userProb=0, serverProb=0, drawProb=0;

		cmd.GetRec("a_level", level);
		cmd.GetRec("a_userWinProb", userProb);
		cmd.GetRec("a_serverWinProb", serverProb);
		cmd.GetRec("a_drawProb", drawProb);

		CRockPaperScissorsProb* prob = new CRockPaperScissorsProb(userProb, serverProb, drawProb);
		m_winProb[level] = prob;
	}
}

bool CRockPaperScissorsGame::ChangeProb(int level, unsigned char userProb, unsigned char serverProb, unsigned char drawProb)
{
	if (userProb + serverProb + drawProb != 100)
		return false;

	CRockPaperScissorsProb* probTable = m_winProb[level];
	if (probTable == NULL)
		return false;

	probTable->SetUserWinProb(userProb);
	probTable->SetServerWinProb(serverProb);
	probTable->SetDrawProb(drawProb);

	CDBCmd cmd;
	cmd.Init(&gserver->m_dbcharingame);

	CLCString query(1024);
	query.Format("UPDATE t_rps_prob SET a_userWinProb = %d, a_serverWinProb = %d, a_drawProb = %d WHERE a_system_idx = %d and a_level = %d"
				 , userProb, serverProb, drawProb, m_probIdx, level);

	cmd.SetQuery(query);
	if( !cmd.Update() )
		return false;

	return true;
}

const CRockPaperScissorsProb* CRockPaperScissorsGame::GetProb(int level)
{
	return m_winProb[level];
}
