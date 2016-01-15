#ifndef __PUZZLE_GAME_H__
#define __PUZZLE_GAME_H__

#include <vector>

class CPuzzleGame
{
public:
	CPuzzleGame(int item[], int itemCnt, int reward = -1);
	CPuzzleGame(std::vector<int> item, int reward = -1);
	~CPuzzleGame(void);

	/**
	* ������ �������� Ȯ���غ���.
	* return value : true(���� ���� ����), false(���� ���� ����)
	**/
	bool CanPlay(CPC* user);

	/**
	* ������ �����սô�.
	* return value : true(���� �÷��� ����), false(���� �÷��� ����)
	**/
	bool Play(CPC* user);

	/**
	* ������ �����սô�.
	**/
	void GiveReward(CPC* user);

private:
	/**
	* ���� �������� �ִ��� Ȯ���غ���.
	* return value : true(����), false(����)
	**/
	bool IsExistItem(CPC* user, int index);

	/**
	* �κ��丮���� ���� �������� ������ ���ؼ� �κ��丮 ������ �����͸� ��������.
	**/
	bool GetRemoveItem(CPC* user, int index, std::vector<CItem*> & itemList);

	/**
	* �κ��丮���� ���� �������� ��¥�� ������.
	**/
	void RemoveItem(CPC* user, std::vector<CItem*> & itemList);

	std::vector<int> m_puzzelIdx;
	int m_rewardIdx;
};

#endif
