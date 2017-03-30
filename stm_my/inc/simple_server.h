/*Константы для работы сервера Ethernet*/

#ifndef _TCPIP_H
#define _TCPIP_H

#include  "ENC28J60.h"

#define OUT_UNIT        0x50
#define IN_UNIT         0xa0
#define WORK_UNIT       0x70


#define ETH_INIT		0
#define ETH_HEAD		1
#define ETH_HEADRECEIVED		2
#define ETH_SENDBLOCK	3
#define ETH_RECVBLOCK	4
#define MAX_SOCKET_COUNT	5
#define MAX_PACKET_LENGTH	1200
#define cSizeHead		5


#define BUFFER_SIZE 1500//400
#define PSTR(s) s

typedef struct eeHeader
{
	int16_t Adr;
	int16_t Size;
	uint8_t	NumDirect;

}eHeader;

typedef struct eeAdrGD {
	uint8_t*	Adr;
	uint16_t	MaxSize;
}eAdrGD;

typedef struct eeSocket
		{
			uint8_t	IP_source[4];
			char	IP_PHASE;
			int		Timeout;
			char	NumBlock;
			eHeader Header;
		} eSocket;

eSocket	Sockets[MAX_SOCKET_COUNT];






int simple_server(eAdrGD* fADRGD,uint8_t* fSostEth,uint8_t* nBlock, uint8_t* fIPAddr,uint8_t* fMACAddr,uint8_t* fPORTNUMBER);
int simple_servercycle(void);

#endif


