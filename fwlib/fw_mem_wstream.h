#ifndef _FW_MEM_WSTREAM_H_
#define _FW_MEM_WSTREAM_H_

#include "fw_wstream.h"

// stream for write to flat buffer (inherited from stream_t)
typedef struct mem_wstream_t mem_wstream_t;
struct mem_wstream_t
{
    wstream_t base; // should be a first element in struct for inheritance
    int pos;        // current position
    int eos;        // end of stream position (index of byte after the last one)
    int ovf;        // overflow flag
    u8 *mem;
};

void mem_wstream_open(mem_wstream_t *stream, u8 *buf, int eos);
void mem_wstream_close(mem_wstream_t *stream);
void mem_wstream_seek(mem_wstream_t *stream, int pos);
int mem_wstream_tell(const mem_wstream_t *stream);
int mem_wstream_eos(const mem_wstream_t *stream);
int mem_wstream_ovf(const mem_wstream_t *stream);
void mem_wstream_reset(mem_wstream_t *stream);

#endif
