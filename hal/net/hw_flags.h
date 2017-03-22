#ifndef _HW_FLAGS_H_
#define _HW_FLAGS_H_

// these are for compatibility with the fwd network stack

// PHY flags are in bits 15-0
#define PHY_HALF_DUPLEX        0
#define PHY_10MBIT             0
#define PHY_100MBIT            (1 << 0)
#define PHY_FULL_DUPLEX        (1 << 1)
#define PHY_AUTO_NEGOTIATION   (1 << 2)
#define PHY_LOOPBACK           (1 << 3)

// MAC flags are in bits 31-16
#define MAC_ACCEPT_BROADCAST    (1 << 16)
#define MAC_ACCEPT_MULTICAST    (1 << 17)
#define MAC_ACCEPT_ANY_UNICAST  (1 << 18)

#endif
