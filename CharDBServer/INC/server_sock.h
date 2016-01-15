#ifndef _SERVER_SOCK_H_
#define _SERVER_SOCK_H_

#include "typedef.h"
#include <arpa/inet.h>
#define MAGIC_NUMBER            0x3812121281282828L

typedef struct _st_1stPktHdr
{
	INT64		llMagicNumber;     	/* 0x3812121281282828L */
	TIME_T64 	tCurTime;          	/* time_t + micro time */
	UINT64   	ullSessID;         	/* TEMS Session ID, �𸦶� 0x00 */
	USHORT   	usTotLen;          	/* sizeof(st_1stPktHdr) + sizeof(st_2ndPktHdr) + sizeof(data) */
	USHORT    	usSerial;			/* �ø��� ��ȣ, ���ο����� CSMS��ȣ�� ��� */
	UINT      	uiCRC;				/* CRC ��Ŷ�� �̻����� �Ǵ� */
} st_1stPktHdr, *pst_1stPktHdr;

#define FIRST_HEADER_LEN		sizeof(st_1stPktHdr)

typedef struct _st_2ndPktHdr
{
	USHORT    	usSvcID;			/* Service ID 	*/
	USHORT    	usMsgID;			/* Message ID */
	USHORT    	usResult;			/* Result */
	USHORT		usDestQKey;			/* Destination QID(Key) Insert HERE */
} st_2ndPktHdr, *pst_2ndPktHdr;

typedef struct _st_PktHdr
{
	st_1stPktHdr	st1stPH;
	st_2ndPktHdr	st2ndPH;
} st_PktHdr, *pst_PktHdr;

#define set_pheader_content( pstPH, sid, mid, totlen, result, sessid )	\
	(pstPH)->st1stPH.llMagicNumber = MAGIC_NUMBER;	\
	(pstPH)->st1stPH.ullSessID = sessid;				\
	(pstPH)->st1stPH.usTotLen = totlen;				\
	(pstPH)->st2ndPH.usResult = result;				\
	(pstPH)->st2ndPH.usSvcID = sid;					\
	(pstPH)->st2ndPH.usMsgID = mid;


#define PACK_HEADER_LEN        sizeof(st_PktHdr)

typedef struct _st_UDPHeader
{
	UCHAR	szStartPK[2];	//��Ŷ ������
	SHORT	usPKLen;		//��Ŷ ����
	SHORT	usSvcID;		//���� ���̵�
	SHORT	usMsgID;		//�޽��� ���̵�
	UINT64	ullSessID;		//�ڵ��� ��ȣ
	SHORT	usPKVer;		//��Ŷ ����
	SHORT	usResult;		//�����ڵ�
	UINT	uiMySeq;
	UINT	uiServSeq;
	UINT	uiReserved;
} st_UDPHeader, *pst_UDPHeader;


#define USOCK_HEADER_LEN        sizeof(st_UDPHeader)


typedef struct _st_SockHeader
{
	INT64       llMagicNumber;     /* 0x3812121281282828L */
	TIME_T64    tCurTime;          /* time_t + micro time */
	INT64       llSessID;          /* TEMS Session ID, �𸦶� 0x00 */
	USHORT      usTotLen;          /* sizeof(st_1stPktHdr) + sizeof(st_2ndPktHdr) + sizeof(data) */
	UCHAR       ucEncFlag;         /* 0x00 : ��ȣȭ �̻��, 0x01 : ��ȣȭ ��� */
	UCHAR       ucEncType;         /* 0x00 : �̻�� default, 0x01 : JvEn(De)cryption ��� */
	USHORT      usPktVer;          /* ó�� 0x10 */
	USHORT      usHashVal;
	USHORT      usSvcID;               /* Service ID */
	USHORT      usMsgID;               /* Message ID */
	USHORT      usSerial;              /* �ø��� ��ȣ */
	USHORT      usResult;              /* Result */
	UINT        uiCRC;                 /* CRC ��Ŷ�� �̻����� �Ǵ� */
	UINT        uiClientIPAddr;        /* Client IP Address */
	UINT        uiServerIPAddr;        /* Server IP Address */
	USHORT      usClientPort;          /* Client Port */
	USHORT      usServerPort;          /* Server Port */
} st_SockHeader, *pst_SockHeader;


#define SOCK_HEADER_LEN        sizeof(st_SockHeader)


//Index�� �ٷ� ã�� ���� event ����ü�� ���� ����ü�� ����Ű�� �Ѵ�
typedef struct _st_cli_info
{
	unsigned long long llKey;

	unsigned int	uiClientIP;
	unsigned short	usSendPort;
	unsigned short	usRecvPort;
} st_cli_info;


#define MAX_SOCK_CNT		50

#ifdef __TESTBED
#define MAX_SOCKSHM_CNT		500
#else
#define MAX_SOCKSHM_CNT		3000
#endif

#define MAX_SOCKBUF_SIZE	30720 * 2

#define MAX_PACKET_SIZE		30000
#define MAX_TEMPBUF_SIZE	200

#define MAX_READBUF_SIZE	10485760


typedef struct _st_READINFO
{
	INT			dSockFd;
	INT			dFirstPackFlag;
	UINT64		llKey;
	INT			dReadLen;
	INT			dReserved;
} st_READINFO, *pst_READINFO;


#define INFO_LEN sizeof( st_READINFO )

#define PKTSUM_INB		0
#define PKTSUM_OUTB		1

typedef struct _st_SOCKINFO
{
	st_READINFO stREADINFO;

	UINT        uiIPAddr;
	USHORT      usPort;
	UCHAR       ucFirstPackInfo;            	// ��뿩�� DEF_USABLE : 0  DEF_USED : 1
	UCHAR       ucWriteFlag;

	INT         dSockFd;            			// �����͸� �о���� Ŭ���̾�Ʈ�� ���� ����
	INT         dWSockFd;           			// �����͸� ������ Ŭ���̾�Ʈ�� ���� ����

	UINT        uiLastPos;          			// szSockBuf�� ���������� �� ��ġ
	UINT        uiReadPos;          			// T_DCRPT�� szSockBuf���� ���� ��ġ

	UINT        uiWritePos;         			// szWSockBuf�� ���������� �� ��ġ
	UINT        uiWSockPos;         			// ���������� SEND �� ��ġ

	time_t      tLastUpdateTime;

	UCHAR       szSockBuf[MAX_SOCKBUF_SIZE]; 	// Ŭ���̾�Ʈ�� ���� ������ ���� �� ����ϴ� ����
	UCHAR       szWSockBuf[MAX_SOCKBUF_SIZE]; 	// Ŭ���̾�Ʈ�� ������ �����͸� ��� �ִ� ����

	UINT        uiSendPos;          			// ������ ������ ���� ���� ������ ��ġ
	USHORT      usSerialServ;
	UCHAR       ucPingFlag;
	UCHAR       ucLastMsgFlag;

	INT64		llSendKey;
	INT64		llPubEncyKey[2];

	/* Data for Alive, Disconnect */
	UINT        uiLSrvIPAddr;
	USHORT      usLSrvPort;
	USHORT      usSerialCli;

	/* Data for summery */
	unsigned long long ullSumBytes[2];
	unsigned long long ullSumPkts[2];

	/* Data for BSMS Logout */
	UINT64      ullGameID;
	UINT        uiServerNo;
	UINT        uiChlNo;

	/* Logical IP, Addr */
	UINT        uiLIPAddr;
	USHORT      usLPort;
	UCHAR       ucBufferFull;
	UCHAR       ucCSMSNo;

	UINT64		ullCharIdx;
} st_SOCK_INFO, *pst_SOCK_INFO;

typedef struct _st_SOCKSHM
{
	UINT			uiUsedCnt;
	UINT			uiReserved;

	st_SOCK_INFO	stSOCKINFO[MAX_SOCKSHM_CNT];
} st_SOCK_SHM, *pst_SOCK_SHM;


typedef struct _st_SOCKBUF
{
	UINT	uiLastPos;
	UINT	uiReadPos;

	UCHAR	szReadBuf[MAX_READBUF_SIZE];
} st_SOCK_READBUF, *pst_SOCK_READBUF;

typedef struct _st_SOCKDELIM
{
	USHORT	usDelim;
	USHORT	usReserved;
	UINT	uiLength;

	INT64	llKey;
} st_SOCKDELIM, *pst_SOCKDELIM;

#define DEF_READ_DELIM	0x0FF0

#define SOCK_DELIM_SIZE		sizeof(st_SOCKDELIM)

#define DEF_MAX_BUF_SIZE	8192*10000
#define DEF_RECVBUF_SIZE    655360
#define DEF_SENDBUF_SIZE    655360
#define MAX_CLIENT_CNT		30
#define MAX_SERVER_CNT		100

#define DEF_OFF		0x00
#define DEF_ON		0x01

#define FINISH_PROGRAM 	1
#define CHECK_SOCKET	2

typedef struct _st_ClientSock
{
	INT64	llKey;
	INT		dSocket;
	UINT 	uiIPAddr;
	USHORT	usPort;
	USHORT	usTemsNo;
	INT		iReserved;
} st_CLIENT_SOCK, *pst_CLIENT_SOCK;

typedef struct _st_ClientInfo
{
	st_CLIENT_SOCK	stCSOCK;
	int     		dRecvLen;
	int     		dSendLen;
	int				dWriteFlag; /* DEF_OFF : 0, DEF_ON : 1 */
	int				dLoginStat; /* DEF_DISCONNECT 0, DEF_CONNECT 1 */
	TIME_T64		tAliveChkTime;
	TIME_T64		tAliveRcvTime;
	TIME_T64		tConnTryTime;
	UCHAR			ucConnTryFlag;
	UCHAR			ucSysNo;
	UCHAR			szReserved[6];

	UCHAR    		szRecvBuf[DEF_RECVBUF_SIZE];
	UCHAR    		szSendBuf[DEF_SENDBUF_SIZE];
} st_CLIENT_INFO, *pst_CLIENT_INFO;


#define __DEF_BUFFER_SIZE	1024000
typedef struct _st_ServerSock
{
	UINT64	ullKey;
	INT		dSocket;
	INT		dReserved;
	UINT	uiServerIPAddr;
	USHORT	usServerPort;
	USHORT	usSvr_Key;
} st_ServerSock, *pst_ServerSock;

typedef struct _st_ServerInfo
{
	st_ServerSock	stSSock;
	time_t          tConnTryTime;
	time_t			tAliveChkTime;
	time_t			tAliveRcvTime;
	int             dConnTryFlag; 	// 0: do not try 1:try
	int             dConnFlag;		// DEF_DISCONNECT:0 DEF_CONNECT:1
	int             dSendLen;
	int             dRecvLen;
	char            szRecvBuf[__DEF_BUFFER_SIZE];
	char            szSendBuf[__DEF_BUFFER_SIZE];
} st_ServerInfo, *pst_ServerInfo;

typedef struct _st_SESSINFO
{
	INT64	llMyKey;
	INT64	llSessKey;

	UINT	uiClient_IPAddr;
	USHORT	usClient_Port;
	USHORT	usReserved;
} st_SESSINFO,* pst_SESSINFO;


pst_SOCK_SHM pstSockTbl;


#define MAX_BROADCAST_CNT	60000

typedef struct _st_BROADCAST_INFO
{
	UINT	uiC_IPADDR;
	USHORT	usC_PORT;
	USHORT	usReserved;

	UINT	uiServerNo;
	UINT	uiTemsNo;

	INT64	llMYKEY;
	INT64	llSESSKEY;
} st_BROADCAST_INFO, *pst_BROADCAST_INFO;


typedef struct _st_PortSts_Header
{
	UINT    uiServerNo; /* set by the data file */
	UINT    uiReserved;
	UINT    uiTemsPortCnt;
	UINT    uiReserved2;
} st_PortSts_Header, * pst_PortSts_Header;

typedef struct _st_PortSts_Body
{
	UINT    uiSvcIPAddr;
	USHORT  usSvcPort;
	UCHAR   szReserved[ 2 ];
	UINT    uiMaxUserCnt;
	UINT    uiCurrUserCnt;
} st_PortSts_Body, * pst_PortSts_Body;

#endif
