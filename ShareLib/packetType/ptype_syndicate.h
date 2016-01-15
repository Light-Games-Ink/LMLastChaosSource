#ifndef __PTYPE_SYNDICATE_H__
#define __PTYPE_SYNDICATE_H__

#include "ptype_base.h"

//Packet SubType Client <-> Server
enum
{
	MSG_RVR_REMOVE_REQ,						//Ż�� ��û
	MSG_RVR_GIVE_JEWEL_REQ,					//���� ���
	MSG_RVR_SYNDICATE_INFO_USER_REQ,		//���� ���� ���� ��û
	MSG_RVR_SYNDICATE_INFO_SERVER_REQ,		//���� ���� ��û

	MSG_RVR_ERROR_REP,						//���� �����ڵ�
	MSG_RVR_SYNDICATE_INFO_USER_REP,		//���� ���� ���� ����

	MSG_RVR_SYNDICATE_INFO_SERVER,			//���� ���� ����
	MSG_UPDATE_RVR_SYNDICATE_TYPE,			//���� ���� ������Ʈ
	MSG_UPDATE_RVR_SYNDICATE_GRADE,			//���� ��� ������Ʈ
	MSG_UPDATE_RVR_CONTRIBUTIONS_POINT,		//���� ����Ʈ ������Ʈ
	MSG_UPDATE_SYNDICATE_HISTORY_LOG,		//���� �����丮 �α� ������Ʈ
	MSG_UPDATE_SYNDICATE_PERCENTAGE,		//���� ���� ������Ʈ
	MSG_UPDATE_KINGNAME,					//�� ���� ������Ʈ

	SYSMSG_INCREASE_POINT,					//����Ʈ ���� �ý��� �޽���

	MSG_UPDATE_OUT_NEEDNAS,					//���� Ż��� �ʿ��� ��������
};

enum
{
	JOIN,			// ����
	REMOVE,			// Ż��
	DEGRADE,		// ����
	PROMOTION,		// �±�
	WIN,			// �¸�
	LOSE,			// ����
};

#pragma pack(push, 1)

namespace RequestClient
{
//Ż�� ��û
struct RVRRemoveRequest : public pTypeBase
{
	int	tab;
	int invenIndex;
	int npcIndex;		// Ż�� NPC
};

//â���� ���� ��ο�û
struct RVRGiveJewel : public pTypeBase
{
	int				npcIndex;				// â���� ���� give NPC
	unsigned int	vIndexByCreateJewel;	// â���� ���� v-index
};

struct RVRServerInfoReq : public pTypeBase
{
	int				syndicateType;			// ������ ��ſ� ���(Ŭ���̾�Ʈ�� ���� �ʿ� ����)
	int				grade;					// ������ ��ſ� ���(Ŭ���̾�Ʈ�� ���� �ʿ� ����)
};

#ifndef _CLIENT_
inline void makeReqSyndicateServerInfoMsg(CNetMsg::SP& msg, int syndicateType, int grade)
{
	RequestClient::RVRServerInfoReq* packet = reinterpret_cast<RequestClient::RVRServerInfoReq*>(msg->m_buf);
	packet->type = MSG_HELPER_COMMAND_STRUCT;
	packet->subType = MSG_HELPER_RVR_RVR_INFO;
	packet->syndicateType = syndicateType;
	packet->grade = grade;
	msg->setSize(sizeof(RequestClient::RVRServerInfoReq));
}
#endif
}

namespace ResponseClient
{
enum
{
	ERROR_SYNDICATE_JOIN_SUCCESS,					// ���� ���� ����
	ERROR_SYNDICATE_REMOVE_SUCCESS,					// ���� Ż�� ����
	ERROR_SYNICATE_GIVE_SUCCESS,					// ���� â���� ���� ��� ����
	ERROR_SYNDICATE_REMOVE_FAIL_NOT_ENOUGH_MONEY,	// Ż���ϱ� ���� ���� ����
	ERROR_SYNDICATE_REMOVE_FAIL_ALREADY_EQUIP,		// ���� ���� ��� ���� ���̴�.
	ERROR_SYNDICATE_BUSY,							// ���� ������ ȥ���մϴ�.
};

//���� ���� ����
struct RVRSyndicateInfoOfUser : public pTypeBase
{
	int			syndicateType;								//���� ���� Ÿ��
	int			grade;										//���
	LONGLONG	accumulateContributionPoint;				//pc�� ���� �⿩��
	LONGLONG	maxPoint;									//��� �� �ִ� ��ġ
};

//error response
struct SyndicateError  : public pTypeBase
{
	int result;
	int syndicateType;
};

#ifndef _CLIENT_
inline void makeSyndicateErrorMsg(CNetMsg::SP& msg, int errorCode, int syndicateType)
{
	ResponseClient::SyndicateError* packet = reinterpret_cast<ResponseClient::SyndicateError*>(msg->m_buf);
	packet->type			= MSG_RVR;
	packet->subType			= MSG_RVR_ERROR_REP;
	packet->result			= errorCode;
	packet->syndicateType	= syndicateType;
	msg->setSize(sizeof(ResponseClient::SyndicateError));
}
#endif
}

namespace UpdateClient
{
struct SyndicateContributePoint : public pTypeBase
{
	int syndicateType;
	bool isFirstJoin;
	LONGLONG accumulateContributionPoint;	// pc�� ���� �⿩��
};

struct SyndicateKind : public pTypeBase
{
	int charIndex;
	int syndicateType;
	int grade;
};

struct SyndicateHistoryData : public pTypeBase
{
	struct historys
	{
		int syndicateType;
		int time;
		int grade;
		int type;
		int targetSyndicateType;
		char targetName[MAX_CHAR_NAME_LENGTH + 1];
	};

	int count;
	historys	data[0];
};

struct SyndicatePercentageData : public pTypeBase
{
	int syndicateType;
	float percentage;
};

struct RVRSyndicateInfoOfServer : public pTypeBase
{
	int syndicateType;
	char kingName[MAX_CHAR_NAME_LENGTH + 1];			//�뻧 �̸�
	int skillIndex;									//���� �� �ִ� ���� �ε���
	int skillLevel;									//���� �� �ִ� ���� ����
};

struct SyndicateKingNameUpdate : public pTypeBase
{
	int syndicateType;
	char kingName[MAX_CHAR_NAME_LENGTH + 1];
};

struct SyndicateIncreasePoint : public pTypeBase
{
	int syndicateType;
	LONGLONG point;
};

struct SyndicateToNeedNasOfOut : public pTypeBase
{
	GoldType_t needNas;
};

#ifndef _CLIENT_
inline void makeUpdateSyndicateContributePoint(CNetMsg::SP& msg, int syndicateType, bool isFirstJoin, LONGLONG point)
{
	UpdateClient::SyndicateContributePoint* packet = reinterpret_cast<UpdateClient::SyndicateContributePoint*>(msg->m_buf);
	packet->type			= MSG_RVR;
	packet->subType			= MSG_UPDATE_RVR_CONTRIBUTIONS_POINT;
	packet->syndicateType	= syndicateType;
	packet->isFirstJoin		= isFirstJoin;
	packet->accumulateContributionPoint = point;
	msg->setSize(sizeof(UpdateClient::SyndicateContributePoint));
}

inline void makeUpdateSyndicateKind(CNetMsg::SP& msg, int syndicateType, int grade, int charIndex)
{
	UpdateClient::SyndicateKind* packet = reinterpret_cast<UpdateClient::SyndicateKind*>(msg->m_buf);
	packet->type			= MSG_RVR;
	packet->subType			= MSG_UPDATE_RVR_SYNDICATE_GRADE;
	packet->charIndex		= charIndex;
	packet->syndicateType	= syndicateType;
	packet->grade			= grade;
	msg->setSize(sizeof(UpdateClient::SyndicateKind));
}

inline void makeUpdateSyndicateIncreaseSysMsg(CNetMsg::SP& msg, int syndicateType, LONGLONG point)
{
	UpdateClient::SyndicateIncreasePoint* packet = reinterpret_cast<UpdateClient::SyndicateIncreasePoint*>(msg->m_buf);
	packet->type	      = MSG_RVR;
	packet->subType		  = SYSMSG_INCREASE_POINT;
	packet->syndicateType = syndicateType;
	packet->point         = point;
	msg->setSize(sizeof(UpdateClient::SyndicateIncreasePoint));
}

inline void makeUpdateSyndicateOutNeedNas(CNetMsg::SP& msg, GoldType_t nas)
{
	UpdateClient::SyndicateToNeedNasOfOut* packet = reinterpret_cast<UpdateClient::SyndicateToNeedNasOfOut*>(msg->m_buf);
	packet->type    = MSG_RVR;
	packet->subType = MSG_UPDATE_OUT_NEEDNAS;
	packet->needNas = nas;
	msg->setSize(sizeof(UpdateClient::SyndicateToNeedNasOfOut));
}
#endif
}

#ifndef _CLIENT_
namespace UpdateServer
{
struct SyndicateUpdateServerInfo : public pTypeServer
{
	LONGLONG	first;
	LONGLONG	second;
};

struct SyndicateJewelCount : public pTypeServer
{
	int syndicateType;
	int count;
};

struct SyndicateUser : public pTypeServer
{
	int syndicateType;
};

struct SyndicateInfo : public pTypeServer
{
	int	jewelCount_k;
	int jewelCount_d;
	int userCount_k;
	int userCount_d;
};

struct SyndicateUpdateKingInfoAll : public pTypeServer
{
	char kingName_d[MAX_CHAR_NAME_LENGTH + 1];
	int kingIndex_d;
	LONGLONG	kingPoint_d;
	char kingName_k[MAX_CHAR_NAME_LENGTH + 1];
	int kingIndex_k;
	LONGLONG kingPoint_k;
};

struct SyndicateUpdateKingInfoEach : public pTypeServer
{
	int syndicateType;
	int kingIndex;
	LONGLONG kingPoint;
	char kingName[MAX_CHAR_NAME_LENGTH + 1];
};

struct SyndicateAdd2ndGradeUser : public pTypeServer
{
	int syndicateType;
	int charIndex;
	LONGLONG point;
	char userName[MAX_CHAR_NAME_LENGTH + 1];
};

struct SyndicateDel2ndGradeUser : public pTypeServer
{
	int syndicateType;
	int charIndex;
};

struct SyndicateUpdate2ndGradeUser : public pTypeServer
{
	int syndicateType;
	int charIndex;
	LONGLONG point;
};

struct SyndicateKingDownGrade : public pTypeServer
{
	int syndicateType;
	int charIndex;
	LONGLONG point;
};

struct SyndicateUserCount : public pTypeServer
{
	int syndicateType;
	int userCount;
};

inline void HelperServerRVRAdd2ndUser(CNetMsg::SP& msg, int syndicateType, int charIndex, LONGLONG point, const char* name)
{
	UpdateServer::SyndicateAdd2ndGradeUser* packet = reinterpret_cast<UpdateServer::SyndicateAdd2ndGradeUser*>(msg->m_buf);
	packet->type = MSG_HELPER_COMMAND_STRUCT;
	packet->subType = MSG_HELPER_ADD_2ND_USER;
	packet->syndicateType = syndicateType;
	packet->charIndex = charIndex;
	packet->point = point;
	strncpy(packet->userName, name, MAX_CHAR_NAME_LENGTH);
	packet->userName[MAX_CHAR_NAME_LENGTH] = '\0';
	msg->setSize(sizeof(UpdateServer::SyndicateAdd2ndGradeUser));
}

inline void HelperServerRVRDel2ndUser(CNetMsg::SP& msg, int syndicateType, int charIndex)
{
	UpdateServer::SyndicateDel2ndGradeUser* packet = reinterpret_cast<UpdateServer::SyndicateDel2ndGradeUser*>(msg->m_buf);
	packet->type = MSG_HELPER_COMMAND_STRUCT;
	packet->subType = MSG_HELPER_DEL_2ND_USER;
	packet->syndicateType = syndicateType;
	packet->charIndex = charIndex;
	msg->setSize(sizeof(UpdateServer::SyndicateDel2ndGradeUser));
}

inline void HelperServerRVRUpdate2ndUser(CNetMsg::SP& msg, int syndicateType, int charIndex, LONGLONG point)
{
	UpdateServer::SyndicateUpdate2ndGradeUser* packet = reinterpret_cast<UpdateServer::SyndicateUpdate2ndGradeUser*>(msg->m_buf);
	packet->type = MSG_HELPER_COMMAND_STRUCT;
	packet->subType = MSG_HELPER_UPDATE_2ND_USER;
	packet->syndicateType = syndicateType;
	packet->charIndex = charIndex;
	packet->point = point;
	msg->setSize(sizeof(UpdateServer::SyndicateUpdate2ndGradeUser));
}

inline void HelperServerRVRKingDownGrade(CNetMsg::SP& msg, int syndicateType, int charIndex, LONGLONG point)
{
	UpdateServer::SyndicateKingDownGrade* packet = reinterpret_cast<UpdateServer::SyndicateKingDownGrade*>(msg->m_buf);
	packet->type = MSG_HELPER_COMMAND_STRUCT;
	packet->subType = MSG_HELPER_DOWN_GRADE;
	packet->syndicateType = syndicateType;
	packet->charIndex = charIndex;
	packet->point = point;
	msg->setSize(sizeof(UpdateServer::SyndicateKingDownGrade));
}

inline void HelperServerRVRIncreaseCreateJewel(CNetMsg::SP& msg, int syndicateType, int count)
{
	UpdateServer::SyndicateJewelCount* packet = reinterpret_cast<UpdateServer::SyndicateJewelCount*>(msg->m_buf);
	packet->type = MSG_HELPER_COMMAND_STRUCT;
	packet->subType = MSG_HELPER_RVR_INCREASE_JEWEL_REQ;
	packet->syndicateType = syndicateType;
	packet->count = count;
	msg->setSize(sizeof(UpdateServer::SyndicateJewelCount));
}

inline void HelperServerRVRDecreaseCreateJewel(CNetMsg::SP& msg, int syndicateType, int count)
{
	UpdateServer::SyndicateJewelCount* packet = reinterpret_cast<UpdateServer::SyndicateJewelCount*>(msg->m_buf);
	packet->type = MSG_HELPER_COMMAND_STRUCT;
	packet->subType = MSG_HELPER_RVR_DECREASE_JEWEL_REQ;
	packet->syndicateType = syndicateType;
	packet->count = count;
	msg->setSize(sizeof(UpdateServer::SyndicateJewelCount));
}

inline void HelperServerRVRJoinRequest(CNetMsg::SP& msg, int syndicateType)
{
	UpdateServer::SyndicateUser* packet = reinterpret_cast<UpdateServer::SyndicateUser*>(msg->m_buf);
	packet->type = MSG_HELPER_COMMAND_STRUCT;
	packet->subType = MSG_HELPER_RVR_JOIN_REQ;
	packet->syndicateType = syndicateType;
	msg->setSize(sizeof(UpdateServer::SyndicateUser));
}

inline void HelperServerRVRLeaveRequest(CNetMsg::SP& msg, int syndicateType)
{
	UpdateServer::SyndicateUser* packet = reinterpret_cast<UpdateServer::SyndicateUser*>(msg->m_buf);
	packet->type = MSG_HELPER_COMMAND_STRUCT;
	packet->subType = MSG_HELPER_RVR_LEAVE_REQ;
	packet->syndicateType = syndicateType;
	msg->setSize(sizeof(UpdateServer::SyndicateUser));
}

inline void HelperServerRVRChangeKing(CNetMsg::SP& msg, int syndicateType, const char* kingName, int kingIndex, LONGLONG kingPoint)
{
	UpdateServer::SyndicateUpdateKingInfoEach* packet = reinterpret_cast<UpdateServer::SyndicateUpdateKingInfoEach*>(msg->m_buf);
	packet->type = MSG_HELPER_COMMAND_STRUCT;
	packet->subType = MSG_HELPER_KING_INFO_UPDATE_EACH;
	packet->syndicateType = syndicateType;
	packet->kingIndex = kingIndex;
	packet->kingPoint = kingPoint;
	strncpy(packet->kingName, kingName, MAX_CHAR_NAME_LENGTH);
	packet->kingName[MAX_CHAR_NAME_LENGTH] = '\0';
	msg->setSize(sizeof(UpdateServer::SyndicateUpdateKingInfoEach));
}

inline void HelperServerRvRSetUserCount(CNetMsg::SP& msg, int syndicateType, int count)
{
	UpdateServer::SyndicateUserCount* packet = reinterpret_cast<UpdateServer::SyndicateUserCount*>(msg->m_buf);
	packet->type = MSG_HELPER_COMMAND_STRUCT;
	packet->subType = MSG_HELPER_SET_USER_COUNT;
	packet->syndicateType = syndicateType;
	packet->userCount = count;
	msg->setSize(sizeof(UpdateServer::SyndicateUserCount));
}
}
#endif
#pragma pack(pop)
#endif // __PTYPE_SYNDICATE__
