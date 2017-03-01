#define _FW_STREAM_C_

#include <string.h>
#include <fastmath.h>
#include "syntax.h"
#include "fw_encode_tables.h"
#include "fw_wstream.h"
#include "fw_dtoa.h"
#include "debug.h"

void wstream_puts(wstream_t *stream, const char *str)
{
    stream->puts(stream, str);
}

void wstream_putc(wstream_t *stream, u8 byte)
{
    stream->putc(stream, byte);
}

void wstream_putb(wstream_t *stream, const void *str, uint len)
{
    if (stream->putb)
    {
        stream->putb(stream, str, len);
    }
    else
    {
        const u8 *buf = str;
        while (len--)
            stream->putc(stream, *buf++);
    }
}

void wstream_putu(wstream_t *stream, uint val)
{
    char buf[(sizeof("4294967295") + 3) & -4];  // max decimal chars of 0xFFFFFFFF, ceiled to word
    char *p = buf + sizeof(buf) - 1;
    *p-- = '\0';
    do
    {
        *p-- = val % 10 + '0';                 // Do divisions, put to buf
    } while (val /= 10);
    stream->puts(stream, p + 1);
}

void wstream_putd(wstream_t *stream, int val)
{
    if (val >= 0)
    {
        wstream_putu(stream, val);
    }
    else
    {
        stream->putc(stream, '-');
        wstream_putu(stream, -val);
    }
}

void wstream_putx(wstream_t *stream, u32 val)
{
    char buf[(sizeof("FFFFFFFF") + 3) & -4];                // max chars of 0xFFFFFFFF, ceiled to word
    char *p = buf + sizeof(buf) - 1;
    *p-- = '\0';
    do
    {
        u32 tmp = val % 16;
        *p-- = (tmp <= 9) ? tmp + '0' : tmp + 'A' - 0x0A;
    } while (val /= 16);
    stream->puts(stream, "0x");
    stream->puts(stream, p + 1);
}

void wstream_putbx(wstream_t *stream, const void *str, uint len)
{
    const u8 *buf = str;
    while (len--)
    {
        uint c = *buf++;
        uint h = c >> 4;
        uint l = c & 0x0F;
        l += (l <= 9) ? '0' : 'A' - 0x0A;
        h += (h <= 9) ? '0' : 'A' - 0x0A;
        stream->putc(stream, h);
        stream->putc(stream, l);
    }
}
