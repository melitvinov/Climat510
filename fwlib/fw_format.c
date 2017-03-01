#define _FW_FORMAT_C_

#include <limits.h>
#include <string.h>
#include <stdarg.h>
#include "syntax.h"
#include "fw_wstream.h"
#include "fw_mem_wstream.h"
#include "fw_conv.h"
#include "fw_format.h"

#include "debug.h"

enum pfFlags
{
    pfMinus   = 0x01,
    pfLj      = 0x02,
    pfZpad    = 0x04,
    pfShort   = 0x08,
};

/** Put width-formatted string to outputter */
static void putww(wstream_t *stream, const char *p, int w, int len, u32 flags)
{
    int pad_len = w - len;       // Len of padding

    /** No need to pad */
    if (pad_len <= 0)
    {
        if (flags & pfMinus)
            stream->putc(stream, '-');    // Put minus before pad

        stream->puts(stream, p);             // Emit string
        return;
    }

    /** Need some padding */
    if (flags & pfLj)          // Left justify
    {
        if (flags & pfMinus)
        {
            pad_len--;
            stream->putc(stream, '-');        // Put minus
        }

        stream->puts(stream, p);             // Emit string

        while (pad_len--)
            stream->putc(stream, ' ');       // Ljustify could be padded with spaces only
    }
    else                        // Rjustify
    {
        if (flags & pfMinus)
        {
            if (flags & pfZpad)
            {
                stream->putc(stream, '-');        // Put minus
                while (--pad_len)     // Add padding
                    stream->putc(stream, '0');
            }
            else
            {
                while (--pad_len)     // Add padding
                    stream->putc(stream, ' ');
                stream->putc(stream, '-');        // Put minus
            }
        }
        else                        // No minus
        {
            int padChar = (flags & pfZpad) ? '0' : ' ';
            while (pad_len--)         // Add padding
                stream->putc(stream, padChar);
        }
        stream->puts(stream, p);   // Emit string
    }
}

/** Format string ala-printf and feed char by char to putFp */
static void format(wstream_t *stream, const char *f, va_list va)
{
    char buf[ (sizeof("4294967295") + 3) & -4];  // max decimal chars of 0xFFFFFFFF, ceiled to word
    uint flags;
    uint w;

    for (; *f; f++)
    {
        if (*f != '%')             // Just copy
        {
            stream->putc(stream, *f);
        }
        else                        // Found format's tag
        {
            flags = w = 0;
            f++;
            if (*f == '-')
            {
                flags |= pfLj;
                f++;
            }
            else if (*f == '0')    // Req to pad with zeroes
            {
                flags |= pfZpad;
                f++;
            }

#ifndef __FLOAT_PRINTF
            for (;*f >= '0' && *f <= '9'; f++)    // Collect field width
                w = w * 10 + *f - '0';
#else
            for (;(*f >= '0' && *f <= '9') || (*f == '.'); f++)    // Collect field width
            {
                if (*f == '.')
                    w = 0;
                else
                    w = w * 10 + *f - '0';
            }
#endif

            if (*f == 'h')                         // Req to print short integer
            {
                flags |= pfShort;
                f++;
            }
            else if (*f == 'l')                    // Long int is default
            {
                f++;
            }

            switch (*f)
            {
            case 'd':
            case 'i':                     // Signed decimal
                {
                    long arg = va_arg( va, long);
                    if (arg < 0)
                    {
                        arg = -arg;
                        flags |= pfMinus;
                    }
                    int len = fw_uitoa( buf, (flags & pfShort) ? (unsigned short) arg : (unsigned int) arg);
                    putww(stream, buf, w, len, flags);
                }
                break;

            case 'u':                               // Unsigned decimal
                {
                    int len = fw_uitoa( buf, (flags & pfShort) ? (unsigned short) va_arg( va, unsigned long) : va_arg( va, unsigned long) );
                    putww(stream, buf, w, len, flags);
                }
                break;

            case 'x':
            case 'X':                     // Hex
                {
                    int len = fw_uxtoa( buf, (flags & pfShort) ? (unsigned short) va_arg( va, unsigned int) : va_arg( va, unsigned int));
                    putww(stream, buf, w, len, flags);
                }
                break;

            case 's':                               // Copy string
                {
                    char *str = va_arg( va, char *);
                    putww(stream, str, w, w ? strlen( str) : 0 , flags);
                }
                break;

                // Extension for printing hex string
            case 'z':
                {
                    char *str = va_arg( va, char *);
                    for (; w--; str++)
                    {
                        int c = (*str >> 4);
                        stream->putc(stream, (c <= 9) ? c + '0' : c + 'A' - 0x0A);
                        c = *str & 0x0F;
                        stream->putc(stream, (c <= 9) ? c + '0' : c + 'A' - 0x0A);
                    }
                }
                break;

            case 'c':                               // Copy char
                stream->putc(stream, va_arg( va, int));
                break;

            case 'p':                               // Copy ptr address
                {
                    int len = fw_uxtoa( buf, va_arg( va, unsigned long));
                    putww(stream, buf, w, len, flags);
                }
                break;

            case '%':                               // Emit '%'
                stream->putc(stream, '%');
                break;

#ifdef __FLOAT_PRINTF
            // Quick and dirty hack to visualize floats somehow, disrespecting format length specifiers
            case 'f':
                wstream_putf(stream, va_arg(va, double));
                break;
#endif

            default:
            case '\0':                              // EOS
                REQUIRE(0);
                return;
            }
        }
    }
}

int fw_sprintf(char *dst, const char *fmt, ...)
{
    mem_wstream_t stream;
    mem_wstream_open(&stream, (u8*)dst, INT_MAX);  // virtually unlimited stream (there is not enough device RAM for INT_MAX size buffer anyway :-)
    va_list va;
    va_start(va, fmt);
    format(&stream.base, fmt, va);
    va_end(va);
    stream.base.putc(&stream.base, '\0');
    return mem_wstream_tell(&stream) - 1;
}

void fw_printf(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    format(&dbg_stream, fmt, va);
    va_end(va);
}

void fw_stream_format(wstream_t *stream, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    format(stream, fmt, va);
    va_end(va);
}
