#ifndef __GPS_DATA_MANAGER__
#define __GPS_DATA_MANAGER__

#include <vector>
#include <map>
class CPC;
class CGPSDataManager
{
public:
	CGPSDataManager();
	~CGPSDataManager();
	static CGPSDataManager* instance();

	typedef std::map< int, std::vector<int> > _MAP;
	_MAP gpsMap;

public:
	//Data
	void _insert(int charIndex, int targetIndex);	//gps �������� ����Ͽ��� ��
	void _remove(int charIndex, int targetIndex);	//������ ��� �ð��� ������ �����͸� �����ؾߵǴ� ���
	void _update(CPC* targetPc);				//Ÿ���� ������ �����Ͽ��� ��
};

#endif