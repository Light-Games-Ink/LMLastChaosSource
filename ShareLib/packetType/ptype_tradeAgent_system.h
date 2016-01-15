#ifndef __PTYPE_TRADEAGENT_H__
#define __PTYPE_TRADEAGENT_H__

#include "ptype_base.h"

#ifdef WIN32
typedef unsigned int tradeAgentIndex_t;
#else
typedef unsigned int tradeAgentIndex_t;
#endif

#define MAX_REG_COUNT 10
#define MAX_LIKE_REG_COUNT 10

//Packet SubType Client <-> Server
enum
{
	MSG_TRADEAGENT_LIST,					// ��ǰ ��ȸ ����Ʈ
	MSG_TRADEAGENT_REG_LIST,				// ��ǰ ��� ����Ʈ
	MSG_TRADEAGENT_REG,						// ��ǰ ���
	MSG_TRADEAGENT_REG_CANCEL,				// ��ǰ ��� ���
	MSG_TRADEAGENT_BUY,						// ��ǰ ����
	MSG_TRADEAGENT_LIKE_LIST,				// ���� ��ǰ ����Ʈ
	MSG_TRADEAGENT_LIKE_REG,				// ���� ��ǰ ���
	MSG_TRADEAGENT_LIKE_CANCEL,				// ���� ��ǰ ���
	MSG_TRADEAGENT_ERROR,					// �ŷ����� ����

	MSG_TRADEAGENT_AFTER_SELL,				// �Ǹ��� �Ǹ��ڿ��� �޽��� ����
	MSG_TRADEAGENT_AFTER_RETURNED,			// ��ǰ �� �Ǹ��ڿ��� �޽��� ����

	MSG_TRADEAGENT_CHANGE_FEE,				// ������ ����
	MSG_TRADEAGENT_CHANGE_DEPOSIT,			// ������ ����
	MSG_TRADEAGENT_RETURN_NAS,				//
};

// ������ �����
enum
{
	TRADEAGENT_DB_MSG_TYPE_SELL,
	TRADEAGENT_DB_MSG_TYPE_RETURNED,
};

#pragma pack(push, 1)

struct TradeAgentItem
{
	tradeAgentIndex_t		index;			// ���� ��ȣ
	int						item_index;
	unsigned char			plus;			// ��ȭ ��ġ
	unsigned char			plus2;			// �߰� ��ġ
	int						flag;
	char					serial[MAX_SERIAL_LENGTH + 1];
	int						item_jobclass;
	int						quantity;
	int						option_count;
	int						option_type[MAX_ITEM_OPTION];
	int						option_level[MAX_ITEM_OPTION];
	int						socketCount;
	int						socket[MAX_SOCKET_COUNT];
	int						item_origin[MAX_ORIGIN_OPTION];
	GoldType_t				nas;
	int						regist_date;
	int						expire_date;
	int						item_num0;				//������ �������ݷ�
	int						item_num1;				//������ �������ݷ�
	int						item_level;
	int						rareGrade;
	char					sell_charName[MAX_ID_NAME_LENGTH];  //MAX_ID_NAME_LENGTH = 64

#ifdef DURABILITY
	int						now_durability;
	int						max_durability;
#endif

	TradeAgentItem()
	{
		memset(this, 0x00, sizeof(*this));
		memset(this->socket, -1, sizeof(this->socket));

#ifdef DURABILITY
		now_durability = 0;
		max_durability = 0;;
#endif
	}
};

namespace RequestClient
{
//�ŷ����� ����Ʈ ��ȸ ��û
struct TradeAgentSystemListBase : public pTypeBase
{
	int charIndex;					//�������� ã�Ƽ� ����� ��� client default = 0
	int pageNo;						// 1~
	char itemName[20];
	int itemTypeTab;				// 0~
	GoldType_t minPrice;
	GoldType_t maxPrice;
};
struct TradeAgentSystemList : public TradeAgentSystemListBase
{
	int JobClass;
	int itemSubType;
	int itemGrade;
	int minItemPlus;
	int maxItemPlus;
	int minAttackValue;
	int maxAttackValue;
	int minMagicValue;
	int maxMagicValue;
	int minSocketCount;
	int maxSocketCount;
	int minLevel;
	int maxLevel;
};

//�ŷ����� ������ ���� ��û
struct TradeAgentSystemBuy : public pTypeBase
{
	int charIndex;					//�������� ã�Ƽ� ����� ��� client default = 0
	int TradeAgentIndex;
	GoldType_t needNas;					//������ ���
};

//�ŷ����� ������ ��� ����Ʈ ��û
struct TradeAgentSystemRegList : public pTypeBase
{
	int charIndex;					//�������� ã�Ƽ� ����� ��� client default = 0
};

//�ŷ����� ������ ��� ��û
struct TradeAgentSystemReg : public pTypeBase
{
	int charIndex;					//�������� ã�Ƽ� ����� ��� client default = 0
	unsigned short tab;
	unsigned short invenIndex;
	int itemIndex;
	int itemSerial;					//���������� ���
	GoldType_t TotalMoney;
	int quantity;
};

struct TradeAgentSystemRegServer : public TradeAgentSystemReg		//������ ��ſ� ���
{
	int charIndex;
	char charName[MAX_CHAR_NAME_LENGTH +1];
	GoldType_t Guarantee;				//�������� ����Ͽ� ���
	TradeAgentItem item;
	int itemCount;
};

//�ŷ����� ������ ��� ��� ��û
struct TradeAgentSystemRegCancel : public pTypeBase
{
	int charIndex;					//�������� ã�Ƽ� ����� ��� client default = 0
	tradeAgentIndex_t index;
};

//�ŷ����� ������ ���ɹ�ǰ ����Ʈ ��û
struct TradeAgentSystemLikeList : public pTypeBase
{
	int charIndex;					//�������� ã�Ƽ� ����� ��� client default = 0
};

//�ŷ����� ������ ���ɹ�ǰ ��� ��û
struct TradeAgentSystemLikeReg : public pTypeBase
{
	int charIndex;					//�������� ã�Ƽ� ����� ��� client default = 0
	tradeAgentIndex_t index;
};

//�ŷ����� ������ ���ɹ�ǰ ��� ��û
struct TradeAgentSystemLikeCancel : public pTypeBase
{
	int charIndex;					//�������� ã�Ƽ� ����� ��� client default = 0
	tradeAgentIndex_t index;
};
}

namespace ResponseClient
{
//Error code
enum
{
	TRADEAGENT_ERROR_NOT_EXIST_ITEM,			// �ش� �������� �������� �ʽ��ϴ�.
	TRADEAGENT_ERROR_NOT_ENOUGH_MONEY,			// ���� �����մϴ�.
	TRADEAGENT_ERROR_SEARCH_ERROR,				// ��ȸ �Ұ�.
	TRADEAGENT_ERROR_INCORRECT_MONEY,			// ���� �����Ͱ� ����Ȯ �մϴ�.
	TRADEAGENT_ERROR_NOT_TRADEITEM,				// �ŷ� �Ұ� �������Դϴ�.
	TRADEAGENT_ERROR_ITEM_REG_FAIL,				// ������ ��Ͽ� �����Ͽ����ϴ�.
	TRADEAGENT_ERROR_ITEM_REG_FULL,				// ����� �� �ִ� �������� ������ �� ã���ϴ�.
	TRADEAGENT_ERROR_ITEM_LIKE_REG_FULL,		// ���ɹ�ǰ���� ��� �� �� �ִ� �������� ������ �� ã���ϴ�.
	TRADEAGENT_ERROR_ITEM_REG_CANCEL_FAIL,		// �Ǹ� �Ϸ�� ������
	TRADEAGENT_ERROR_ITEM_BUY_FAIL,				// ������ ���� ����
	TRADEAGENT_ERROR_ITEM_BUY_ING,				// �������� �Ǹ� �������Դϴ�.
	TRADEAGENT_ERROR_ITEM_LIKE_LIST_FAIL,		// ������ ���ɹ�ǰ ����Ʈ ��ȸ ����(���� ��ǰ�� ����)
	TRADEAGENT_ERROR_ITEM_LIKE_REG_FAIL,		// ������ ���ɹ�ǰ �߰� ����
	TRADEAGENT_ERROR_ITEM_LIKE_CANCEL_FAIL,		// ������ ���ɹ�ǰ ���� ����
};

//�ŷ����� ����Ʈ ��ȸ ����
struct TradeAgentSystemList : public pTypeBase
{
	int charIndex;					//������ ����ߴ� ��, client �����ؾ� �ϴ� ������
	short pageNo;
	unsigned char nextPage;
	int count;
	int	curServerTime;
	TradeAgentItem list[0];
};

//�ŷ����� ������ ���� ����
struct TradeAgentSystemBuy : public pTypeBase
{
	int charIndex;					//������ ����ߴ� ��, client �����ؾ� �ϴ� ������
	GoldType_t needNas;
};

struct TradeAgentSystemBuyWithServer : public TradeAgentSystemBuy
{
	int itemIndex;
	int itemCount;
	GoldType_t itemPrice;
	char itemSerial[MAX_SERIAL_LENGTH + 1];
	char sellerName[MAX_CHAR_NAME_LENGTH +1];
};

//�ŷ����� ������ ��� ����Ʈ ����
struct TradeAgentSystemRegList : public pTypeBase
{
	int charIndex;					//������ ����ߴ� ��, client �����ؾ� �ϴ� ������
	GoldType_t Guarantee;			//���� ��Ͻ� �ʿ��� ������ ��.
	float tradeAgentFee;				//������ %
	int count;
	int	curServerTime;
	TradeAgentItem list[0];
};

//�ŷ����� ������ ��� ����
struct TradeAgentSystemReg : public pTypeBase
{
	int charIndex;					// ������ ����ߴ� ��, client �����ؾ� �ϴ� ������
	int itemIndex;					// ����� ������
	int quantity;				// ����� ������ ���� (���������� ���)
	GoldType_t Guarantee;			// ������ �Է�Ƽ (���������� ���)
	int itemCount;					// ���� �޽����� ������ ���� ������ ī��Ʈ (���������� ���)
	unsigned short tab;						// ���� ��ų �������� �κ� ��ġ (���������� ���)
	unsigned short invenIndex;
};

struct TradeAgentSystemRegWithServer : public TradeAgentSystemReg
{
	char serial[MAX_SERIAL_LENGTH + 1];
	GoldType_t TotalMoney;
};

//�ŷ����� ������ ��� ��� ����
struct TradeAgentSystemRegCancel : public pTypeBase
{
	int charIndex;					//������ ����ߴ� ��, client �����ؾ� �ϴ� ������
};

struct TradeAgentSystemRegCancelWithServer : public TradeAgentSystemRegCancel
{
	int itemIndex;
	int itemCount;
	char serial[MAX_SERIAL_LENGTH + 1];
};

//�ŷ����� ������ ���ɹ�ǰ ����Ʈ ����
struct TradeAgentSystemLikeList : public pTypeBase
{
	int charIndex;					//������ ����ߴ� ��, client �����ؾ� �ϴ� ������
	int count;
	int	curServerTime;
	TradeAgentItem list[0];
};

//�ŷ����� ������ ���ɹ�ǰ ��� ����
struct TradeAgentSystemLikeReg : public pTypeBase
{
	int charIndex;					//������ ����ߴ� ��, client �����ؾ� �ϴ� ������
};

//�ŷ����� ������ ���ɹ�ǰ ��� ����
struct TradeAgentSystemLikeCancel : public pTypeBase
{
	int charIndex;					//������ ����ߴ� ��, client �����ؾ� �ϴ� ������
};

struct TradeAgentSystemError : public pTypeBase
{
	int errorCode;
	int charIndex;					//������ ����ߴ� ��, client �����ؾ� �ϴ� ������
};

struct TradeAgentReturnMoney : public pTypeBase
{
	int charIndex;
	GoldType_t nas;
};

struct TradeAgentAfterSell : public pTypeBase
{
	int charIndex;
	int itemIndex;
	int itemCount;
};

struct TradeAgentAfterReturned : public pTypeBase
{
	int charIndex;
	int itemIndex;
	int itemCount;
};

struct TradeAgentChangeFee : public pTypeBase
{
	float fee;
};

struct TradeAgentChangeDeposit : public pTypeBase
{
	unsigned int deposit;
};
}

#ifndef _CLIENT_

inline void makeTradeAgentError(CNetMsg::SP& msg, int errorCode)
{
	ResponseClient::TradeAgentSystemError *packet = reinterpret_cast<ResponseClient::TradeAgentSystemError*>(msg->m_buf);
	packet->type = MSG_TRADEAGENT;
	packet->subType = MSG_TRADEAGENT_ERROR;
	packet->errorCode = errorCode;
	msg->setSize(sizeof(ResponseClient::TradeAgentSystemError));
}

inline void makeTradeAgentReturnMoney(CNetMsg::SP& msg, int charIndex, GoldType_t nas)
{
	ResponseClient::TradeAgentReturnMoney *packet = reinterpret_cast<ResponseClient::TradeAgentReturnMoney*>(msg->m_buf);
	packet->type = MSG_TRADEAGENT;
	packet->subType = MSG_TRADEAGENT_RETURN_NAS;
	packet->charIndex = charIndex;
	packet->nas = nas;
	msg->setSize(sizeof(ResponseClient::TradeAgentReturnMoney));
}

inline void makeTradeAgentAfterSell(CNetMsg::SP& msg, int charIndex, int itemIndex, int itemCount)
{
	ResponseClient::TradeAgentAfterSell* packet = reinterpret_cast<ResponseClient::TradeAgentAfterSell*>(msg->m_buf);
	packet->type = MSG_TRADEAGENT;
	packet->subType = MSG_TRADEAGENT_AFTER_SELL;
	packet->charIndex = charIndex;
	packet->itemIndex = itemIndex;
	packet->itemCount = itemCount;
	msg->setSize(sizeof(ResponseClient::TradeAgentAfterSell));
}

inline void makeTradeAgentAfterReturned(CNetMsg::SP& msg, int charIndex, int itemIndex, int itemCount)
{
	ResponseClient::TradeAgentAfterReturned* packet = reinterpret_cast<ResponseClient::TradeAgentAfterReturned*>(msg->m_buf);
	packet->type = MSG_TRADEAGENT;
	packet->subType = MSG_TRADEAGENT_AFTER_RETURNED;
	packet->charIndex = charIndex;
	packet->itemIndex = itemIndex;
	packet->itemCount = itemCount;
	msg->setSize(sizeof(ResponseClient::TradeAgentAfterReturned));
}

#endif

#pragma pack(pop)

#endif
