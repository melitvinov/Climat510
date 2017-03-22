#ifndef _ENC28J60_H_
#define _ENC28J60_H_

//-- bit definitions header was found on the net. it's somebody else good work

// ENC28J60 Control Registers
// Control register definitions are a combination of address,
// bank number, and Ethernet/MAC/PHY indicator bits.

// - Register address        (bits 0-4)
// - Bank number        (bits 5-6)
// - MAC/PHY indicator        (bit 7)
#define ADDR_MASK        0x1F
#define BANK_MASK        0x60
#define BANK_MASK_IDX    5
#define SPRD_MASK        0x80

// All-bank registers
#define EIE              0x1B
#define EIR              0x1C
#define ESTAT            0x1D
#define ECON2            0x1E
#define ECON1            0x1F

#define IS_MII_REG  (1 << 7)

#define BANK_0      (0 << 5)
#define BANK_1      (1 << 5)
#define BANK_2      (2 << 5)
#define BANK_3      (3 << 5)

#define SHARED_REGS_OFFSET  0x1B

// Bank 0 registers
#define ERDPTL           (0x00 | BANK_0)
#define ERDPTH           (0x01 | BANK_0)
#define EWRPTL           (0x02 | BANK_0)
#define EWRPTH           (0x03 | BANK_0)
#define ETXSTL           (0x04 | BANK_0)
#define ETXSTH           (0x05 | BANK_0)
#define ETXNDL           (0x06 | BANK_0)
#define ETXNDH           (0x07 | BANK_0)
#define ERXSTL           (0x08 | BANK_0)
#define ERXSTH           (0x09 | BANK_0)
#define ERXNDL           (0x0A | BANK_0)
#define ERXNDH           (0x0B | BANK_0)
#define ERXRDPTL         (0x0C | BANK_0)
#define ERXRDPTH         (0x0D | BANK_0)
#define ERXWRPTL         (0x0E | BANK_0)
#define ERXWRPTH         (0x0F | BANK_0)
#define EDMASTL          (0x10 | BANK_0)
#define EDMASTH          (0x11 | BANK_0)
#define EDMANDL          (0x12 | BANK_0)
#define EDMANDH          (0x13 | BANK_0)
#define EDMADSTL         (0x14 | BANK_0)
#define EDMADSTH         (0x15 | BANK_0)
#define EDMACSL          (0x16 | BANK_0)
#define EDMACSH          (0x17 | BANK_0)

// Bank 1 registers
#define EHT0             (0x00 | BANK_1)
#define EHT1             (0x01 | BANK_1)
#define EHT2             (0x02 | BANK_1)
#define EHT3             (0x03 | BANK_1)
#define EHT4             (0x04 | BANK_1)
#define EHT5             (0x05 | BANK_1)
#define EHT6             (0x06 | BANK_1)
#define EHT7             (0x07 | BANK_1)
#define EPMM0            (0x08 | BANK_1)
#define EPMM1            (0x09 | BANK_1)
#define EPMM2            (0x0A | BANK_1)
#define EPMM3            (0x0B | BANK_1)
#define EPMM4            (0x0C | BANK_1)
#define EPMM5            (0x0D | BANK_1)
#define EPMM6            (0x0E | BANK_1)
#define EPMM7            (0x0F | BANK_1)
#define EPMCSL           (0x10 | BANK_1)
#define EPMCSH           (0x11 | BANK_1)
#define EPMOL            (0x14 | BANK_1)
#define EPMOH            (0x15 | BANK_1)
#define EWOLIE           (0x16 | BANK_1)
#define EWOLIR           (0x17 | BANK_1)
#define ERXFCON          (0x18 | BANK_1)
#define EPKTCNT          (0x19 | BANK_1)

// Bank 2 registers
#define MACON1           (0x00 | BANK_2 | IS_MII_REG)

#define MACON3           (0x02 | BANK_2 | IS_MII_REG)
#define MACON4           (0x03 | BANK_2 | IS_MII_REG)
#define MABBIPG          (0x04 | BANK_2 | IS_MII_REG)
#define MAIPGL           (0x06 | BANK_2 | IS_MII_REG)
#define MAIPGH           (0x07 | BANK_2 | IS_MII_REG)
#define MACLCON1         (0x08 | BANK_2 | IS_MII_REG)
#define MACLCON2         (0x09 | BANK_2 | IS_MII_REG)
#define MAMXFLL          (0x0A | BANK_2 | IS_MII_REG)
#define MAMXFLH          (0x0B | BANK_2 | IS_MII_REG)
#define MAPHSUP          (0x0D | BANK_2 | IS_MII_REG)
#define MICON            (0x11 | BANK_2 | IS_MII_REG)
#define MICMD            (0x12 | BANK_2 | IS_MII_REG)
#define MIREGADR         (0x14 | BANK_2 | IS_MII_REG)
#define MIWRL            (0x16 | BANK_2 | IS_MII_REG)
#define MIWRH            (0x17 | BANK_2 | IS_MII_REG)
#define MIRDL            (0x18 | BANK_2 | IS_MII_REG)
#define MIRDH            (0x19 | BANK_2 | IS_MII_REG)

// Bank 3 registers
#define MAADR1           (0x00 | BANK_3 | IS_MII_REG)
#define MAADR0           (0x01 | BANK_3 | IS_MII_REG)
#define MAADR3           (0x02 | BANK_3 | IS_MII_REG)
#define MAADR2           (0x03 | BANK_3 | IS_MII_REG)
#define MAADR5           (0x04 | BANK_3 | IS_MII_REG)
#define MAADR4           (0x05 | BANK_3 | IS_MII_REG)
#define EBSTSD           (0x06 | BANK_3)
#define EBSTCON          (0x07 | BANK_3)
#define EBSTCSL          (0x08 | BANK_3)
#define EBSTCSH          (0x09 | BANK_3)
#define MISTAT           (0x0A | BANK_3 | IS_MII_REG)
#define EREVID           (0x12 | BANK_3)
#define ECOCON           (0x15 | BANK_3)
#define EFLOCON          (0x17 | BANK_3)
#define EPAUSL           (0x18 | BANK_3)
#define EPAUSH           (0x19 | BANK_3)

// PHY registers
#define PHCON1           0x00
#define PHSTAT1          0x01
#define PHHID1           0x02
#define PHHID2           0x03
#define PHCON2           0x10
#define PHSTAT2          0x11
#define PHIE             0x12
#define PHIR             0x13
#define PHLCON           0x14

// ENC28J60 ERXFCON Register Bit Definitions
#define ERXFCON_UCEN     0x80
#define ERXFCON_ANDOR    0x40
#define ERXFCON_CRCEN    0x20
#define ERXFCON_PMEN     0x10
#define ERXFCON_MPEN     0x08
#define ERXFCON_HTEN     0x04
#define ERXFCON_MCEN     0x02
#define ERXFCON_BCEN     0x01

// ENC28J60 EIE Register Bit Definitions
#define EIE_INTIE        0x80
#define EIE_PKTIE        0x40
#define EIE_DMAIE        0x20
#define EIE_LINKIE       0x10
#define EIE_TXIE         0x08
#define EIE_WOLIE        0x04
#define EIE_TXERIE       0x02
#define EIE_RXERIE       0x01

// ENC28J60 EIR Register Bit Definitions
#define EIR_PKTIF        0x40
#define EIR_DMAIF        0x20
#define EIR_LINKIF       0x10
#define EIR_TXIF         0x08
#define EIR_WOLIF        0x04
#define EIR_TXERIF       0x02
#define EIR_RXERIF       0x01

// ENC28J60 ESTAT Register Bit Definitions
#define ESTAT_INT        0x80
#define ESTAT_LATECOL    0x10
#define ESTAT_RXBUSY     0x04
#define ESTAT_TXABRT     0x02
#define ESTAT_CLKRDY     0x01

// ENC28J60 ECON2 Register Bit Definitions
#define ECON2_AUTOINC    0x80
#define ECON2_PKTDEC     0x40
#define ECON2_PWRSV      0x20
#define ECON2_VRPS       0x08

// ENC28J60 ECON1 Register Bit Definitions
#define ECON1_TXRST      0x80
#define ECON1_RXRST      0x40
#define ECON1_DMAST      0x20
#define ECON1_CSUMEN     0x10
#define ECON1_TXRTS      0x08
#define ECON1_RXEN       0x04
#define ECON1_BSEL1      0x02
#define ECON1_BSEL0      0x01

// ENC28J60 MACON1 Register Bit Definitions
#define MACON1_LOOPBK    0x10
#define MACON1_TXPAUS    0x08
#define MACON1_RXPAUS    0x04
#define MACON1_PASSALL   0x02
#define MACON1_MARXEN    0x01

// ENC28J60 MACON2 Register Bit Definitions
#define MACON2_MARST     0x80
#define MACON2_RNDRST    0x40
#define MACON2_MARXRST   0x08
#define MACON2_RFUNRST   0x04
#define MACON2_MATXRST   0x02
#define MACON2_TFUNRST   0x01

// ENC28J60 MACON3 Register Bit Definitions
#define MACON3_PADCFG2   0x80
#define MACON3_PADCFG1   0x40
#define MACON3_PADCFG0   0x20
#define MACON3_TXCRCEN   0x10
#define MACON3_PHDRLEN   0x08
#define MACON3_HFRMLEN   0x04
#define MACON3_FRMLNEN   0x02
#define MACON3_FULDPX    0x01

// ENC28J60 MICMD Register Bit Definitions
#define MICMD_MIISCAN    0x02
#define MICMD_MIIRD      0x01

// ENC28J60 MISTAT Register Bit Definitions
#define MISTAT_NVALID    0x04
#define MISTAT_SCAN      0x02
#define MISTAT_BUSY      0x01

// ENC28J60 PHY PHCON1 Register Bit Definitions
#define PHCON1_PRST      0x8000
#define PHCON1_PLOOPBK   0x4000
#define PHCON1_PPWRSV    0x0800
#define PHCON1_PDPXMD    0x0100

// ENC28J60 PHY PHSTAT1 Register Bit Definitions
#define PHSTAT1_PFDPX    0x1000
#define PHSTAT1_PHDPX    0x0800
#define PHSTAT1_LLSTAT   0x0004
#define PHSTAT1_JBSTAT   0x0002

// ENC28J60 PHY PHCON2 Register Bit Definitions
#define PHCON2_FRCLINK   0x4000
#define PHCON2_TXDIS     0x2000
#define PHCON2_JABBER    0x0400
#define PHCON2_HDLDIS    0x0100

// ENC28J60 Packet Control Byte Bit Definitions
#define PKTCTRL_PHUGEEN  0x08
#define PKTCTRL_PPADEN   0x04
#define PKTCTRL_PCRCEN   0x02
#define PKTCTRL_POVERRIDE 0x01

// SPI operation codes
#define ENC28J60_READ_CTRL_REG       0x00
#define ENC28J60_READ_BUF_MEM        0x3A
#define ENC28J60_WRITE_CTRL_REG      0x40
#define ENC28J60_WRITE_BUF_MEM       0x7A
#define ENC28J60_BIT_FIELD_SET       0x80
#define ENC28J60_BIT_FIELD_CLR       0xA0
#define ENC28J60_SOFT_RESET          0xFF


#endif
