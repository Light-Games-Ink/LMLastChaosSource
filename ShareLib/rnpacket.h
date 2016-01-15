#ifndef __RENEW_PACKET_H__
#define __RENEW_PACKET_H__

#include "MemoryPoolBase.h"
#include "rntype.h"
#include "logsystem.h"

#define PACKET_HEADER_SIZE		(sizeof(rnPacket::Header))
#define APPEND_PACKET_SIZE		(16)
#define PACKET_CHECK_SUM_SIZE	(10)
#define MAX_PACKET_MESSAGE		(16383)

class rnPacket : public MemoryPoolBaseWithMutex<rnPacket>
{
public:
	typedef boost::shared_ptr<rnPacket>		SP;

public:
	static void init();

public:
	explicit rnPacket();
	explicit rnPacket(std::string& msg);
	explicit rnPacket(const char* msg, const int length);
	~rnPacket();

	tBYTE* header()
	{
		return buffer_;
	}
	tUINT size()
	{
		return next_store_pos_;
	}
	tBYTE* data()
	{
		return buffer_;
	}
	std::vector<std::string>& getVector()
	{
		return vec_;
	}

public:
	bool addString(std::string& msg);
	bool addString(const tBYTE* msg, tUINT length);
	void calc();

private:
	void			mallocBuffer( tINT size );

private:
	tBYTE*			buffer_;				// �޸� ���̽� ����Ʈ
	std::vector<std::string>	vec_;
	tUINT			next_store_pos_;		// ������ �����Ұ�, ���������� ��Ŷ�� ���̸� ��Ÿ��('\0' ������)

	tINT			init_alloc_size_;
	tINT			pool_index_;			// �޸� Ǯ������ �ε���
};

#endif
