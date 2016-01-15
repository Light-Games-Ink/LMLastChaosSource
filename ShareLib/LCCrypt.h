#ifndef __LCCRYPT_H__
#define __LCCRYPT_H__

// DES BYTE ALIGN - 8, ORIGINAL LENGTH - 4, DUMMY - 1
#define LCCNM_META_DATA_SIZE		13

#define LC_DES3_DEFAULT_KEY_HIGH		(0x6B772641)
#define LC_DES3_MASK_HIGH				(0x3A65943B)
#define LC_LENGTH_VEC					405
#define LC_DES3_DEFAULT_KEY_LOW			(0xD06F4ABE)
#define LC_DES3_MASK_LOW				(0x4C452A96)
#define LC_DES3_OLD_KEY_HIGH			(0x53653453)
#define LC_DES3_OLD_KEY_LOW				(0x51476296)

// ���� ���� ����� ���ϰ� LCDES3CONTEXT �����͸� �̿��Ͽ� ��ȣ ��ȣ�� Ȱ���Ѵ�
typedef struct __tagLCDES3CONTEXT
{
	unsigned int KnL[32];
	unsigned int nKeyHigh;
	unsigned int nKeyLow;

	__tagLCDES3CONTEXT()
	{
		memset(KnL, 0, sizeof(KnL));
		nKeyHigh = LC_DES3_DEFAULT_KEY_HIGH;
		nKeyLow  = LC_DES3_DEFAULT_KEY_LOW;
	}
} LCDES3CONTEXT;

// ��ȣŰ ���� ������
#define CNM_KEY_TYPE				LCDES3CONTEXT

// ��ȣ/��ȣ ���� ũ��
#define CNM_TEMP_BUFFER_LENGTH	(MAX_MESSAGE_DATA + LCCNM_META_DATA_SIZE)

// ���̺귯������ ���ǵǾ� �Ǵ� �Լ�

#ifdef _MSC_VER
#ifdef  __cplusplus
extern "C" {
#endif // __cplusplus
#endif // _MSC_VER

// ��ȣŰ �ʱ�ȭ �Լ�
extern void CNM_InitKeyValue(CNM_KEY_TYPE* pKey);

// ��ȣŰ ���� �Լ� : ���� ��ȣŰ�κ��� ���� ��ȣŰ�� ����
extern void CNM_NextKey(CNM_KEY_TYPE* pKey);

// ��ȣŰ ����
extern void CNM_CopyKey(CNM_KEY_TYPE* pDestKey, const CNM_KEY_TYPE* pSrcKey);

// ��ȣȭ
// return value		��ȣȭ�� ����, ���� �߻��� ����
// pSrc				���� ������
// nLenSrc			���� ������ ����
// pKey				��ȣŰ ������
// pDest			��� ����
extern int CNM_Crypt(const unsigned char* pSrc, int nLenSrc, CNM_KEY_TYPE* pKey, unsigned char* pDest, unsigned char* pTmpBuf);
extern int CNM_OldCrypt(const unsigned char* pSrc, int nLenSrc, CNM_KEY_TYPE* pKey, unsigned char* pDest, unsigned char* pTmpBuf);

// ��ȣȭ
// return value		��ȣ�� ����, ���� �߻��� 0 �Ǵ� ����
// pSrc				��ȣ�� ������
// nLenSrc			��ȣ�� ������ ����
// pKey				��ȣŰ ������
// pDest			��� ����
// pTmpBuf			��ȣ�� �ӽ� ����
extern int CNM_Decrypt(const unsigned char* pSrc, int nLenSrc, CNM_KEY_TYPE* pKey, unsigned char* pDest, unsigned char* pTmpBuf);
extern int CNM_OldDecrypt(const unsigned char* pSrc, int nLenSrc, CNM_KEY_TYPE* pKey, unsigned char* pDest, unsigned char* pTmpBuf);

// Seed ���� : Client
// return value		������ Seed Value
// strID			User ID
// strPW			User Password
// nTickCount		return value of GetTickCount()
extern unsigned int CNM_MakeSeedForClient(const char* strID, const char* strPW, unsigned long nTickCount);

// Seed ���� : Server
// return value		������ Seed Value
// nRandom			Random Value
// nPulse			Server Pulse
extern unsigned int CNM_MakeSeedForServer(int nRandomValue, int nServerPulse);

// Seed���� Key ����
// pKey				������ Key�� ������ ��
// nSeed			����� Seed Value
extern void CNM_MakeKeyFromSeed(CNM_KEY_TYPE* pKey, unsigned int nSeed);

#ifdef _MSC_VER
#ifdef  __cplusplus
}
#endif // __cplusplus
#endif // _MSC_VER

#endif // __LCCRYPT_H__
//
