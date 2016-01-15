#ifndef __ATTENDANCE_MANAGER_H__
#define __ATTENDANCE_MANAGER_H__

enum
{
	NONE,
	ATTENDANCE_SYS_MSG_CASE1,		//�⼮ ���� ���� ����
	ATTENDANCE_SYS_MSG_CASE2,		//���� ���� ���λ��¿��� ������ �Ⱓ�� ���� �Ǵ� �⼮ ���� ������ �Ⱓ�� ����� ���� ���ӿ� ���� ���� �Ͽ��� ��
	ATTENDANCE_SYS_MSG_CASE3,		//���ӿ� ���� ���� �ƴ� �� ����ǰ� ����� ������ ���� �� ���ӿ� ���� �Ͽ��� ��
};

class CAttendanceManager
{
public:
	CAttendanceManager(CPC* pc);
	~CAttendanceManager();

	CPC* _owner;

private:
	int m_acc_count;
	int m_max_acc;
	int m_last_reward_point;

	bool m_isAttendance;
	bool m_isUseAssure;
	bool m_isFirst;

	int m_sysMsg;

public:
	void setAttendance(bool isAttendance);
	bool getAttendance();

	void setUseAssure(bool isUseAssure);
	bool getUseAssure();

	void setAccCount(int accCount);
	int getAccCount();

	void setMaxAcc(int maxAcc);
	int getMaxAcc();

	void setLastRewardPoint(int last_reward_point);
	int getLastRewardPoint();

	void setSysMsg(int sysMsg);
	int getSysMsg();

	void load(MYSQL* conn);

	void checkAttendance();
	void finish();

	void reward();

	void assureOk();

	void sendSysMsg();
};

#endif