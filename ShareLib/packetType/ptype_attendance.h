#ifndef __PTYPE_ATTENDANCE_H__
#define __PTYPE_ATTENDANCE_H__

#include "ptype_base.h"

//subtype
enum
{
	//Requset
	MSG_SUB_ATTENDANCE_CHECK_REQ,
	MSG_SUB_ATTENDANCE_REWARD_REQ,
	MSG_SUB_ATTENDANCE_ASSURE_CANCEL_OK_REQ,	//���� ������ ��� ���� �ϰڴ�

	//Response
	MSG_SUB_ATTENDANCE_CHECK_RES,				//���� RESPONSE 1��
	MSG_SUB_ATTENDANCE_REWARD_RES,				//���� RESPONSE 3��

	//Update
	MSG_SUB_ATTENDANCE_FINISH,					//���� RESPONSE 2��
	MSG_SUB_ATTENDANCE_INFO_UPD,				//���� RESPONSE 0��
	MSG_SUB_ATTENDANCE_ASSURE_CANCEL_OK_UPD,	//���� ������ ��� ���� �ϰڴ�?
	MSG_SUB_ATTENDANCE_SYS_MSG,					//�⼮ �ý��� �޽���
};

//error code
enum
{
	ATTENDANCE_ERROR_SUCCESS,
	ATTENDANCE_ERROR_NO_REWARD,
	ATTENDANCE_ERROR_INVEN_FULL,
};

//system message
enum
{
	ATTENDANCE_SYS_MSG_TIME_END = 1,					//�⼮ ���� ������ ��� �Ⱓ�� ����Ǿ����ϴ�.
	ATTENDANCE_SYS_MSG_END_OK = 2,						//�⼮ ���� ������ �����Ǿ����ϴ�. ���� �����⼮....
	ATTENDANCE_SYS_MSG_TIME_END_INIT_ATTENDANCE = 3,	//�⼮ ���� ������ ����� �� ���� �⼮�� ���� �ʾ� �⼮ �� ���� �ʱ�ȭ �Ǿ����ϴ�.
};

#pragma pack(push, 1)

namespace RequestClient
{
	struct AttendanceCheck : public pTypeThirdBase
	{
	};

	struct AttendanceReward : public pTypeThirdBase
	{
	};

	struct AttendanceAssureOk : public pTypeThirdBase
	{
	};
}

namespace ResponseClient
{
	struct AttendanceCheck : public pTypeThirdBase
	{
		int acc_count;
		int up_exp;
	};

	struct AttendanceReward : public pTypeThirdBase
	{
		int error_code;
	};

	struct AttendanceAssureOk : public pTypeThirdBase
	{
	};

#ifndef _CLIENT_
	inline void makeAttendanceCheckMsg(CNetMsg::SP& rmsg, int acc_count, int up_exp)
	{
		AttendanceCheck* packet = reinterpret_cast<AttendanceCheck*>(rmsg->m_buf);
		packet->type = MSG_EXTEND;
		packet->subType = MSG_EX_ATTENDANCE_EXP_SYSTEM;
		packet->thirdType = MSG_SUB_ATTENDANCE_CHECK_RES;
		packet->acc_count = acc_count;
		packet->up_exp = up_exp;
		rmsg->setSize(sizeof(AttendanceCheck));
	}

	inline void makeAttendanceRewardMsg(CNetMsg::SP& rmsg, int error_code)
	{
		AttendanceReward* packet = reinterpret_cast<AttendanceReward*>(rmsg->m_buf);
		packet->type = MSG_EXTEND;
		packet->subType = MSG_EX_ATTENDANCE_EXP_SYSTEM;
		packet->thirdType = MSG_SUB_ATTENDANCE_REWARD_RES;
		packet->error_code = error_code;
		rmsg->setSize(sizeof(AttendanceReward));
	}
#endif
}

namespace UpdateClient
{
	struct AttendanceFinish : public pTypeThirdBase
	{
		int acc_count;
		int up_exp;
	};

	struct AttendanceInfo : public pTypeThirdBase
	{
		int acc_count;
		int up_exp;
	};

	struct AttendanceAssureOk : public pTypeThirdBase
	{
	};

	struct AttendanceTimeEnd : public pTypeThirdBase
	{
	};

	struct AttendanceSysMsg : public pTypeThirdBase
	{
		int sysMsg;
	};

#ifndef _CLIENT_
	inline void makeAttendanceFinish(CNetMsg::SP& rmsg, int acc_count, int up_exp)
	{
		AttendanceFinish* packet = reinterpret_cast<AttendanceFinish*>(rmsg->m_buf);
		packet->type = MSG_EXTEND;
		packet->subType = MSG_EX_ATTENDANCE_EXP_SYSTEM;
		packet->thirdType = MSG_SUB_ATTENDANCE_FINISH;
		packet->acc_count = acc_count;
		packet->up_exp = up_exp;
		rmsg->setSize(sizeof(AttendanceFinish));
	}

	inline void makeAttendanceInfo(CNetMsg::SP& rmsg, int acc_count, int up_exp)
	{
		AttendanceInfo* packet = reinterpret_cast<AttendanceInfo*>(rmsg->m_buf);
		packet->type = MSG_EXTEND;
		packet->subType = MSG_EX_ATTENDANCE_EXP_SYSTEM;
		packet->thirdType = MSG_SUB_ATTENDANCE_INFO_UPD;
		packet->acc_count = acc_count;
		packet->up_exp = up_exp;
		rmsg->setSize(sizeof(AttendanceInfo));
	}

	inline void makeAttendanceAssureCancelOkMsg(CNetMsg::SP& rmsg)
	{
		AttendanceAssureOk* packet = reinterpret_cast<AttendanceAssureOk*>(rmsg->m_buf);
		packet->type = MSG_EXTEND;
		packet->subType = MSG_EX_ATTENDANCE_EXP_SYSTEM;
		packet->thirdType = MSG_SUB_ATTENDANCE_ASSURE_CANCEL_OK_UPD;
		rmsg->setSize(sizeof(AttendanceAssureOk));
	}

	inline void makeAttendanceSysMsg(CNetMsg::SP& rmsg, int sysMsg)
	{
		AttendanceSysMsg* packet = reinterpret_cast<AttendanceSysMsg*>(rmsg->m_buf);
		packet->type = MSG_EXTEND;
		packet->subType = MSG_EX_ATTENDANCE_EXP_SYSTEM;
		packet->thirdType = MSG_SUB_ATTENDANCE_SYS_MSG;
		packet->sysMsg = sysMsg;
		rmsg->setSize(sizeof(AttendanceSysMsg));
	}
#endif
}

#pragma pack(pop)

#endif