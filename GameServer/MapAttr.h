#ifndef __AMAPATTR_H__
#define __AMAPATTR_H__

class CMapAttr
{
public:
	CMapAttr();
	~CMapAttr();

	///////////////
	// �� �Ӽ� ����
	int					m_size[2];		// �� ũ��: 0 - width, 1 - height
	unsigned short**	m_attr;			// �Ӽ� �迭
	float**				m_height;		// ���� ��

	bool Load(int zone, int ylayer, int w, int h, const char* attrmap, const char* heightmap);
};

#endif
//
