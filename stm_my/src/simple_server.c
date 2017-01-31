/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher
 * Copyright: GPL V2
 * See http://www.gnu.org/licenses/gpl.html
 *
 * Ethernet remote device and sensor
 * UDP and HTTP interface 
        url looks like this http://baseurl/password/command
        or http://baseurl/password/
 *
 * Title: Microchip ENC28J60 Ethernet Interface Driver
 * Chip type           : ATMEGA88 with ENC28J60
 * Note: there is a version number in the text. Search for tuxgraphics
 *********************************************/


/*********************************************
 * modified: 2007-08-08
 * Author  : awake
 * Copyright: GPL V2
 * http://www.icdev.com.cn/?2213/
 * Host chip: ADUC7026
**********************************************/
//#include <includes.h>
#include <string.h>
#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "net.h"
#include "simple_server.h"

eAdrGD	*pADRGD;
uint8_t* EthSost;
uint8_t* EthBlock;
uint16_t* PORTNUMBER;
uint8_t* IPAddr;
uint8_t* MACAddr;


unsigned int plen;
unsigned int dat_p;
unsigned char ffi=0;
unsigned char fcmd_pos=0;
unsigned char fcmd;
unsigned char payloadlen=0;
char fstr[30];
char cmdval;

int16_t GlobData;


//extern void delay_ms(unsigned char ms);

// please modify the following two lines. mac and ip have to be unique
// in your local area network. You can not have the same numbers in
// two devices:

// base url (you can put a DNS name instead of an IP addr. if you have
// a DNS server (baseurl must end in "/"):
static char baseurl[]="http://192.168.1.25/";
//static unsigned int myport =PORTNUM; // listen port for tcp/www (max range 1-254)
// or on a different port:
//static char baseurl[]="http://10.0.0.24:88/";
//static unsigned int mywwwport =88; // listen port for tcp/www (max range 1-254)
//
static unsigned int myudpport =1200; // listen port for udp
// how did I get the mac addr? Translate the first 3 numbers into ascii is: TUX


static unsigned char fbuf[BUFFER_SIZE+1];






void BufCpy(char *pp1, char *pp2, int n) { //dest,source,number
        while(n--) *pp1++ = *pp2++;
//        ClrDog;
		}



// takes a string of the form password/commandNumber and analyse it
// return values: -1 invalid password, otherwise command number
//                -2 no command given but password valid
unsigned char analyse_get_url(char *str)
	{
	unsigned char i=0;
	if (verify_password(str)==0)
		{
		return(-1);
		}
	// find first "/"
	// passw not longer than 9 char:
	while(*str && i<10 && *str >',' && *str<'{')
		{
        if (*str=='/')
			{
            str++;
            break;
        	}
        i++;
        str++;
		}
	if (*str < 0x3a && *str > 0x2f)
		{
        // is a ASCII number, return it
        return(*str-0x30);
		}
	return(-2);
	}

/*uint16_t CRC16(char *fbuf, const uint16_t start, const uint16_t end)
{
  volatile int i,j;
  volatile uint16_t bCRC;
  volatile char ch;
  bCRC = 0xFFFF;
  for (i=start; i<end; i++)
  {
	  ch = *fbuf;
    bCRC = bCRC ^ ch;
    fbuf++;
    for (j=0; j<8; j++)
    {
      if ( (bCRC % 2) > 0 )
      {
        bCRC = bCRC >> 1;
        bCRC = bCRC ^ 0xA001;
      }
      else
        bCRC = bCRC >> 1;
    }
  }
  return bCRC;
}*/

char CheckSum(char *byte, int size)
{
  int i;
  char res = 0;
  for (i=0; i < size; i++)    // 263
  {
	  res = res + byte[i];
  }
  return res;
}

int SocketUpdate(char nSock,char* f_buf,int data_p,int* fbsize)
{
switch (Sockets[nSock].IP_PHASE)
{
	case(ETH_INIT):
			*fbsize=0;return 0;
	case(ETH_HEAD):
		if (!data_p)
		{
			*fbsize=0;
			return 0;
		}
//		else
		*EthSost=WORK_UNIT;
		BufCpy((char*)&Sockets[nSock].Header,&f_buf[data_p],cSizeHead);
		Sockets[nSock].NumBlock=Sockets[nSock].Header.NumDirect&0x0F;
/*		Sockets[nSock].IP_PHASE=ETH_HEADRECEIVED;
		*fbsize=0;
		return 0;
	case(ETH_HEADRECEIVED):*/
		fill_tcp_buf(f_buf,1,&Sockets[nSock].Header.NumDirect);
		*fbsize=1;
		if ((Sockets[nSock].Header.NumDirect&0xF0)==OUT_UNIT)
			Sockets[nSock].IP_PHASE=ETH_SENDBLOCK;
		else
			Sockets[nSock].IP_PHASE=ETH_RECVBLOCK;
		return 1;
	case(ETH_SENDBLOCK):
		if (Sockets[nSock].Header.Size>MAX_PACKET_LENGTH)
		{

			fill_tcp_buf(f_buf,MAX_PACKET_LENGTH,((char*)pADRGD[Sockets[nSock].NumBlock].Adr)+Sockets[nSock].Header.Adr);
			Sockets[nSock].Header.Adr+=MAX_PACKET_LENGTH;
			Sockets[nSock].Header.Size-=MAX_PACKET_LENGTH;
			*fbsize=MAX_PACKET_LENGTH;
//			Sockets[nSock].IP_PHASE=ETH_INIT;
			return 1;
		}

		fill_tcp_buf(f_buf,Sockets[nSock].Header.Size,((char*)pADRGD[Sockets[nSock].NumBlock].Adr)+Sockets[nSock].Header.Adr);
		/*if (Sockets[nSock].Header.Size == 263)
		{
			volatile int16_t ii;
			ii = f_buf[62];
			ii = ii << 8;
			ii = ii + f_buf[61];
			if (ii > 1500)
			{
				ii = 10;
				f_buf[61] = 0x10;  // 95
				f_buf[62] = 0x27;  // 95
			}
		}*/
		*fbsize=Sockets[nSock].Header.Size;

		Sockets[nSock].IP_PHASE=ETH_INIT;
		*EthSost=OUT_UNIT;

		return 1;
	case(ETH_RECVBLOCK):
		if (Sockets[nSock].Header.Size<=plen-54)
        {
          volatile char crc = 55-CheckSum(&fbuf[data_p], info_data_len-1);
          if ( crc != fbuf[info_data_len+53] ) return 0;
        }

		if (Sockets[nSock].Header.Size<=plen-54)//(plen-54))/*info_data_len/*(plen-54)*/
		{
			plen=Sockets[nSock].Header.Size;
			*EthBlock=Sockets[nSock].NumBlock;
			*EthSost=IN_UNIT;
			Sockets[nSock].IP_PHASE=ETH_INIT;
		}
		else plen-=54;
//		if ((Sockets[nSock].Header.Adr<pADRGD[Sockets[nSock].NumBlock].MaxSize-plen))/*&&(Sockets[nSock].Header.Adr>=0)
//				&&(Sockets[nSock].Header.Size<pADRGD[Sockets[nSock].NumBlock].MaxSize))*/
//			if ((Sockets[nSock].NumBlock<12)&&((Sockets[nSock].Header.NumDirect&0xF0)==IN_UNIT))
			{

				BufCpy(((char*)pADRGD[Sockets[nSock].NumBlock].Adr)+Sockets[nSock].Header.Adr,&f_buf[data_p],plen);
				Sockets[nSock].Header.Adr+=plen;

				// test
				//if (Sockets[nSock].Header.Size == 263)
				//{
				//	CRC16(&f_buf[data_p], 0, 50);

					//volatile int16_t ii;
					//ii = f_buf[98];
					//ii = ii << 8;
					//ii = ii + f_buf[97];
					//if (ii > 1500)
					//{
					//	ii = 10;
					//	f_buf[105] = 0x10;  // 95     // 61 макс 1 конт
					//	f_buf[106] = 0x27;  // 95	 // 62 макс 1 конт
					//}
				//}
				// test
			}
		Sockets[nSock].Header.Size-=plen;
		*fbsize=0;
		return 1;
}
}

#warning IP !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int simple_servercycle(void)
{
	uchar tIPAddr[4];
	char i,nS,freeSlot;
	volatile unsigned int j;
//		OSTimeDlyHMSM(0, 0, 0, 50);
        // get the next new packet:
        plen = enc28j60PacketReceive(BUFFER_SIZE, fbuf);
        //USART_DMASendData(USART1,fbuf,plen);

        /*plen will ne unequal to zero if there is a valid packet (without crc error) */
        if(plen==0)
        	{
            return;
        	}
        // arp is broadcast if unknown but a host may also
        // verify the mac address by sending it to 
        // a unicast address.
        if(eth_type_is_arp_and_my_ip(fbuf,plen))
        	{
			make_arp_answer_from_request(fbuf);
			//USART_DMASendText(USART1,"make_arp_answer_from_request\n");
            return;
        	}

        // check if ip packets are for us:
        if(eth_type_is_ip_and_my_ip(fbuf,plen)==0)
        	{
            return;
        	}

                if(fbuf[IP_PROTO_P]==IP_PROTO_ICMP_V && fbuf[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V)
        	{
            // a ping packet, let's send pong	
			make_echo_reply_from_request(fbuf, plen);
			//USART_DMASendText(USART1,"make_echo_reply_from_request\n");
			return;
        	}
               // tcp port www start, compare only the lower byte

		if (fbuf[IP_PROTO_P]==IP_PROTO_TCP_V&&fbuf[TCP_DST_PORT_H_P]==(*PORTNUMBER)/256&&fbuf[TCP_DST_PORT_L_P]==(*PORTNUMBER)%256)
			{
			nS=101;
			freeSlot=9;
			for (i=0;i<MAX_SOCKET_COUNT;i++)
			{
				Sockets[i].Timeout--;
				if (Sockets[i].Timeout<0)
				{
					Sockets[i].Timeout=0;
					Sockets[i].IP_PHASE=0;
				}
				if (!Sockets[i].IP_PHASE) freeSlot=i;
				BufCpy(tIPAddr,Sockets[i].IP_source,4);
				if (check_ip_scr(fbuf,tIPAddr))
				{
					nS=i;break;
				}
			}
			if (nS>100) nS=freeSlot;
            //nS=0;

			if (fbuf[TCP_FLAGS_P] & TCP_FLAGS_SYN_V)
				{
                make_tcp_synack_from_syn(fbuf);
                // make_tcp_synack_from_syn does already send the syn,ack
        		Sockets[nS].IP_PHASE=ETH_HEAD;
        		Sockets[nS].Timeout=20;
	            BufCpy(Sockets[nS].IP_source,tIPAddr,4);

                return;
            	}
	        if (fbuf[TCP_FLAGS_P] & TCP_FLAGS_ACK_V)
				{
	            GlobData++;
	            init_len_info(fbuf); // init some data structures
	            // we can possibly have no data, just ack:
	            dat_p=get_tcp_data_pointer();
	            if (dat_p==0)
					{
	                if (fbuf[TCP_FLAGS_P] & TCP_FLAGS_FIN_V)
						{
	                    // finack, answer with ack
	                    make_tcp_ack_from_any(fbuf);
	                    return;
	                	}
	                // just an ack with no data, wait for next packet
	                }
	            else
	            {
	            	//if (Sockets[nS].IP_PHASE == ETH_RECVBLOCK)
	            	//{
	            	//	fbuf[27] = 0xFF;
	            	//}

					j=checksum(&fbuf[IP_SRC_P], 8+TCP_HEADER_LEN_PLAIN+info_data_len,2);
					if (j)
					{
						//j = 0xFF;
						return;
					}
	            }
	            check_ip_scr(fbuf,tIPAddr);
//	            GlobData++;
	            BufCpy(Sockets[nS].IP_source,tIPAddr,4);
	            SocketUpdate(nS,fbuf,dat_p,&plen);

	            if (plen)
	            	make_tcp_ack_with_data(fbuf,plen,0); // send data
	            else
	            	make_tcp_ack_from_any(fbuf); // send ack for http get
				return;
				}
			}
	// tcp port www end
	//

}

int simple_server(eAdrGD* fADRGD,uint8_t* fSostEth,uint8_t* nBlock, uint8_t* fIPAddr,uint8_t* fMACAddr,uint8_t* fPORTNUMBER)
	{
	SPI1_Init();

//	Del_1ms(100);
	/*initialize enc28j60*/
	pADRGD=fADRGD;
	EthSost=fSostEth;
	EthBlock=nBlock;
	PORTNUMBER=fPORTNUMBER;
	MACAddr=fMACAddr;
	IPAddr=fIPAddr;
	enc28j60Init(MACAddr);
	init_ip_arp_udp_tcp(MACAddr,IPAddr,*PORTNUMBER);
    //ЦёКѕµЖЧґМ¬:0x476 is PHLCON LEDA(ВМ)=links status, LEDB(єм)=receive/transmit
    enc28j60PhyWrite(PHLCON,0x7a4);
	enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz
//	Del_1ms(20);
	Sockets[0].IP_PHASE=0;
	//init the ethernet/ip layer:
//        return (0);
	}
