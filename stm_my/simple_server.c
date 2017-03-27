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
#include "syntax.h"
#include <string.h>
#include "hal.h"
#include "ip_arp_udp_tcp.h"
#include "net.h"
#include "simple_server.h"

#include "syntax.h"
#include "wtf.h"
#include "debug.h"

static const eAdrGD   *pADRGD;
static uint8_t* EthSost;
static uint8_t* EthBlock;
static uint16_t* PORTNUMBER;
static const uint8_t* IPAddr;
static uint8_t* MACAddr;


static unsigned int plen;
static unsigned int dat_p;

static eSocket  Sockets[MAX_SOCKET_COUNT];

// XXX: +1 because lame phy writes beyound the packet end
static unsigned char fbuf[BUFFER_SIZE+1];






void BufCpy(char *pp1, char *pp2, int n) { //dest,source,number
    while (n--) *pp1++ = *pp2++;
//        ClrDog;
}



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

void SocketUpdate(char nSock,char* f_buf,int data_p,int* fbsize)
{
    LOG("socket update");

    switch (Sockets[nSock].IP_PHASE)
    {
    case(ETH_INIT):
        LOG("socket init");
        *fbsize=0;
        return;

    case(ETH_HEAD):
        if (!data_p)
        {
            *fbsize=0;
            return;
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
        if ((Sockets[nSock].Header.NumDirect&0xF0) == OUT_UNIT)
            Sockets[nSock].IP_PHASE=ETH_SENDBLOCK;
        else
            Sockets[nSock].IP_PHASE=ETH_RECVBLOCK;
        return;

    case(ETH_SENDBLOCK):
        LOG("send size %d, block %d, offset %d."
            "block addr is 0x%08x", Sockets[nSock].Header.Size, Sockets[nSock].NumBlock, Sockets[nSock].Header.Adr,
            (int) pADRGD[Sockets[nSock].NumBlock].Adr);

        if (Sockets[nSock].Header.Size>MAX_PACKET_LENGTH)
        {
            fill_tcp_buf(f_buf,MAX_PACKET_LENGTH,((char*)pADRGD[Sockets[nSock].NumBlock].Adr)+Sockets[nSock].Header.Adr);
            Sockets[nSock].Header.Adr+=MAX_PACKET_LENGTH;
            Sockets[nSock].Header.Size-=MAX_PACKET_LENGTH;
            *fbsize=MAX_PACKET_LENGTH;
//			Sockets[nSock].IP_PHASE=ETH_INIT;
            return;
        }
        fill_tcp_buf(f_buf, Sockets[nSock].Header.Size, (u8 *)(pADRGD[Sockets[nSock].NumBlock].Adr) + Sockets[nSock].Header.Adr);
        /*if (Sockets[nSock].Header.Size  ==  263)
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
        return;

    case(ETH_RECVBLOCK):
        LOG("socket recvblock");
        if (Sockets[nSock].Header.Size<=plen-54)
        {
            volatile char crc = 55-CheckSum(&fbuf[data_p], info_data_len-1);
            if (crc != fbuf[info_data_len+53])
                return;
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
//			if ((Sockets[nSock].NumBlock<12)&&((Sockets[nSock].Header.NumDirect&0xF0) == IN_UNIT))
        {

            BufCpy(((char*)pADRGD[Sockets[nSock].NumBlock].Adr)+Sockets[nSock].Header.Adr,&f_buf[data_p],plen);
            Sockets[nSock].Header.Adr+=plen;

            // test
            //if (Sockets[nSock].Header.Size  ==  263)
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
        return;
    }
}

// XXX: temporary adapters for mac api compatibility
static uint mac_recv_pkt_adapter(void)
{
    uint len;
    u8 *macbuf = HAL_mac_read_packet(&len);

    if (! macbuf)
        return 0;

    REQUIRE(plen <= sizeof(fbuf) - 1);

    memcpy(fbuf, macbuf, len);
    HAL_mac_free_buf(macbuf);

    #warning "inside the phy was a fucka nullterminating the packet. moved it here"
    fbuf[plen] = '\0';

    return len;
}


#warning IP !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void simple_servercycle(void)
{
    uint8_t tIPAddr[4];
    char i,nS,freeSlot;
    volatile unsigned int j;
//		OSTimeDlyHMSM(0, 0, 0, 50);
    // get the next new packet:

    plen = mac_recv_pkt_adapter();

    if (! plen)
        return;

    // arp is broadcast if unknown but a host may also
    // verify the mac address by sending it to
    // a unicast address.
    if (eth_type_is_arp_and_my_ip(fbuf,plen))
    {
        make_arp_answer_from_request(fbuf);
        //USART_DMASendText(USART1,"make_arp_answer_from_request\n");
        return;
    }

    // check if ip packets are for us:
    if (eth_type_is_ip_and_my_ip(fbuf,plen) == 0)
    {
        return;
    }

    if (fbuf[IP_PROTO_P] == IP_PROTO_ICMP_V && fbuf[ICMP_TYPE_P] == ICMP_TYPE_ECHOREQUEST_V)
    {
        // a ping packet, let's send pong
        make_echo_reply_from_request(fbuf, plen);
        //USART_DMASendText(USART1,"make_echo_reply_from_request\n");
        return;
    }
    // tcp port www start, compare only the lower byte

    if (fbuf[IP_PROTO_P] == IP_PROTO_TCP_V&&fbuf[TCP_DST_PORT_H_P] == (*PORTNUMBER)/256&&fbuf[TCP_DST_PORT_L_P] == (*PORTNUMBER)%256)
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
            LOG("initing len info");

            init_len_info(fbuf); // init some data structures
            // we can possibly have no data, just ack:
            dat_p=get_tcp_data_pointer();
            if (dat_p == 0)
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
                LOG("got data p");
                //if (Sockets[nS].IP_PHASE  ==  ETH_RECVBLOCK)
                //{
                //	fbuf[27] = 0xFF;
                //}

                #warning "ignore checksum for now"
                if (0)
                {
                    j=checksum(&fbuf[IP_SRC_P], 8+TCP_HEADER_LEN_PLAIN+info_data_len,2);
                    if (j)
                    {
                        //j = 0xFF;
                        return;
                    }
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

void simple_server(const eAdrGD *fADRGD, uint8_t* fSostEth,uint8_t* nBlock, const uint8_t* fIPAddr,uint8_t* fMACAddr,uint16_t *fPORTNUMBER)
{

//	Del_1ms(100);
    /*initialize enc28j60*/
    pADRGD=fADRGD;
    EthSost=fSostEth;
    EthBlock=nBlock;
    PORTNUMBER=fPORTNUMBER;
    MACAddr=fMACAddr;
    IPAddr=fIPAddr;
    HAL_mac_init(MACAddr, PHY_FULL_DUPLEX | MAC_ACCEPT_BROADCAST);
    init_ip_arp_udp_tcp(MACAddr,IPAddr,*PORTNUMBER);
    //ЦёКѕµЖЧґМ¬:0x476 is PHLCON LEDA(ВМ)=links status, LEDB(єм)=receive/transmit
//    enc28j60PhyWrite(PHLCON,0x7a4);
//    enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz
//	Del_1ms(20);
    Sockets[0].IP_PHASE=0;
    //init the ethernet/ip layer:
//        return (0);
}
