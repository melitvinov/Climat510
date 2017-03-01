#define _FW_STRING_C_

#include <string.h>
#include <ctype.h>
#include "syntax.h"
#include "fw_string.h"

// Copy string to fixed len string (appending terminating zero)
// Len do not include space for terminating zero
char * strfcpy( char *dst, const char *src, uint len)
{
    uint pos;

    for (pos = 0; *src && pos < len; src++, dst++, pos++) // Copy
        *dst = *src;

    while (pos++ < len)    // Append whites
        *dst++ = ' ';

    *dst = '\0';           // Terminate
    return dst - len;
}

// Copy string to fixed len string (appending terminating zero)
// Size is including space for terminating zero
void strsafecpy(char *dst, const char *src, uint size)
{
    if (!size) return;
    size -= 1;
    for (uint cnt = 0; *src && (cnt < size); src++, dst++, cnt++) // Copy
        *dst = *src;
    *dst = '\0';           // Terminate
}

char *rstrip_spaces(char *str)
{
    char *last_symb = str;

    for (char *p = str; *p; p++)
    {
        if ( !isspace((int)*p) )
            last_symb = p + 1;
    }
    *last_symb = '\0';

    return str;
}

char *lstrip_spaces(char *str)
{
    return str += strspn(str, " \t\n\v\f\r");    // Trim left whites
}

const char *skip_lspaces(const char *str)
{
    for (; *str == ' '; str++);
    return str;
}

char *strip_spaces(char *str)
{
    return rstrip_spaces((char *)lstrip_spaces(str));
}

char *lowcase(char *str)
{
    for (char *s = str; (*s = tolower((int)*s)); s++);
    return str;
}

char *upcase(char *str)
{
    for (char *s = str; (*s = toupper((int)*s)); s++);
    return str;
}

char *compress_spaces( char *str)
{
    char *w = str;
    char *s = str;
    int in_spaces = 0;

    for (; *s; s++)
    {
        if (!isspace((int)*s))   // Non-space character, copy it
        {
            in_spaces = 0;
        }
        else                // Space character
        {
            if (!in_spaces)      // First space, copy it
                in_spaces = 1;
            else
                continue;   // Not-first space
        }
        *w++ = *s;
    }
    *w = '\0';             // put EOL
    return str;
}

char *remove_spaces( char *str)
{
    char *w = str;
    char *s = str;

    for (; *s; s++)
    {
        if ( !isspace((int)*s))
            *w++ = *s;
    }
    *w = '\0';
    return str;
}

int streq_n(const char *s1, const char *s2, uint len)
{
    while (len--)
        if (*s1++ != *s2++) return 0;
    return 1;
}

int streq(const char *s1, const char *s2)
{
    return strcmp(s1, s2) == 0;
}

// split line by crlf. returns pointer to start.
// also returns pointer to the end of line (aka start of next line). it may point to zero-terminator
char *split_by_newline(char *start, char **end)
{
    char *s = start;
    if (*s == '\0') return NULL;
    char *out = s;
    // rewind to the end of line
    while (*s != '\0' && *s != '\n' && *s != '\r')
        s++;
    // skip trailing CR and/or LF, point to next line
    while (*s == '\n' || *s == '\r')
        s++;
    *end = s;
    return out;
}
