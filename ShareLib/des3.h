#ifndef __DES3_H__
#define __DES3_H__

// LCDES3������ ������ single-length DES3�� �����ϵ��� ��

inline void LCDES3_InitKey(LCDES3CONTEXT* pLCDES3Context, unsigned int nKeyHigh, unsigned int nKeyLow)
{
	pLCDES3Context->nKeyHigh = nKeyHigh;
	pLCDES3Context->nKeyLow = nKeyLow;
}

#endif // __DES3_H__
//
