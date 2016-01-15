#ifndef __CONFIG_PACKET_H_
#define __CONFIG_PACKET_H_

#define MAX_PACKET_SIZE			(4096)

// ���۰��� ���� ���
#define MAX_MESSAGE_TYPE		((int)(sizeof(unsigned char)))
#define MAX_MESSAGE_SIZE		MAX_PACKET_SIZE
#define MAX_MESSAGE_DATA		(MAX_MESSAGE_SIZE - MAX_MESSAGE_TYPE)

#endif
