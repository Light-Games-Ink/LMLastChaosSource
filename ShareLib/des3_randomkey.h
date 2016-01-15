#ifndef __DES3_RANDOM_KEY_H__
#define __DES3_RANDOM_KEY_H__

// #ifndef ������ �ߺ� include�� ������ �ʿ� ����!!!!
// ���� ���� des3.cpp �̿��� ���� include �ؼ��� �ȵȴ�

// Key Gen�� Random �Լ� ��
// ���� Ŭ���̾�Ʈ ���� �Լ���
// ��� inline�̰� des3.cpp �̿ܿ��� include �ϸ� �ȵ�
// CircleMUD���� ������

#define DES3_RANDOM_M			((unsigned int)0x7FFFFFFF)
#define DES3_RANDOM_Q			((unsigned int)0x0001F31D)
#define DES3_RANDOM_A			((unsigned int)0x000041A7)
#define DES3_RANDOM_R			((unsigned int)0x00000B14)

inline unsigned int LCDES3_GetRandom32Bit(register unsigned int nSeed)
{
	register unsigned int nLow, nHigh;
	nHigh = nSeed / DES3_RANDOM_Q;
	nLow  = nSeed % DES3_RANDOM_Q;
	nSeed = DES3_RANDOM_A * nLow + DES3_RANDOM_R * nHigh;
	return nSeed;
}
//

#endif
