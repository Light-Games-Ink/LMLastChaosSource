#ifndef __BILL_MESSAGE_H__
#define __BILL_MESSAGE_H__

typedef enum __tagBillMessage
{
	MSG_BILL_UNKNOWN	= 0,						// �߸��� �޼���

	BM_CONNECT = 1100,

	MSG_BILLITEM_CASHBALANCE_REQ = 1700,	// ����� �ܾ׿�û : gameId(uc) userId(str:32) cpId(n)
	MSG_BILLITEM_CASHBALANCE_REP = 2700,			// ����	    	   : returnCode(uc) gameId(uc) userId(str:32) cpId(n) cashBalance(int)

	MSG_BILLITEM_CANBUY_REQ = 1720,			// ����������û    : gmaeId(uc) userId(str:32) cpId(n) itemId(n)
	MSG_BILLITEM_CANBUY_REP = 2720,				// ����			   : returnCode(uc) gameId(uc) userId(str:32) cpId(n) itemId(n) cashBalance(int)

	MSG_BILLITEM_BUY_REQ = 1740,			// ���� ��û	   : gmaeId(uc) userId(str:32) cpId(n) itemId(n)
	MSG_BILLITEM_BUY_REP = 2740,					// ����			   : returnCode(uc) gameId(uc) userId(str:32) cpId(n) itemId(n) cashBalance(int)

	MSG_BILLITEM_COUPON_LIST_REQ = 1800,	// ���� ����Ʈ ��û : gameId(uc) userId(str:50) cpId(c) userIndex(n)
	MSG_BILLITEM_COUPON_LIST_REP = 2800,		// ����

	MSG_BILLITEM_BUYBYCOUPON_REQ = 1810,	// ���� ��� ���� ��û :
	MSG_BILLITEM_BUYBYCOUPON_REP = 2810,		// ����

	MSG_BILLITEM_SERVERLIVE = 4400,				// ���� ����̺�
} BILLMESSAGE_TYPE;

typedef enum __tagBillErrorMessage
{
	MSG_BILLITEM_ERROR_SUCCESS,				// ����
	MSG_BILLITEM_ERROR_LACKCASH,			// �ܾ׺���
	MSG_BILLITEM_ERROR_NOTUSER,				// �ش����� ����
	MSG_BILLITEM_ERROR_NOTITEM,				// �ش������ ����
	MSG_BILLITEM_ERROR_DATABASE,			// DB ����
	MSG_BILLITEM_ERROR_PACKET,				// ��Ŷ ����
	MSG_BILLITEM_ERROR_ETC,					// ��Ÿ ����
	MSG_BILLITEM_ERROR_NOTACCOUNT	= 12,			// ���� ���� ����
} BILL_ERROR_MESSAGE_TYPE;

#endif // __BILL_MESSAGE_H__
//
