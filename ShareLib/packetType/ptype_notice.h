#ifndef __PTYPE_NOTICE_H__
#define __PTYPE_NOTICE_H__

#include <string.h>
#include "ptype_base.h"


enum MsgExNoticeThirdType
{
	MSG_EX_NOTICE_LIST,								// ���� ����Ʈ ��û
};


#pragma pack(push, 1)
struct NoticeData
{
	int noticeRev;				//������ ��ȣ
	char noticeString[107];		//���� ��Ʈ��
	int date;					//�ð�
};

namespace RequestClient
{
	//���� ������ ��û
	struct NoticeList : public pTypeThirdBase
	{
		int pageNum;
	};
}

namespace ResponseClient
{
	//���� ������ ����
	struct NoticeList : public pTypeThirdBase
	{
		bool isNextPage;
		int count;
		NoticeData data[0];
	};
}

namespace UpdateClient
{
	//���� ������ ������ ������Ʈ (Ŭ���̾�Ʈ�� �޾Ƽ� üũ �� ������ ���� �Ǵ�)
	struct checkNotice : public pTypeThirdBase
	{
		int revNum;
	};

	struct checkPopupNotice : public pTypeThirdBase
	{
		int html_num;
	};

	struct closePopupNotice : public pTypeThirdBase
	{
		int html_num;
	};

	struct popupNoticeClear : public pTypeThirdBase
	{};
}
#pragma pack(pop)

#ifndef _CLIENT_
	inline void CheckNoticeMsg(CNetMsg::SP &rmsg, int revNum)
	{
		UpdateClient::checkNotice* packet = reinterpret_cast<UpdateClient::checkNotice*>(rmsg->m_buf);
		packet->type = MSG_UPDATE_DATA_FOR_CLIENT;
		packet->subType = MSG_SUB_UPDATE_CHECK_NOTICE;	//129
		packet->revNum = revNum;
		rmsg->setSize(sizeof(UpdateClient::checkNotice));
	}

	inline void CheckPopupNoticeMsg(CNetMsg::SP &rmsg, int html_num)
	{
		UpdateClient::checkPopupNotice* packet = reinterpret_cast<UpdateClient::checkPopupNotice*>(rmsg->m_buf);
		packet->type = MSG_UPDATE_DATA_FOR_CLIENT;
		packet->subType = MSG_SUB_UPDATE_CHECK_POPUP_NOTICE;	//130
		packet->html_num = html_num;
		rmsg->setSize(sizeof(UpdateClient::checkPopupNotice));
	}

	inline void ClosePopupNoticeMsg(CNetMsg::SP &rmsg, int html_num)
	{
		UpdateClient::closePopupNotice* packet = reinterpret_cast<UpdateClient::closePopupNotice*>(rmsg->m_buf);
		packet->type = MSG_UPDATE_DATA_FOR_CLIENT;
		packet->subType = MSG_SUB_UPDATE_CLOSE_POPUP_NOTICE;	//131
		packet->html_num = html_num;
		rmsg->setSize(sizeof(UpdateClient::closePopupNotice));
	}

	inline void PopupNoticeClearMsg(CNetMsg::SP &rmsg)
	{
		UpdateClient::popupNoticeClear* packet = reinterpret_cast<UpdateClient::popupNoticeClear*>(rmsg->m_buf);
		packet->type = MSG_UPDATE_DATA_FOR_CLIENT;
		packet->subType = MSG_SUB_UPDATE_CLEAR_POPUP_NOTICE;
		rmsg->setSize(sizeof(UpdateClient::popupNoticeClear));
	}
#endif

#endif