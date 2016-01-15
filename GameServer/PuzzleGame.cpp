#include "stdhdrs.h"

#include "PuzzleGame.h"
#include "CmdMsg.h"

CPuzzleGame::CPuzzleGame(int item[], int itemCnt, int reward)
	: m_puzzelIdx(item, item+itemCnt), m_rewardIdx(reward)
{
}

CPuzzleGame::CPuzzleGame(std::vector<int> item, int reward)
	: m_puzzelIdx(item), m_rewardIdx(reward)
{
}

CPuzzleGame::~CPuzzleGame(void)
{
}

bool CPuzzleGame::CanPlay(CPC* user)
{
	// �κ��丮�� ���� �������� �ִ��� ã�ƺ���. ���� �������� ������, �÷����� �� ����.
	std::vector<int>::iterator itr = m_puzzelIdx.begin();
	std::vector<int>::iterator itrEnd = m_puzzelIdx.end();
	for ( ; itr != itrEnd ; ++itr)
	{
		if (!IsExistItem(user, *itr))
			return false;
	}

	return true;
}

bool CPuzzleGame::Play(CPC* user)
{
	// ���� ������ �����غ��ô�. ���⼭�� ���� �������� ������ �մϴ�.
	// 1. ���� �������� �����ϱ� ���� �κ��丮�� ������ �����͵��� ���Ϳ� ��ƺ��ô�.
	std::vector<CItem*> itemList;

	std::vector<int>::iterator itr = m_puzzelIdx.begin();
	std::vector<int>::iterator itrEnd = m_puzzelIdx.end();
	for ( ; itr != itrEnd ; ++itr)
	{
		if (!GetRemoveItem(user, *itr, itemList))
			return false;
	}

	// 2. �������� ��¥ �����սô�.
	RemoveItem(user, itemList);

	return true;
}

bool CPuzzleGame::IsExistItem(CPC* user, int index)
{
	// �κ��丮�� ���� �������� �ִ��� Ȯ���غ���.
	if (user->m_inventory.FindByDBIndex(index, 0, 0) == NULL)
		return false;

	return true;
}

bool CPuzzleGame::GetRemoveItem(CPC* user, int index, std::vector<CItem*> & itemList)
{
	CItem* item = user->m_inventory.FindByDBIndex(index, 0, 0);
	if (item == NULL)
		return false;

	itemList.push_back(item);

	return true;
}

void CPuzzleGame::RemoveItem(CPC* user, std::vector<CItem*> & itemList)
{
	std::vector<CItem*>::iterator itr = itemList.begin();
	std::vector<CItem*>::iterator itrEnd = itemList.end();
	for ( ; itr != itrEnd ; ++itr )
	{
		user->m_inventory.decreaseItemCount((*itr), 1);
	}
}

void CPuzzleGame::GiveReward(CPC* user)
{
	if (user == NULL || m_rewardIdx < 0)
		return;

	if( gserver->m_rewardMgr.setRewardData(user, m_rewardIdx, -1, (int)user->m_RockPaperScissorsInfo.GetWinCount() ) )
	{
		if( gserver->m_rewardMgr.doReward(user, m_rewardIdx) )
		{
			// ������ ���� �޽��� ������.

			// �ʱ�ȭ
			user->m_RockPaperScissorsInfo.SetWinCount(0);
			user->m_RockPaperScissorsInfo.SetGameCount(0);
			user->m_RockPaperScissorsInfo.SetStart(false);

			return;
		}
	}

	// ������ ���� ���� �޽���
}
