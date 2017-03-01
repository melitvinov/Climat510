#define _FW_CONV_C_

#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>
#include "syntax.h"
#include "fw_string.h"
#include "fw_conv.h"
#include "debug.h"

#define SET_ERRNO   0

uint fw_atou(const char *str, u32 *val)
{
    const char *p;

    str = skip_lspaces(str);
    for (p = str; isdigit((int)*p); p++);           // rewind to the end of valid string
    if (p == str) return 0;                     // no digits at all

    uint dec_cnt = 1;
    u32 out = 0;
    for (p--; p >= str; p--, dec_cnt *= 10)    // p points to the last symbol at the loop start
        out += (*p - '0') * dec_cnt;

    *val = out;
    return 1;
}

uint fw_atoi(const char *str, s32 *val)
{
    uint out;

    str = skip_lspaces(str);
    if (str[0] == '-')
    {
        if (!decstr_to_uint( &str[1], &out)) return 0;
        *val = -out;
        return 1;
    }
    if (str[0] == '+')
        str++;
    if (!decstr_to_uint( str, &out)) return 0;
    *val = out;
    return 1;
}

long fw_strtol10(const char *str, char **tail)
{
    const char *s = str;
    const char *p;

    while (isspace((int)*s)) // skip leading spaces
        s++;

    uint is_neg = 0;
    if (*s == '-')
    {
        is_neg = 1;
        s++;
    }
    else if (*s == '+')
    {
        s++;
    }

    for (p = s; isdigit((int)*p); p++);     // rewind to the end of valid string
    if (p == s)
    {
        *tail = (char *)str;
        return 0;                           // failed to find any digits
    }

    *tail = (char *)p;
    uint is_ovf = 0;
    uint dec_cnt = 1;
    ulong out = 0;
    p--;                                    // now p points to the last valid symbol
    for (; p >= s; p--, dec_cnt *= 10)
    {
        out += (*p - '0') * dec_cnt;
        if (out > LONG_MAX)                 // clamp. we do not care that LONG_MAX and LONG_MIN are different to keep things simple
        {
            out = LONG_MAX;
            is_ovf = 1;
        }
    }

    if (SET_ERRNO && is_ovf)
        errno = ERANGE;
    return is_neg ? -out : out;
}

/** No check for overflow */
uint decstr_to_uint(const char *str, uint *val)
{
    uint dec_cnt = 1;
    u32 out = 0;
    const char *p;

    if ( !*str) return 0;  // Empty

    for (p = str; *p; p++);                    // Rewind to the end of string
    for (p--; p >= str; p--, dec_cnt *= 10)    // p points to the last symbol at the loop start
    {
        if ( !isdigit((int)*p)) return 0;
        out += (*p - '0') * dec_cnt;
    }
    *val = out;
    return 1;
}

uint decstr_to_int( const char *str, int *val)
{
    uint out;

    if (*str == '-')
    {
        if (decstr_to_uint( ++str, &out))
        {
            *val = -out;
            return 1;
        }
    }
    else if (*str)
    {
        if ( decstr_to_uint( str, &out))
        {
            *val = out;
            return 1;
        }
    }
    return 0;
}

static int hexdig_to_uint(int c)
{
    switch (c)
    {
    case 'a' ... 'f': return c -= 'a' - 0xA;
    case 'A' ... 'F': return c -= 'A' - 0xA;
    case '0' ... '9': return c -= '0';
    default:          return -1;
    }
}

uint hexstr_to_uint( const char *str, u32 *val)
{
    int shift = 0;
    s32 out = 0;
    const char *p;

    if ( !*str) return 0;  // Empty

    for ( p = str; *p; p++);                  // Rewind to the end of string
    for ( p--; p >= str && shift < 32; p--, shift += 4)     // locStrP points to the last symbol at the loop start
    {
        int c = *p;

        c = hexdig_to_uint(c);
        if (c < 0) return 0;
        out += c << shift;
    }

    *val = out;
    return 1;
}

u32 short_to_bcd(s32 val)
{
    u32 in;
    u32 out;

    /** assuming string is short: -32768..32767 */
    if ( val < 0)
    {
        out = 0x0D;    // put '-' in zero nibble
        in = -val;
    }
    else
    {
        out = 0x0C;    // +
        in = val;
    }

    out |= (in % 10) << 20;     // 10000
    in /= 10;

    out |= (in % 10) << 16;     // 1000
    in /= 10;

    out |= (in % 10) << 12;     // 100
    in /= 10;

    out |= (in % 10) << 8;     // 10
    in /= 10;

    out |= in << 4;            // 1

    return out;
}


const char *short_to_decstr(char *str, s32 val)
{
    u32 in;
    /** assuming string is short: -32768..32767 */
    if (val < 0)
    {
        *str = '-';
        in = -val;
    }
    else
    {
        *str = ' ';
        in = val;
    }

    *(str += 6) = 0;
    *(--str) = in % 10 + '0';     // 10000
    *(--str) = (in /= 10) % 10 + '0';     // 1000
    *(--str) = (in /= 10) % 10 + '0';     // 100
    *(--str) = (in /= 10) % 10 + '0';     // 10
    *(--str) = in/10 + '0';

    return --str;
}


const char *char_to_decstr( char *str, s32 val)
{
    u32 in;
    /** assuming string is char: -128..127 */
    if ( val < 0)
    {
        *str = '-';
        in = -val;
    }
    else
    {
        *str = ' ';
        in = val;
    }

    *(str += 4) = 0;
    *(--str) = in % 10 + '0';             // 100
    *(--str) = (in /= 10) % 10 + '0';     // 10
    *(--str) = in/10 + '0';

    return --str;
}

char *jsonstr_to_utf8(char *out, const char *in)
{
    char *p = out;

    while (1)
    {
        int c = *in++;

        if (c < ' ') break;

        if (c != '\\')  // non-escaped
        {
            *p++ = c;
        }
        else            // escaped
        {
            c = *in++;

            if (c < ' ') break;

            if (c == 'u')
            {
                uint code = 0;

                c = hexdig_to_uint(*in++);
                if (c < 0) break;
                code |= c << 12;
                c = hexdig_to_uint(*in++);
                if (c < 0) break;
                code |= c << 8;
                c = hexdig_to_uint(*in++);
                if (c < 0) break;
                code |= c << 4;
                c = hexdig_to_uint(*in++);
                if (c < 0) break;
                code |= c << 0;

                // encode utf
                if (code > 0x7FF)  // 16 bits
                {
                    *p++ = ((code >> 12) & 0x0F) | 0xE0;
                    *p++ = ((code >> 6) & 0x3F) | 0x80;
                    *p++ = (code & 0x3F) | 0x80;
                }
                else if (code > 0x07F)
                {
                    *p++ = ((code >> 6) & 0x1F) | 0xC0;
                    *p++ = (code & 0x3F) | 0x80;
                }
                else
                {
                    *p++ = code;
                }
            }
            else
            {
                if (c == 'b')
                    c = '\b';
                else if (c == 'f')
                    c = '\f';
                else if (c == 'n')
                    c = '\n';
                else if (c == 'r')
                    c = '\r';
                else if (c == 't')
                    c = '\t';
                else if (c == '"' || c == '\\' || c == '/')
                    ;   // copy verbatim
                else
                    break;  //  unrecognized escape

                *p++ = c;
            }
        }
    }

    *p = '\0';
    return out;
}

int utf8_chr(char *out, uint16_t input, int out_size)
{
    char *p = out;
    if (input > 0x7FF)  // 16 bits
    {
        if (out_size < 3) return 0;
        *p++ = ((input >> 12) & 0x0F) | 0xE0;
        *p++ = ((input >> 6) & 0x3F) | 0x80;
        *p++ = (input & 0x3F) | 0x80;
    }
    else if (input > 0x07F)
    {
        if (out_size < 2) return 0;
        *p++ = ((input >> 6) & 0x1F) | 0xC0;
        *p++ = (input & 0x3F) | 0x80;
    }
    else
    {
        if (out_size < 1) return 0;
        *p++ = input;
    }
    return p - out;
}

static int utf8_next(const char **str)
{
    const char *s = *str;
    int out = -1;
    if ((*s & 0x80) == 0)
    {
        out = *s++;
    }
    else if ((*s & 0xE0) == 0xC0)   // 2 bytes
    {
        uint c = (*s++ & 0x1F) << 6;
        if ((*s & 0xC0) == 0x80)
        {
            c |= *s++ & 0x3F;
            out = c;
        }
    }
    else if ((*s & 0xF0) == 0xE0)   // 3 bytes
    {
        uint c = (*s++ & 0x0F) << 12;
        if ((*s & 0xC0) == 0x80)
        {
            c |= (*s++ & 0x3F) << 6;
            if ((*s & 0xC0) == 0x80)
            {
                c |= *s++ & 0x3F;
                out = c;
            }
        }
    }
    else
    {
        s++;
    }
    *str = s;
    return out;
}

void utf16_str_to_utf8(char *out, const u16 *in, uint out_size)
{
    REQUIRE(out_size);
    out_size -= 1;
    while (*in)
    {
        int len = utf8_chr(out, *in++, out_size);
        if (! len)
            break;
        out += len;
        out_size -= len;
    }
    *out = '\0';
}

void utf8_str_to_utf16(u16 *out, const char *in, uint out_size)
{
    REQUIRE(out_size);
    out_size -= 1;
    while (out_size--)
    {
        int chr = utf8_next(&in);
        if (! chr)
            break;
        if (chr < 0)
            chr = '?';
        *out++ = chr;
    }
    *out = '\0';
}

const char * char_to_decstr_nozeroes_nopad( char *str, s32 val)
{
    u32 in;
    int is_negative = 0;
    /** assuming string is char: -128..127 */

    if ( val < 0)
    {
        is_negative = 1;
        in = -val;
    }
    else
    {
        in = val;
    }

    *(str += 4) = 0;
    *(--str) = in % 10 + '0';          // units

    if ( in /= 10 )
    {
        *(--str) = in % 10 + '0';      // decimals

        if ( in /= 10 )
            *(--str) = in + '0';       // hundreds
    }

    if (is_negative)
        *(--str) = '-';
    return str;
}

/** Converts unsigned long to string. Returns number of symbols printed */
int fw_uitoa(char *str, u32 val)
{
    char buf[ (sizeof("4294967295") + 3) & -4];  // max decimal chars of 0xFFFFFFFF, ceiled to word
    char *p = buf;
    int len;

    do
    {
        *p++ = val % 10 + '0';                 // Do divisions, put to buf in reverse order
    } while (val /= 10);

    len = p - buf;                            // We have a full integer in buf now. Units go first. Do a reverse copy

    while (p > buf)
        *str++ = *(--p);

    *str = '\0';
    return len;
}

int fw_uxtoa(char *str, u32 val)
{
    char buf[ (sizeof("FFFFFFFF") + 3) & -4];                // max chars of 0xFFFFFFFF, ceiled to word
    char *p = buf;
    int len;

    do
    {
        uint tmp = val % 16;
        *p++ = (tmp <= 9) ? tmp + '0' : tmp + 'A' - 10;     // Do divisions, put to buf in reverse order
    } while (val /= 16);

    len = p - buf;                                        // We have a full integer in buf now. Units go first. Do a reverse copy

    while (p > buf)
        *str++ = *(--p);

    *str = '\0';
    return len;
}
