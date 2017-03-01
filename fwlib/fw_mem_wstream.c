#define _FW_MEM_WSTREAM_C_

#include "syntax.h"
#include "fw_mem_wstream.h"

static void mem_wstream_putc(wstream_t *base, u8 byte)
{
    mem_wstream_t *me = (mem_wstream_t *)base;
    if (me->ovf) return;

    int pos = me->pos;
    const int eos = me->eos;

    if (pos >= eos) // got overflow
    {
        me->ovf = 1;
        return;
    }

    if (pos >= 0)   // negative pos is valid for skipping part of buffer
        me->mem[pos] = byte;
    me->pos = pos + 1;
}

// put zero-terminated string
static void mem_wstream_puts(wstream_t *base, const char *str)
{
    mem_wstream_t *me = (mem_wstream_t *)base;
    if (me->ovf) return;

    const int eos = me->eos;                // assume eos is non-negative
    for (; me->pos < 0; me->pos++, str++)   // skip initial part of buffer
    {
        if (!*str) return;
    }

    for (; me->pos < eos; me->pos++, str++)
    {
        if (!*str) return;
        me->mem[me->pos] = *str;
    }

    if (*str)                               // attempt to write after eos
    {
        me->ovf = 1;
        return;
    }
}

static void mem_wstream_putb(wstream_t *base, const u8 *buf, uint len)
{
    mem_wstream_t *me = (mem_wstream_t *)base;
    if (me->ovf) return;

    const int eos = me->eos;                        // assume eos is non-negative
    for (; me->pos < 0; me->pos++, buf++, len--)    // skip initial part of buffer
    {
        if (!len) return;
    }

    for (; me->pos < eos; me->pos++, buf++, len--)
    {
        if (!len) return;
        me->mem[me->pos] = *buf;
    }

    if (len)                               // attempt to write after eos
    {
        me->ovf = 1;
        return;
    }
}

static void mem_wstream_putc_null(wstream_t *base, u8 byte)
{
    mem_wstream_t *me = (mem_wstream_t *)base;
    if (me->ovf) return;

    int pos = me->pos;
    const int eos = me->eos;

    if (pos >= eos) // got overflow
    {
        me->ovf = 1;
        return;
    }

    me->pos = pos + 1;
}

static void mem_wstream_puts_null(wstream_t *base, const char *str)
{
    mem_wstream_t *me = (mem_wstream_t *)base;
    if (me->ovf) return;

    const int eos = me->eos;                // assume eos is non-negative
    for (; me->pos < eos; me->pos++, str++)
    {
        if (!*str) return;
    }

    if (*str)                               // attempt to write after eos
    {
        me->ovf = 1;
        return;
    }
}

static void mem_wstream_putb_null(wstream_t *base, const u8 *buf, uint len)
{
    mem_wstream_t *me = (mem_wstream_t *)base;
    if (me->ovf) return;

    const int eos = me->eos;                // assume eos is non-negative
    for (; me->pos < eos; me->pos++, buf++, len--)
    {
        if (!len) return;
    }

    if (len)                               // attempt to write after eos
    {
        me->ovf = 1;
        return;
    }
}

// eos may be set only at open
void mem_wstream_open(mem_wstream_t *stream, u8 *buf, int eos)
{
    stream->mem = buf;
    stream->pos = 0;
    stream->eos = eos;
    stream->ovf = 0;
    stream->base.putc = buf ? mem_wstream_putc : mem_wstream_putc_null;
    stream->base.puts = buf ? mem_wstream_puts : mem_wstream_puts_null;
    stream->base.putb = buf ? mem_wstream_putb : mem_wstream_putb_null;
}

void mem_wstream_close(mem_wstream_t *stream)
{
    stream->eos = 0;    // this prevent any writes
}

// eos is not checked
void mem_wstream_seek(mem_wstream_t *stream, int pos)
{
    stream->pos = pos;
}

void mem_wstream_reset(mem_wstream_t *stream)
{
    stream->pos = 0;
    stream->ovf = 0;
}

// return actual pos
int mem_wstream_tell(const mem_wstream_t *stream)
{
    return stream->pos;
}

// check end of stream
int mem_wstream_eos(const mem_wstream_t *stream)
{
    return stream->pos >= stream->eos;
}

int mem_wstream_ovf(const mem_wstream_t *stream)
{
    return stream->ovf;
}
