#ifndef __WEAR_INVEN_MANAGER__
#define __WEAR_INVEN_MANAGER__

#include "../ShareLib/packetType/ptype_old_do_item.h"

struct wearItem_search_pair_t
{
	char			wearpos;
	CItem*			pItem;
};
typedef std::vector<wearItem_search_pair_t> wearItem_search_t;

class CWearInvenManager
{
public:
	explicit CWearInvenManager(CPC* pc);
	~CWearInvenManager(void);

public:
	static void Init();
	bool isValidWarePos(int pos);

private:
	static std::set<int>	warePos_set_;
	static int				m_suitWearingPos[6];

	CPC* _owner;
	int _lastError;

	CItem* wearSuitItem;
	int dressCostumCount;

	//wearItemInfo�� ������ wearPos ����
	void DressNormal(CItem* item, int wearPos);
	void DressCostum(CItem* item, int wearPos);
	void DressCostumSuit(CItem* pSuit);

	void UndressNormal(CItem* item, int wearPos);
	void UndressCostum(CItem* item, int wearPos);
	void UndressCostumeSuit();			//��Ʈ �ڽ�Ƭ ����
	void UndressCostumeAll();			//�ڽ�Ƭ ��� ����
	//wearItemInfo�� ������ wearPos ����

	//��ȿ�� �˻� �Լ�
	bool CanDressNormal(CItem* item, int wearPos);
	bool CanDressCostum(CItem* item, int wearPos);
	bool CanDressCostumSuit(CItem* item);

	bool CanUndressNormal(CItem* item, int wearPos);
	bool CanUndressCostum(CItem* item, int wearPos);
	bool CanUndressCostumSuit(CItem* item);
	//��ȿ�� �˻� �Լ�

	//������ �ɼ� ���� �Լ�
	void ApplyDeleteItem(CItem* item, int wearPos);


public:
	std::vector<CItem*> wearItemInfo;

	void SaveWearInvenInfo(std::vector<std::string>& vec);

	bool AddNormalItem(CItem* item, int wearPos);
	bool AddCostumItem(CItem* item, int wearPos);
	bool AddCostumSuitItem(CItem *item);

	bool DelNormalItem(int wearPos, short tab = -1, short invenIndex = -1);
	bool DelCostumItem(int wearPos, short tab = -1, short invenIndex = -1);
	bool DelCostumSuitItem(short tab = -1, short invenIndex = -1);

	//�Ⱓ����, ������ 0 � ���Ǵ� �Լ��Դϴ�.
	bool RemoveItem(char wearPos);

	void initCostumSuitItem(CItem* item);
	void initCostumItem(CItem* item, int wearPos);

	CItem* getWearItem(int wearPos);
	CItem* getCosWearItem(int wearPos);
	CItem* getCosWearSuitItem();
	void setCosWearSuitItem(CItem* item);

	CItem* FindByVirtualIndex(int vIndex);
	CItem* FindByDBIndex(int index);

	int GetLastError()
	{
		return _lastError;
	}

	void sendWearInfoToClient();

	void makeItemInfo( UpdateClient::itemInfo* info, CItem* pitem);
	void makeSaveInfo( std::string* buf, CItem* item, int* count, int wearPos );

	bool isSyndicateEquipment();

	CItem* getCalcWear(int index);

	void searchWearItem(wearItem_search_t& vec);

#ifdef HARDCORE_SERVER
	void sendToLCE_Only_hardcore();
#endif

	void sendOneItemInfo(int wearPos);
	void checkItemTime();

	//�� ��ȭ ��ġ
	int getWearItemPlusSumCount();
};
#endif
