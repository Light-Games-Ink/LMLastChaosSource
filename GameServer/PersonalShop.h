#ifndef __PERSONAL_SHOP_H__
#define __PERSONAL_SHOP_H__

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "../ShareLib/MemoryPoolBase.h"

// SHOP CONF
#define PS_MAX_NORMAL_ITEM			10		// �Ϲ� ������ �ŷ� �ִ� ��
#define PS_MAX_PACKAGE_ITEM			5		// ��Ű�� �Ǹ� �ŷ� �ִ� ��
//#define PS_NEED_MONEY_PREMIUM		5000	// �����̾� ���� ���� �ʿ� ����
#define PS_TEX_PREMIUM				200		// ����: �����̾�: 2%

#define PS_TEX_NORMAL				0		// ����: �Ϲ�: 0%

// PERSONAL SHOP TYPE
#define PST_NOSHOP			0
#define PST_SELL			(1 << 0)		// �Ǹ�
#define PST_BUY				(1 << 1)		// ����
#define PST_PREMIUM			(1 << 2)		// �����̾�
#define PST_PACKAGE			(1 << 3)		// ��Ű��
#define PST_SOLDOUT			(1 << 4)		// �Ǹ� ����

class CPersonalShop : public MemoryPoolBaseWithMutex<CPersonalShop>
{
	char			m_type;
	CLCString		m_name;

	int				m_normalitemindex[PS_MAX_NORMAL_ITEM];
	LONGLONG		m_normalCount[PS_MAX_NORMAL_ITEM];
	LONGLONG		m_normalPrice[PS_MAX_NORMAL_ITEM];

	int				m_packageitemindex[PS_MAX_PACKAGE_ITEM];
	LONGLONG		m_packageCount[PS_MAX_PACKAGE_ITEM];
	LONGLONG		m_packagePrice;

public:

	typedef boost::shared_ptr<CPersonalShop> SP;
	typedef boost::weak_ptr<CPersonalShop> WSP;

	CPersonalShop(char type, const char* name);

	bool AddItem(bool bNormal, int itemindex, LONGLONG count, LONGLONG price);

	int FindItem(bool bNormal, int itemindex);
	void RemoveNormalItem(int itemindex, LONGLONG count);
	void RemovePackage();

	char GetType()
	{
		return m_type;
	}
	const char* GetName()
	{
		return (const char*)m_name;
	}
	char GetNormalCount();
	int GetNextNormalItem(int pos);
	int GetNormalItemIndex(int pos)
	{
		return (pos < 0 || pos >= PS_MAX_NORMAL_ITEM) ? -1 : m_normalitemindex[pos];
	}
	LONGLONG GetNormalItemCount(int pos)
	{
		return (pos < 0 || pos >= PS_MAX_NORMAL_ITEM) ? -1 : m_normalCount[pos];
	}
	LONGLONG GetNormalItemPrice(int pos)
	{
		return (pos < 0 || pos >= PS_MAX_NORMAL_ITEM) ? -1 : m_normalPrice[pos];
	}
	LONGLONG GetPackagePrice()
	{
		return m_packagePrice;
	}
	char GetPackageCount();
	int GetNextPackageItem(int pos);
	int GetPackageItemIndex(int pos)
	{
		return (pos < 0 || pos >= PS_MAX_PACKAGE_ITEM) ? -1 : m_packageitemindex[pos];
	}
	LONGLONG GetPackageItemCount(int pos)
	{
		return (pos < 0 || pos >= PS_MAX_PACKAGE_ITEM) ? 0 : m_packageCount[pos];
	}
};

#endif
//
