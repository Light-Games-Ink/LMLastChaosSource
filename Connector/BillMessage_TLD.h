#ifndef __BILL_MESSAGE_H__
#define __BILL_MESSAGE_H__

typedef enum __tagBillMessage
{
	MSG_BILL_UNKNOWN	= 0,						// �߸��� �޼���

	BM_CONNECT,								// Ŀ���� ���� ��û: protocolversion(str:17) serverNum(uc) gameId(uc)
	// protocolversion = "TEnterBillV1.0"

	MSG_BILLITEM_CASHBALANCE_REQ,			// ����� �ܾ׿�û : gameId(uc) userindex(n) userId(str:32) cpId(n)
	MSG_BILLITEM_CASHBALANCE_REP,			// ����	    	   : returnCode(uc) gameId(uc) userindex(n) cpId(n) cashBalance(int)

	MSG_BILLITEM_CANBUY_REQ,				// ����������û    : gameId(uc) userindex(n) userId(str:32) cpId(n) count(uc) itemId(n:count) requestCash(n)
	MSG_BILLITEM_CANBUY_REP,				// ����			   : returnCode(uc) gameId(uc) userindex(n) cpId(n) count(uc) itemId(n:count) cashBalance(int)

	MSG_BILLITEM_BUY_REQ,					// ���� ��û	   : serial(n) gameId(uc) userindex(n) userId(str:32) cpId(n) ip(str:30) count(uc) itemId(n:count)
	MSG_BILLITEM_BUY_REP,					// ����			   : serial(n) returnCode(uc) gameId(uc) userindex(n) cpId(n) count(uc) itemId(n:count) cashBalance(int)

	MSG_BILLITEM_SERVERLIVE,				// ���� ����̺�

	// SOAPHelper ��
	MSG_SOAP_CONNECT_REQ,					// SOAPHelper�� ���� ��û : version(n) server_type(uc) server_no(n)
	// server_type: 1-�α��μ���, 2-������
	MSG_SOAP_CONNECT_REP,					// SOAPHelper�� ���� ��û : errorcode(n)
	// errorcode: MSG_SOAP_LOGIN_ERROR_TYPE
	MSG_SOAP_USER_AUTH_CODE_REQ	, 			// LOGIN_SERVER->SOAP ���� �н����� �ؽ� ��û		: key(n) user_id(str)
	MSG_SOAP_USER_AUTH_CODE_REP	, 			// SOAP->LOGIN_SERVER ���� �н����� �ؽ� ����		: key(n) password_hash(str)

	MSG_BILLITEM_RELOAD_CASH,				// ĳ�������� ���� ���� ���ε�
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
	MSG_BILLITEM_ERROR_NOTACCOUNT	= 12,	// ���� ���� ����
} BILL_ERROR_MESSAGE_TYPE;

typedef enum __tagMsgSOAPConnectErrorType
{
	MSG_SOAP_CONNECT_ERROR_OK				, // ����
	MSG_SOAP_CONNECT_ERROR_VERSION			, // ���� ����
	MSG_SOAP_CONNECT_ERROR_SERVER_TYPE		, // ���� Ÿ�� ����
	MSG_SOAP_CONNECT_ERROR_SERVER_NO		, // ���� ��ȣ ���� (�߸��� �� �Ǵ� �ߺ�)
	MSG_SOAP_CONNECT_ERROR_ERROR			, // �׿� ����
} MSG_SOAP_CONNECT_ERROR_TYPE;

#endif // __BILL_MESSAGE_H__
//
