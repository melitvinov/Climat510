#include "syntax.h"

#include "stm32f10x.h"
#include "hal_pincfg.h"
#include "hal_sys.h"
#include "hal_systimer.h"
#include "hw_flags.h"
#include "enc28j60.h"
#include "hal_mac.h"

#include "debug.h"

//--

#define MAX_PKT_SIZE    (HAL_ETH_MTU + HAL_ETH_HDR_SIZE)
#define MAX_FRAMELEN    (MAX_PKT_SIZE + 4)    // + crc

// space for storing one tx packet, including the control byte, very packet of max framelen and 7-byte status vector
#define MAX_TX_PKT_SPACE  (MAX_FRAMELEN + 1 + 7)

// ethernet chip memory layout:
// rx buffer start must be at 0 according to errata 5. one tx packet is enough to for blocking tx, so
// we allocate a single tx buffer end of the memory. remaining memory are for rx buffers
#define MEM_BASE        0x0000
#define MEM_SIZE        0x2000

#define TXMEM_BASE    (MEM_SIZE - MAX_TX_PKT_SPACE)
#define TXMEM_LAST    (MEM_SIZE - 1)

#define RXMEM_BASE    0x0000
#define RXMEM_LAST    (TXMEM_BASE - 1)

// datasheet recommends tx mem base be even
PANIC_IF(TXMEM_BASE % 2);

#define SPI_XFER for (int _todo __cleanup(spi_release) = spi_access(); _todo; _todo = 0)

// 2 bytes are prepadding to make eth mtu land on a word boundary
typedef struct __aligned(4)
{
    u8 data[2 + MAX_PKT_SIZE + HAL_ETH_BUF_EXTRA_TAIL];
} mac_buf_t;

typedef struct
{
    u16 rx_head;
    u8 current_bank;
    u8 used_bufs;
    mac_buf_t bufs[2];
} mac_rt_t;

static SPI_TypeDef *const spi = SPI1;

static mac_rt_t rt;

// --- spi stuff

static inline void spi_sync(void)
{
    while ((spi->SR & (SPI_SR_TXE | SPI_SR_BSY)) != (SPI_SR_TXE));
    spi->DR;
    spi->SR;
}

static inline int spi_access(void)
{
    GPIOD->BSRR = 1 << (2 + 16);
    return 1;
}

static inline void spi_release(int *dummy)
{
    spi_sync();
    // at least 210 ns are required after accessing mac and mii registers.
    // one nop is 28 ns, 8 nops (+ spi registers access inside the sync) are > 220 ns
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    GPIOD->BSRR = 1 << 2;
}

static void spi_init(void)
{
    GPIOD->BSRR = 1 << 2;           // cs
    hal_pincfg_out(GPIOD, 2);

    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    hal_pincfg_out_alt(GPIOA, 5);   // sck
    hal_pincfg_out_alt(GPIOA, 7);   // mosi
    hal_pincfg_in(GPIOA, 6);        // miso, floating

    spi->CR1 = 0;
    spi->CR2 = 0;
    spi->I2SCFGR = 0;

    // spi mode 0, clock = fCPU / 2 = 36 / 2 = 18 MHz
    // maximum spi frequency for ethernet chip is 20 MHz according to the datasheet
    REQUIRE(HAL_SYS_F_CPU / 4. <= 20E6);
    spi->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE | SPI_CR1_MSTR;
    spi_sync();
}

static void spi_write(const u8 *src, uint len)
{
    for (; len; len--)
    {
        while (! (spi->SR & SPI_SR_TXE));
        spi->DR = *src++;
    }
}

static void spi_read(u8 *dst, uint len)
{
    spi_sync();

    for (; len; len--)
    {
        spi->DR = 0;
        while (! (spi->SR & SPI_SR_RXNE));
        *dst++ = spi->DR;
    }
}

static void spi_txrx(const void *tx, uint txlen, void *rx, uint rxlen)
{
    SPI_XFER
    {
        if (txlen)
            spi_write(tx, txlen);
        if (rxlen)
            spi_read(rx, rxlen);
    }
}

// this one accesses spi too, since op and src memories are are expensive to glue together
static void stream_to_buf_mem(const void *src, uint len)
{
    const u8 op = ENC28J60_WRITE_BUF_MEM;
    SPI_XFER
    {
        spi_write(&op, 1);
        spi_write(src, len);
    }
}

static void stream_from_buf_mem(void *dst, uint len)
{
    const u8 op = ENC28J60_READ_BUF_MEM;
    spi_txrx(&op, 1, dst, len);
}

static void set_bank(u8 addr)
{
    u8 bank = (addr & BANK_MASK) >> BANK_MASK_IDX;
    u8 banked_addr = addr & ADDR_MASK;

    if (banked_addr >= SHARED_REGS_OFFSET)
        return;

    if (bank == rt.current_bank)
        return;

    const u8 to_clr[] = {ENC28J60_BIT_FIELD_CLR | ECON1, ECON1_BSEL1 | ECON1_BSEL0};
    const u8 to_set[] = {ENC28J60_BIT_FIELD_SET | ECON1, bank};

    spi_txrx(to_clr, 2, NULL, 0);
    spi_txrx(to_set, 2, NULL, 0);

    rt.current_bank = bank;
}


static void set_eth_bits(u8 addr, u8 mask)
{
    set_bank(addr);

    const u8 tx[] = {ENC28J60_BIT_FIELD_SET | (addr & ADDR_MASK), mask};
    spi_txrx(tx, 2, NULL,  0);
}

static void clr_eth_bits(u8 addr, u8 mask)
{
    set_bank(addr);

    const u8 tx[] = {ENC28J60_BIT_FIELD_CLR | (addr & ADDR_MASK), mask};
    spi_txrx(tx, 2, NULL, 0);
}

static void reset_chip(void)
{
    const u8 op = ENC28J60_SOFT_RESET;
    spi_txrx(&op, 1, NULL, 0);
}

static void write_control_reg(u8 address, u8 data)
{
    set_bank(address);

    const u8 tx[] = {ENC28J60_WRITE_CTRL_REG | (address & ADDR_MASK), data};
    spi_txrx(tx, 2, NULL, 0);
}

static u8 read_control_reg(u8 addr)
{
    set_bank(addr);

    const u8 tx[] = {ENC28J60_READ_CTRL_REG | (addr & ADDR_MASK)};

    if (! (addr & IS_MII_REG))
    {
        u8 rx;
        spi_txrx(tx, 2, &rx, 1);
        return rx;
    }
    else    // one dummy read is required for these
    {
        u8 rx[2];
        spi_txrx(tx, 2, rx, 2);
        return rx[1];
    }
}

static void write_phy_reg(u8 addr, u16 data)
{
    write_control_reg(MIREGADR, addr);
    write_control_reg(MIWRL, data);
    write_control_reg(MIWRH, data >> 8);

    // wait until the PHY write completes
    while (read_control_reg(MISTAT) & MISTAT_BUSY);
}


__unused static u16 read_phy_reg(u8 addr)
{
    write_control_reg(MIREGADR, addr);
    write_control_reg(MICMD, MICMD_MIIRD);

    while (read_control_reg(MISTAT) & MISTAT_BUSY);

    write_control_reg(MICMD, 0);

    u16 out = read_control_reg(MIRDL);
    out |= read_control_reg(MIRDH) << 8;
    return out;
}

//-- buffer allocator. there is just two buffers, so logic is simple

void *HAL_mac_alloc_buf(void)
{
    PANIC_IF(countof(rt.bufs) != 2);
    static const s8 next_free_buf[4] = {0, 1, 0, -1};
    int pos = next_free_buf[rt.used_bufs];
    if (pos < 0)
    {
        WARN("Failed to alloc buffer");
        return NULL;
    }
    rt.used_bufs |= 1 << pos;
    // +2 is to make data aligned for the next layers after ethernet
    return rt.bufs[pos].data + 2;
}

// Free buf. Addr may point to any byte of buffer, not just beginning
void HAL_mac_free_buf(void *addr)
{
    uint pos =  ((u8 *)addr - (u8 *)rt.bufs) / sizeof(rt.bufs[0]);
    if (pos >= countof(rt.bufs))
    {
        REQUIRE(0);
        return;
    }

    uint mask = 1 << pos;
    if (!(rt.used_bufs & mask))
    {
        REQUIRE(0);
        return;
    }
    rt.used_bufs &= ~mask;
}


void HAL_mac_init(const u8 *mac_addr, u32 flags)
{
    if (flags & PHY_FULL_DUPLEX)
        LOG("starting phy in full-duplex mode");
    if (flags & MAC_ACCEPT_ANY_UNICAST)
        WARN("this mac doesn't support 'any unicast' filter");
    if (flags & PHY_100MBIT)
        WARN("this phy doesn't support 100 mbit");
    if (flags & PHY_AUTO_NEGOTIATION)
        WARN("this phy doesn't support auto negotiation");
    if (flags & PHY_LOOPBACK)
        WARN("loopback is not implemented");

    spi_init();

    // wait >=1 ms after reset, as suggested in errata 1.2
    reset_chip();
    hal_systimer_sleep_us(2000);

    u8 rev = read_control_reg(EREVID);
    LOG("ethernet chip revision register: 0x%02x", rev);

    // bank 0

    rt.rx_head = RXMEM_BASE;

    write_control_reg(ERXSTL, RXMEM_BASE & 0xFF);
    write_control_reg(ERXSTH, RXMEM_BASE >> 8);
    write_control_reg(ERXNDL, RXMEM_LAST & 0xFF);
    write_control_reg(ERXNDH, RXMEM_LAST >> 8);
    // these should be even, errata 14
    write_control_reg(ERXRDPTL, RXMEM_LAST & 0xFF);
    write_control_reg(ERXRDPTH, RXMEM_LAST >> 8);
    // tx
    write_control_reg(ETXSTL, TXMEM_BASE & 0xFF);
    write_control_reg(ETXSTH, TXMEM_BASE >> 8);

    // bank 1

    u8 val = ERXFCON_UCEN | ERXFCON_CRCEN;
    if (flags & MAC_ACCEPT_MULTICAST)
        val |= ERXFCON_MCEN;
    if (flags & MAC_ACCEPT_BROADCAST)
        val |= ERXFCON_BCEN;

    write_control_reg(ERXFCON, val);

    // bank 2

    write_control_reg(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
    // enable automatic padding to 60bytes and CRC operations, report frame len, full duplex

    val = MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN;
    if (flags & PHY_FULL_DUPLEX)
        val |= MACON3_FULDPX;

    write_control_reg(MACON3, val);

    write_control_reg(MACON4, 0);
    // back-to-back inter-packed gap. 0x15 for full duplex, 0x12 for half
    write_control_reg(MABBIPG, (flags & PHY_FULL_DUPLEX) ? 0x15 : 0x12);
    // set inter-frame gap (non-back-to-back)
    write_control_reg(MAIPGL, 0x12);

    if (! (flags & PHY_FULL_DUPLEX))
        write_control_reg(MAIPGH, 0x0C);

    write_control_reg(MAMXFLL, MAX_FRAMELEN & 0xFF);
    write_control_reg(MAMXFLH, MAX_FRAMELEN >> 8);

    // bank 3

    write_control_reg(MAADR5, mac_addr[0]);
    write_control_reg(MAADR4, mac_addr[1]);
    write_control_reg(MAADR3, mac_addr[2]);
    write_control_reg(MAADR2, mac_addr[3]);
    write_control_reg(MAADR1, mac_addr[4]);
    write_control_reg(MAADR0, mac_addr[5]);
    // no clock output
    write_control_reg(ECOCON, 0);

    // phy
    write_phy_reg(PHCON1, (flags & PHY_FULL_DUPLEX) ? PHCON1_PDPXMD : 0);
    // leds: led b displays tx/rx activity, led a displays link status
    write_phy_reg(PHLCON, 0x3472);

    set_eth_bits(ECON2, ECON2_AUTOINC);

    // go
    set_eth_bits(ECON1, ECON1_RXEN);
}


uint HAL_mac_write_packet(void *data, uint len)
{
    REQUIRE(len <= MAX_PKT_SIZE);

    // passed data is nbuf. there is two prepadding bytes.
    // be smartass and utilize one of them as a required control byte
    u8 *p = data;
    p -= 1;
    len += 1;
    *p = 0;       // control byte 0 means use default, no overrides

    write_control_reg(EWRPTL, TXMEM_BASE & 0xFF);
    write_control_reg(EWRPTH, TXMEM_BASE >> 8);

    stream_to_buf_mem(p, len);

    const uint last = TXMEM_BASE + len - 1;
    write_control_reg(ETXNDL, last & 0xFF);
    write_control_reg(ETXNDH, last >> 8);

    clr_eth_bits(EIR, EIR_TXIF | EIR_TXERIF);
    set_eth_bits(ECON1, ECON1_TXRTS);

    // wait for the end of transmission to simplify buffer management.
    // transfer should fast enough for non-realtime system, < 2ms for 1500 byte payload.
    //
    // we check if transmission has failed, reset the transmitter and try again

    uint is_ok = 0;

    uint tries = 16;
    while (1)
    {
        u8 eir =  read_control_reg(EIR);
        if (! (eir & (EIR_TXIF | EIR_TXERIF)))
            break;

        clr_eth_bits(ECON1, ECON1_TXRTS);

        if (! (eir & EIR_TXERIF))
        {
            is_ok = 1;
            break;
        }

        set_eth_bits(ECON1, ECON1_TXRST);
        clr_eth_bits(ECON1, ECON1_TXRST);
        clr_eth_bits(EIR, EIR_TXERIF | EIR_TXIF);

        if (--tries)
        {
            WARN("transmission error, retry");
            set_eth_bits(ECON1, ECON1_TXRTS);
        }
        else
        {
            WARN("failed to send packet");
            break;
        }
    }

    HAL_mac_free_buf(data);
    return is_ok;
}


void *HAL_mac_read_packet(uint *len)
{
    // reliable way to see if there is a packet (errata 6)
    if (read_control_reg(EPKTCNT) == 0)
        return NULL;

    u8 *ret = NULL;

    struct __packed
    {
        u16 next_pkt;
        u16 len;
        u16 rxstat;
    } preamble;

    write_control_reg(ERDPTL, rt.rx_head);
    write_control_reg(ERDPTH, rt.rx_head >> 8);

    stream_from_buf_mem(&preamble, sizeof(preamble));

    rt.rx_head = preamble.next_pkt;
    uint rxlen = preamble.len - 4; // shave off crc

    // bit 7 signals 'packet is ok'
    if ((preamble.rxstat & 0x80) && rxlen > 0 && rxlen <= (MAX_PKT_SIZE))
    {
        ret = HAL_mac_alloc_buf();
        if (! ret)
        {
            WARN("failed to alloc buf, dropping packet");
        }
        else
        {
            stream_from_buf_mem(ret, rxlen);
            *len = rxlen;
        }
    }
    else
    {
        WARN("bad packet, ignoring");
    }

    // advance the read pointer to lag one byte behind and be even (errata 14)
    uint rd = rt.rx_head;
    if (rd == 0)
        rd = RXMEM_LAST;
    else
        rd -= 1;

    write_control_reg(ERXRDPTL, rd);
    write_control_reg(ERXRDPTH, rd >> 8);

    // decrement the packet counter to indicate we are done with this packet
    set_eth_bits(ECON2, ECON2_PKTDEC);

    return ret;
}

void HAL_mac_periodic(void)
{
    ;
}
