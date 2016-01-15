#ifndef __PTYPE_EXPRESS_H__
#define __PTYPE_EXPRESS_H__

#include "ptype_base.h"

#define EXPRESS_SYSTEM_COUNT_PER_PAGE		(10)
#define EXPRESS_SYSTEM_EXPIRE_DATE			(15)

#define EXPRESS_SENDER_NAME					(16)		// Ŭ���̾�Ʈ �������� ����

#ifdef WIN32
typedef unsigned int expressIndex_t;
#else
typedef unsigned int expressIndex_t;
#endif

// â��NPC (Express system)
enum
{
	MSG_SUB_EXPRESS_EXIST,							// â�� NPC�� ������(�Ǵ� ��)�� �ִ��� 0 - ����, 1 - ����
	MSG_SUB_EXPREES_LIST,							// â�� NPC���� ����Ʈ ��û ó��
	MSG_SUB_EXPRESS_TAKE,							// â�� NPC���� ������(�Ǵ� ��)�� ����
	MSG_SUB_EXPRESS_DELETE,							// â�� NPC���� ������(�Ǵ� ��)�� ����

	MSG_SUB_AUTHORIZE = 100,						// ��� ���� ����
	MSG_SUB_EXPRESS_ALL_USER,						// ������ ��� �������� ������(�Ǵ� ��)���� - Game Server���� ó��
	MSG_SUB_EXPRESS_SERVER_GROUP,					// Ư�� ���� �׷� - Game Server���� ó��
	MSG_SUB_EXPRESS_ZONE,							// Ư�� ���� �׷� / Ư�� �� - Game Server���� ó��
	MSG_SUB_EXPRESS_ONE_USER,						// Ư�� ���� - SubHelper���� ó��

	MSG_SUB_EXPRESS_SEARCH_NICKNAME,				// ������ �̸� �˻�
	MSG_SUB_EXPRESS_SEND_ITEM_TO_NICKNAME,			// �������� Express�� ������ ����
};

enum
{
	EXPRESS_SEND_TYPE_NONE,			// �����Ͱ� ���� ���
	EXPRESS_SEND_TYPE_LOGIN,		// �α��ν� �ڵ����� ������ ���
	EXPRESS_SEND_TYPE_GM,			// ���
	EXPRESS_SEND_TYPE_TRADE_AGENT_NPC_BUY,		// �Ǹ� ���� �ý��� (���ſϷ�)
	EXPRESS_SEND_TYPE_TRADE_AGENT_NPC_SELL,		// �Ǹ� ���� �ý��� (�ǸſϷ�)
	EXPRESS_SEND_TYPE_TRADE_AGENT_NPC_CANCEL,		// �Ǹ� ���� �ý��� (�Ǹ����)
	EXPRESS_SEND_TYPE_TRADE_AGENT_NPC_RETURN,	// �Ǹ� ���� �ý��� (�ݼ�)
	EXPRESS_SEND_TYPE_ETC,			// ��Ÿ
	EXPRESS_SEND_UNSEARCHABLE_STONESTATUS,		// �ź��� ������ ����
};

#pragma pack(push, 1)
//////////////////////////////////////////////////////////////////////////
struct ExpressSystemItemInfo
{
	int				index;			// ���� ��ȣ
	int				item_index;
	unsigned char	plus;			// ��ȭ ��ġ
	unsigned char	plus2;
	int				flag;
	char			serial[MAX_SERIAL_LENGTH + 1];
	int				item_count;
	int				option_count;
	int				option_type[MAX_ITEM_OPTION];
	int				option_level[MAX_ITEM_OPTION];
	int				socket[MAX_SOCKET_COUNT];
	int				item_origin[MAX_ORIGIN_OPTION];
	LONGLONG		nas;
	int				registe_date;
	int				expire_date;
	short			send_type;
	char			sender[EXPRESS_SENDER_NAME + 1];

#ifdef DURABILITY
	int				now_durability;
	int				max_durability;
#endif

	LONGLONG		tradeagent_nas;				//default = -1
	int				tradeagent_itemIndex;		//default = -1
	int				tradeagent_itemCount;		//default = -1

	ExpressSystemItemInfo()
	{
		memset(this, 0x00, sizeof(*this));
		memset(this->socket, -1, sizeof(this->socket));

#ifdef DURABILITY
		now_durability = 0;
		max_durability = 0;
#endif
	}
};

namespace RequestClient
{
struct expressList : public pTypeBase
{
	unsigned short	npcIndex;
	unsigned short	pageIndex;
};

struct expressTake : public pTypeBase
{
	unsigned short	npcIndex;
	expressIndex_t	index;
};

struct expressDelete : public pTypeBase
{
	unsigned short	npcIndex;
	expressIndex_t	index;
};

// �ź��� ������ ����
struct express_unsearchable_stonestatue_searchName : public pTypeBase
{
	char nickName[MAX_CHAR_NAME_LENGTH + 1];
};

struct express_use_unsearchable_stonestatue : public pTypeBase
{
	unsigned short		tab;			// �ź��� ������ ����
	unsigned short		invenIndex;
	int					virualIndex;
	unsigned short		send_tab;		// ���� ������
	unsigned short		send_invenIndex;
	int					send_virutalIndex;
	int					itemCount;
	char toNickName[MAX_CHAR_NAME_LENGTH + 1]; // �޴� ���
};
}

namespace ResponseClient
{
enum
{
	ERR_NO_ERROR,				// ����
	ERR_DB_ERROR,				// ��� ����
	ERR_NO_TAKE_ITEM,			// ã�� ������ ����
	ERR_CAN_NOT_CREATE_ITEM,	// �������� ������ �� ����
	ERR_INVALID_INFORMATION,	// ������(�Ǵ� ��)������ �߸��� ����
	ERR_FULL_INVENTORY,			// ������ ����
	ERR_NOT_FOUND_NPC,			// NPC�� ã�� �� ���ų�, �Ÿ��� �ʹ� �ٴϴ�.

	ERR_NOT_SEND_ITEM,			// �����Ҽ� ���� ������
	ERR_NOT_ENOUGHT_COUNT,		// ������ �������� ������ ���ڸ�
	ERR_NOT_FOUND_NICKNAME_SYS_ERR,	// ���޹޴� nick�� ����, �ڽ��� Express�� �ٽ� �־���
};

struct ExpressExist : public pTypeBase
{
	unsigned char	flag;	// 0 - ����, 1 - ����
};

struct expressList : public pTypeBase
{
	unsigned short	nowPage;
	unsigned char	nextPage;			// 0 - ����, 1 - ����
	unsigned char	count;
	ExpressSystemItemInfo	list[0];
};

struct expressTake : public pTypeBase
{
	unsigned char	result;				// 0 - OK, 1 >= - ERROR
};

struct expressDelete : public pTypeBase
{
	unsigned char	result;				// 0 - OK, 1 >= - ERROR
};

// �ź��� ������ ����
struct express_unsearchable_stonestatus_searchName : public pTypeBase
{
	unsigned char	result;				// 0 - found, 1 - not found
};

struct express_use_unsearchable_stonestatus : public pTypeBase
{
	unsigned char	result;				// 0 - OK, 1 >= - ERROR
};

//////////////////////////////////////////////////////////////////////////
#ifndef _CLIENT_	// Ŭ���̾�Ʈ�� ���� ó��
inline void makeExpressExist(CNetMsg::SP& msg, int flag)
{
	ExpressExist* packet = reinterpret_cast<ExpressExist*>(msg->m_buf);
	msg->m_mtype = packet->type = MSG_EXPRESS_SYSTEM;
	packet->subType = MSG_SUB_EXPRESS_EXIST;
	packet->flag = flag;
	msg->setSize(sizeof(ExpressExist));
}

inline void makeExpressTake(CNetMsg::SP& msg, int result)
{
	expressTake* packet = reinterpret_cast<expressTake*>(msg->m_buf);
	msg->m_mtype = packet->type = MSG_EXPRESS_SYSTEM;
	packet->subType = MSG_SUB_EXPRESS_TAKE;
	packet->result = result;
	msg->setSize(sizeof(expressTake));
}

inline void makeExpressDelete(CNetMsg::SP& msg, int result)
{
	expressTake* packet = reinterpret_cast<expressTake*>(msg->m_buf);
	msg->m_mtype = packet->type = MSG_EXPRESS_SYSTEM;
	packet->subType = MSG_SUB_EXPRESS_DELETE;
	packet->result = result;
	msg->setSize(sizeof(expressTake));
}

inline void makeExpressSearchNickname(CNetMsg::SP& msg, int result)
{
	express_unsearchable_stonestatus_searchName* packet = reinterpret_cast<express_unsearchable_stonestatus_searchName*>(msg->m_buf);
	packet->type = MSG_EXPRESS_SYSTEM;
	packet->subType = MSG_SUB_EXPRESS_SEARCH_NICKNAME;
	packet->result = result;
	msg->setSize(sizeof(express_unsearchable_stonestatus_searchName));
}

inline void makeExpressSendToNickName(CNetMsg::SP& msg, int result)
{
	express_use_unsearchable_stonestatus* packet = reinterpret_cast<express_use_unsearchable_stonestatus*>(msg->m_buf);
	packet->type = MSG_EXPRESS_SYSTEM;
	packet->subType = MSG_SUB_EXPRESS_SEND_ITEM_TO_NICKNAME;
	packet->result = result;
	msg->setSize(sizeof(express_use_unsearchable_stonestatus));
}

#endif
}

//////////////////////////////////////////////////////////////////////////
// Messenger server <---> GM TOOL
namespace RequestGMTool
{
struct authorize : public pTypeBase
{
};

/*
	server_no == -1 && zone_no == -1 : ��� ����
	server_no != -1 && zone_no == -1 : Ư�� ������
	server_no == -1 && zone_no != -1 : ��� �������� Ư�� ��
*/
struct sendItemToPlayingUser : public pTypeBase
{
	int			server_no;
	int			zone_no;
	int			item_index;
	int			item_count;
	int			nas;
	int			minLevel;
	int			maxLevel;
	char		sender[EXPRESS_SENDER_NAME + 1];
};

struct sendItemToOneUser : public pTypeBase
{
	int			server_no;
	int			char_index;
	int			item_index;
	int			item_count;
	int			nas;
	short		send_type;
	char		sender[EXPRESS_SENDER_NAME + 1];
};
}

namespace ResponseGMTool
{
enum
{
	ERR_NO_ERROR,				// OK
	ERR_ALREADY_CONNECT,
	ERR_INVALID_SERVER_NO,
	ERR_INVALID_ZONE_NO,
	ERR_SENDER_IS_EMPTY,		// �����̰� ����
	ERR_INVALID_ITEM_INDEX,
	ERR_INVALID_ITEM_COUNT,
	ERR_INVALID_NAS,
	ERR_INVALID_LEVEL,
};

struct authorize : public pTypeBase
{
	int			result;
};

struct sendItemToPlayingUser : public pTypeBase
{
	int			result;
};

struct sendItemToOneUser : public pTypeBase
{
	int			result;
};

//////////////////////////////////////////////////////////////////////////
#ifndef _CLIENT_
inline void makeAuthorize(CNetMsg::SP& msg, int result)
{
	authorize* packet = reinterpret_cast<authorize*>(msg->m_buf);
	msg->m_mtype = packet->type = MSG_EXPRESS_SYSTEM;
	packet->subType = MSG_SUB_AUTHORIZE;
	packet->result = result;
	msg->setSize(sizeof(authorize));
}

inline void makeSendToAllUser(CNetMsg::SP& msg, int result)
{
	sendItemToPlayingUser* packet = reinterpret_cast<sendItemToPlayingUser*>(msg->m_buf);
	msg->m_mtype = packet->type = MSG_EXPRESS_SYSTEM;
	packet->subType = MSG_SUB_EXPRESS_ALL_USER;
	packet->result = result;
	msg->setSize(sizeof(sendItemToPlayingUser));
}

inline void makeSendToServerGroup(CNetMsg::SP& msg, int result)
{
	sendItemToPlayingUser* packet = reinterpret_cast<sendItemToPlayingUser*>(msg->m_buf);
	msg->m_mtype = packet->type = MSG_EXPRESS_SYSTEM;
	packet->subType = MSG_SUB_EXPRESS_SERVER_GROUP;
	packet->result = result;
	msg->setSize(sizeof(sendItemToPlayingUser));
}

inline void makeSendToZone(CNetMsg::SP& msg, int result)
{
	sendItemToPlayingUser* packet = reinterpret_cast<sendItemToPlayingUser*>(msg->m_buf);
	msg->m_mtype = packet->type = MSG_EXPRESS_SYSTEM;
	packet->subType = MSG_SUB_EXPRESS_ZONE;
	packet->result = result;
	msg->setSize(sizeof(sendItemToPlayingUser));
}

inline void makeSendToOneUser(CNetMsg::SP& msg, int result)
{
	sendItemToOneUser* packet = reinterpret_cast<sendItemToOneUser*>(msg->m_buf);
	msg->m_mtype = packet->type = MSG_EXPRESS_SYSTEM;
	packet->subType = MSG_SUB_EXPRESS_ONE_USER;
	packet->result = result;
	msg->setSize(sizeof(sendItemToOneUser));
}
#endif
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)

#endif
