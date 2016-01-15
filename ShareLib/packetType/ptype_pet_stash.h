#ifndef __PTYPE_PET_STASH__
#define __PTYPE_PET_STASH__

#include "ptype_base.h"

#define MAX_PET_STASH_KEEP_COUNT	20
#define MAX_PET_PROXY_COUNT			1

#define PET							0
#define APET						1

//Packet SubType Client <-> Server
enum
{
	MSG_PET_STASH_LIST_UPDATE,
	MSG_PET_STASH_TAKE,
	MSG_PET_STASH_TAKE_UPDATE,
	MSG_PET_STASH_KEEP,
	MSG_PET_STASH_KEEP_UPDATE,
	MSG_PET_STASH_PROXY,
	MSG_PET_STASH_PROXY_UPDATE,
	MSG_PET_STASH_PROXY_CANCEL,
	MSG_PET_STASH_PROXY_CANCEL_UPDATE,
	MSG_PET_STASH_EFFECT,
	MSG_PET_STASH_EFFECT_BY_INMAP,
	MSG_PET_STASH_EFFECT_BROAD_CAST,
	MSG_PET_STASH_PET_UPDATE,
	MSG_PET_STASH_REQUEST_EFFECT_BROADCAST,
	MSG_PET_STASH_ERROR,
};

#pragma pack(push, 1)

namespace RequestClient
{
struct PetStashTake : public pTypeBase
{
	int charIndex;
	int petItemVIndex;								//�� ������ ���� �ε���
};

struct PetStashKeep : public pTypeBase
{
	int charIndex;
	int petItemVIndex;
};

struct PetStashProxyReg : public pTypeBase
{
	int petItemVIndex;
};

struct PetStashProxyCancel : public pTypeBase
{
	int petItemVIndex;
};

struct PetStashRequestEffectBroadcast : public pTypeBase
{
	int effectNo;
};
}


namespace ResponseClient
{
enum
{
	ERROR_PET_STASH_KEEP_SUCCESS,					//�ñ�� ����
	ERROR_PET_STASH_TAKE_SUCCESS,					//ã�� ����
	ERROR_PET_STASH_PROXY_SUCCESS,					//�븮���� ��� ����
	ERROR_PET_STASH_PROXY_CANCEL_SUCCESS,			//�븮���� ��� ��� ����

	ERROR_PET_STASH_KEEP_FAIL_NOT_EXIST_PET,		//���� �������� ����
	ERROR_PET_STASH_KEEP_FAIL_TERMINATION_ITEM,		//�� ��� ������ ���Ⱓ ����
	ERROR_PET_STASH_KEEP_FAIL_ALREADY_EQ_PET,		//���� �������̴�.
	ERROR_PET_STASH_KEEP_FAIL_PET_SEAL,				//���� ���λ����̴�.
	ERROR_PET_STASH_KEEP_FAIL_FULL,					//�� â�� ���� ��.

	ERROR_PET_STASH_TAKE_FAIL_NOT_EXIST_PET,		//���� �������� ����
	ERROR_PET_STASH_TAKE_FAIL,						//�� ã�� ����

	ERROR_PET_STASH_PROXY_CANCEL_NOT_EXIST,			//�븮�������� ��ϵ� ���� �ƴϴ�.

	ERROR_PET_STASH_ERROR_NOT_USE_SYSTEM,			//�� â�� �ý����� ����� �� �����ϴ�.

	ERROR_PET_STASH_FAIL_ETC,						//�˼� ���� ����
};

struct PetStashTake : public pTypeBase
{
	int result;
};

struct PetStashKeep : public pTypeBase
{
	int result;
};

struct PetStashProxyReg : public pTypeBase
{
	int result;
};

struct PetStashProxyCancel : public pTypeBase
{
	int result;
};

struct PetStashError : public pTypeBase
{
	int errorCode;
};
}

namespace UpdateClient
{
struct PetStashData
{
	int			virtualIndex;		// ������ ������ ���� index
	int			dbIndex;			// DB���� Index
	int			isPetOrApet;		// 0 : ��1, 1 : ��2
	int			plus;
	int			petLevel;
	int			petHungryMax;
	int			petHungry;
	LONGLONG	petLevelupExp;
	LONGLONG	petExp;
	int			petFaithMax;
	int			petFaith;
	//�길 ���
	int			hp;
	int			maxHp;
	int			abilityPoint;
	char		grade;
	//�����길 ���
	int			petStr;
	int			petInt;
	int			petDex;
	int			petCon;
	LONGLONG	petAccExp;
	int			remainTime;
	int			petPlusStr;
	int			petPlusInt;
	int			petPlusDex;
	int			petPlusCon;
	//
	char		petName[MAX_CHAR_NAME_LENGTH];
};

struct PetStashList : public pTypeBase
{
	int			proxyPetItemVIndex;	//�븮���� ��� �� ������ ���� �ε���
	short		count;
	PetStashData petStashData[0];
};

struct PetStashTake : public pTypeBase
{
	int petItemVIndex;								//�� ������ ���� �ε���
};

struct PetStashKeep : public pTypeBase
{
	PetStashData petStashData;
};

struct PetStashPetData : public pTypeBase
{
	PetStashData petStashData;
};

struct PetStashProxyReg : public pTypeBase
{
	int virtualIndex;
};

struct PetStashProxyCancel : public pTypeBase
{
	int virtualIndex;
};

struct PetStashEffect : public pTypeBase // Ŭ�� �������� ������
{
	int effectNo;
};

struct PetStashEffectByInMap : public pTypeBase // �θ� or ��â�� �Ⱓ �����϶�
{
	int effectNo;
};

struct PetStashEffectBr : public pTypeBase
{
	int charIndex;
	int effectNo;
};
}

#pragma pack(pop)

#ifndef _CLIENT_

inline void makePetStashErrorMsg(CNetMsg::SP &msg, int errorCode)
{
	ResponseClient::PetStashError *packet = reinterpret_cast<ResponseClient::PetStashError*>(msg->m_buf);
	packet->type = MSG_PET_STASH;
	packet->subType = MSG_PET_STASH_ERROR;
	packet->errorCode = errorCode;
	msg->setSize(sizeof(ResponseClient::PetStashError));
}

inline void makePetStashTakeMsg(CNetMsg::SP& msg, int errorCode)
{
	ResponseClient::PetStashTake *packet = reinterpret_cast<ResponseClient::PetStashTake*>(msg->m_buf);
	packet->type = MSG_PET_STASH;
	packet->subType = MSG_PET_STASH_TAKE;
	packet->result = errorCode;
	msg->setSize(sizeof(ResponseClient::PetStashTake));
}

inline void makePetStashTakeUpdateMsg(CNetMsg::SP& msg, int petVIndex)
{
	UpdateClient::PetStashTake *packet = reinterpret_cast<UpdateClient::PetStashTake*>(msg->m_buf);
	packet->type = MSG_PET_STASH;
	packet->subType = MSG_PET_STASH_TAKE_UPDATE;
	packet->petItemVIndex = petVIndex;
	msg->setSize(sizeof(UpdateClient::PetStashTake));
}

inline void makePetStashKeepMsg(CNetMsg::SP& msg, int errorCode)
{
	ResponseClient::PetStashKeep *packet = reinterpret_cast<ResponseClient::PetStashKeep*>(msg->m_buf);
	packet->type = MSG_PET_STASH;
	packet->subType = MSG_PET_STASH_KEEP;
	packet->result = errorCode;
	msg->setSize(sizeof(ResponseClient::PetStashKeep));
}

inline void makePetStashProxyRegMsg(CNetMsg::SP& msg, int errorCode)
{
	ResponseClient::PetStashProxyReg *packet = reinterpret_cast<ResponseClient::PetStashProxyReg*>(msg->m_buf);
	packet->type = MSG_PET_STASH;
	packet->subType = MSG_PET_STASH_PROXY;
	packet->result = errorCode;
	msg->setSize(sizeof(ResponseClient::PetStashProxyReg));
}

inline void makePetStahsProxyRegUpdateMsg(CNetMsg::SP& msg, int petVIndex)
{
	UpdateClient::PetStashProxyReg* packet = reinterpret_cast<UpdateClient::PetStashProxyReg*>(msg->m_buf);
	packet->type = MSG_PET_STASH;
	packet->subType = MSG_PET_STASH_PROXY_UPDATE;
	packet->virtualIndex = petVIndex;
	msg->setSize(sizeof(UpdateClient::PetStashProxyReg));
}

inline void makePetStashProxyCancelMsg(CNetMsg::SP& msg, int errorCode)
{
	ResponseClient::PetStashProxyCancel *packet = reinterpret_cast<ResponseClient::PetStashProxyCancel*>(msg->m_buf);
	packet->type = MSG_PET_STASH;
	packet->subType = MSG_PET_STASH_PROXY_CANCEL;
	packet->result = errorCode;
	msg->setSize(sizeof(ResponseClient::PetStashProxyCancel));
}

inline void makePetStashProxyCancelUpdateMsg(CNetMsg::SP& msg, int petVIndex)
{
	UpdateClient::PetStashProxyCancel *packet = reinterpret_cast<UpdateClient::PetStashProxyCancel*>(msg->m_buf);
	packet->type = MSG_PET_STASH;
	packet->subType = MSG_PET_STASH_PROXY_CANCEL_UPDATE;
	packet->virtualIndex = petVIndex;
	msg->setSize(sizeof(UpdateClient::PetStashProxyCancel));
}

inline void makePetStashEffectMsg(CNetMsg::SP& msg, int effectNo)
{
	UpdateClient::PetStashEffect *packet = reinterpret_cast<UpdateClient::PetStashEffect*>(msg->m_buf);
	packet->type = MSG_PET_STASH;
	packet->subType = MSG_PET_STASH_EFFECT;
	packet->effectNo = effectNo;
	msg->setSize(sizeof(UpdateClient::PetStashEffect));
}

inline void makePetStashEffectMsgByInmap(CNetMsg::SP& msg, int effectNo)
{
	UpdateClient::PetStashEffectByInMap *packet = reinterpret_cast<UpdateClient::PetStashEffectByInMap*>(msg->m_buf);
	packet->type = MSG_PET_STASH;
	packet->subType = MSG_PET_STASH_EFFECT_BY_INMAP;
	packet->effectNo = effectNo;
	msg->setSize(sizeof(UpdateClient::PetStashEffect));
}

inline void makePetStashEffectBrMsg(CNetMsg::SP& msg, int effectNo, int charIndex)
{
	UpdateClient::PetStashEffectBr *packet = reinterpret_cast<UpdateClient::PetStashEffectBr*>(msg->m_buf);
	packet->type = MSG_PET_STASH;
	packet->subType = MSG_PET_STASH_EFFECT_BROAD_CAST;
	packet->charIndex = charIndex;
	packet->effectNo = effectNo;
	msg->setSize(sizeof(UpdateClient::PetStashEffectBr));
}

inline void makePetStashPetUpdateMsg(CNetMsg::SP& msg)
{
	UpdateClient::PetStashPetData *packet = reinterpret_cast<UpdateClient::PetStashPetData*>(msg->m_buf);
	packet->type = MSG_PET_STASH;
	packet->subType = MSG_PET_STASH_PET_UPDATE;
	msg->setSize(sizeof(UpdateClient::PetStashPetData));
}
#endif

#endif
