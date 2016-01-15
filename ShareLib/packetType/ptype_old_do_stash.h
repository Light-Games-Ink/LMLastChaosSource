#ifndef __PTYPE_OLD_DO_STASH_H__
#define __PTYPE_OLD_DO_STASH_H__

#include "ptype_base.h"

#define MIN_STASH_PASSWORD_LENGTH			(6)
#define MAX_STASH_PASSWORD_LENGTH			(8)

#ifdef STASH_PASSWORD
#define A_CID_LENGTH						(7)
#endif

enum
{
	MSG_STASH_ERROR,
	MSG_STASH_LIST,							// â�� ������ ����Ʈ ��û	: password(str)
	MSG_STASH_KEEP,							// â�� ���� �ñ��		: password(str) keepcount(n) [row(c) col(c) item_idx(n) count(ll)]:keepcount
	MSG_STASH_TAKE,							// ã�� ��û				: password(str) takecount(n) [item_idx(n) count(ll)]:takecount

#ifdef STASH_PASSWORD
	MSG_STASH_CHANGE_PASSWORD = 100,		// â�� �н����� ����
	MSG_STASH_DELETE_PASSWORD,				// ��ȣ ����
#endif
};

#pragma pack(push, 1)
//////////////////////////////////////////////////////////////////////////
namespace RequestClient
{
struct doStashList : public pTypeBase
{
#ifdef STASH_PASSWORD
	char					password[MAX_STASH_PASSWORD_LENGTH + 1];
#endif
};

struct doStashKeep : public pTypeBase
{
#ifdef STASH_PASSWORD
	char					password[MAX_STASH_PASSWORD_LENGTH + 1];
#endif
	int						npcKind;
	LONGLONG				keepMoney;
	unsigned short			keepCount;
	struct
	{
		unsigned short		tab;
		unsigned short		invenIndex;
		int					virtualIndex;
		int					count;
	} list[0];
};

struct doStashTake : public pTypeBase
{
#ifdef STASH_PASSWORD
	char					password[MAX_STASH_PASSWORD_LENGTH + 1];
#endif
	int						npcKind;
	LONGLONG				takeMoney;
	unsigned short			takeCount;
	struct
	{
		unsigned short		invenIndex;		// tab�� ������ INVENTORY_TAB_STASH���� �����Ѵ�.
		int					virtualIndex;
		int					count;
	} list[0];
};

#ifdef STASH_PASSWORD
struct doStashChangePassword : public pTypeBase
{
	char					old_password[MAX_STASH_PASSWORD_LENGTH + 1];
	char					new_password[MAX_STASH_PASSWORD_LENGTH + 1];
};

struct doStashDeletePassword : public pTypeBase
{
	char					identity[A_CID_LENGTH + 1];
};
#endif
}

//////////////////////////////////////////////////////////////////////////

namespace ResponseClient
{
#ifdef STASH_PASSWORD
struct doStashDeletePassword : public pTypeBase
{
	unsigned char		result;		// 0 - ok, 1 - error
};
#endif

#ifndef _CLIENT_
inline void StashErrorMsg(CNetMsg::SP& msg, MSG_STASH_ERROR_TYPE err)
{
	msg->Init(MSG_STASH);
	RefMsg(msg) << (unsigned char)MSG_STASH_ERROR
				<< (unsigned char)err;
}

#ifdef STASH_PASSWORD
inline void DeleteStashPasswordErrorMsg(CNetMsg::SP& msg, unsigned char result)
{
	doStashDeletePassword* packet = reinterpret_cast<doStashDeletePassword*>(msg->m_buf);
	packet->type = MSG_STASH;
	packet->subType = MSG_STASH_DELETE_PASSWORD;
	packet->result = result;
	msg->setSize(sizeof(doStashDeletePassword));
}
#endif

#endif
}
//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)

#endif
