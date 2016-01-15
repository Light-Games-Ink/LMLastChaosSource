#ifndef __GUILD_KICK_H__
#define __GUILD_KICK_H__

class CGuildKick
{
public:
	CGuildKick(void);
	~CGuildKick(void);

private:
	int m_masterKickStauts;			// �渶 �߹� ���� ���°�
	int m_masterKickRequestChar;	// �渶 �߹� ��û ĳ����
	int m_masterKickRequestTime;	// �渶 �߹� ��û �ð�

public:
	void setKickStatus(int _status);
	int getKickStatus(void);

	void setKickReuestChar(int _charIndex);
	int getKickRequestChar(void);

	void setKickRequestTime(int _time);
	int getKickRequestTime(void);
};


#endif
