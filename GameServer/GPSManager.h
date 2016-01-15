#ifndef __GPS_MANAGER__
#define __GPS_MANAGER__

#include "mysql.h"

class CDBCmd;

class CGPSManager
{
public:
	explicit CGPSManager(CPC* pc);
	~CGPSManager();

private:
	CPC* _owner;
	CPC* m_gpsTarget;				//Ÿ�� ĳ����
	std::string m_gpsTargetName;	//Ÿ�� �̸�
	int m_gpsTargetIndex;			//Ÿ�� ĳ���� �ε���
	bool m_isGps;					//gps ������ ��� ����
	bool m_isGpsInterrupt;			//gps ���ؾ����� ��� ����

	bool isNotFoundSend;			//��ã�� ������ ��Ŷ�� ���� �ʿ䰡 �����Ƿ� �������ִ� ������ �ʿ� (�ѹ��̶� �������� true, �ƴϸ� false)

	void sendNotFound();
public:
	//DataBase
	void save(std::vector<std::string> & vec);
	void load(MYSQL* conn);

	//Data
	void setGpsTarget(CPC* targetPC);
	void setIsGps(bool gps);
	void setIsGpsInterrupt(bool gpsInterrupt);

	CPC* getGpsTarget();
	int getGpsTargetIndex();

	bool isGps();
	bool isGpsInterrupt();

	void inputTargetInfo(int targetIndex, const char* targetName);
	void sendTargetInfo();
	void sendGpsTargetMoveInfo();
	void deleteTarget();
};

class ArtifactGPSManager
{
private:
	bool m_isGps;					//gps ������ ��� ����
	CPC* _owner;

public:
	explicit ArtifactGPSManager(CPC* pc);
	~ArtifactGPSManager();

	void setIsGps(bool gps);
	bool isGps();

	void sendGpsTargetMoveInfo();
	void sendGpsOwnerCountInfo();
};

#endif