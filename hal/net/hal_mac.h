#ifndef _HAL_MAC_H_
#define _HAL_MAC_H_

// MTU of ethernet
#define HAL_ETH_MTU             1500
#define HAL_ETH_HDR_SIZE        14
// reserved bytes at the end of buffer (for appending zero-terminators to http steam etc)
#define HAL_ETH_BUF_EXTRA_TAIL  4

void HAL_mac_init(const u8 *mac_addr, u32 flags);
void *HAL_mac_read_packet(uint *len);
uint HAL_mac_write_packet(void *data, uint len);
void HAL_mac_periodic(void);

void *HAL_mac_alloc_buf(void);
void HAL_mac_free_buf(void *addr);

#endif
