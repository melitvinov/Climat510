#ifndef _FW_FORMAT_H_
#define _FW_FORMAT_H_
/** Public */
#include "fw_wstream.h"

void fw_printf(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
int fw_sprintf(char *dst, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
void fw_stream_format(wstream_t *stream, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

#ifndef _PC
#define printf( Fmt, ...)           fw_printf( (Fmt),  ## __VA_ARGS__)
#define sprintf( Targ, Fmt, ...)    fw_sprintf( (Targ), (Fmt), ## __VA_ARGS__)
#endif

#endif
