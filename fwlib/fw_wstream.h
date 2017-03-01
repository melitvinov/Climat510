#ifndef _FW_WSTREAM_H_
#define _FW_WSTREAM_H_
/** Public */

// this struct may be inherited to provide additional context
typedef struct wstream_t wstream_t;
struct wstream_t
{
    void (*putc)(wstream_t *me, u8 byte);                   // pointer to char putter
    void (*puts)(wstream_t *me, const char *str);           // pointer to string putter
    void (*putb)(wstream_t *me, const u8 *buf, uint len);   // pointer to byte putter
};

void wstream_putu(wstream_t *stream, uint val);
void wstream_putd(wstream_t *stream, int val);
void wstream_putx(wstream_t *stream, u32 val);
void wstream_puts(wstream_t *stream, const char *str);
void wstream_putc(wstream_t *stream, u8 byte);
void wstream_putfloat(wstream_t *stream, float val);
void wstream_putf(wstream_t *stream, double val);
void wstream_putb(wstream_t *stream, const void *str, uint len);
void wstream_putbx(wstream_t *stream, const void *str, uint len);
void wstream_putu_32_base64(wstream_t *stream, u32 val);
void wstream_putu_64_base64(wstream_t *stream, u64 val);
void wstream_put_base64(wstream_t *stream, const void *src, uint len);
void wstream_puts_base64(wstream_t *stream, const char *str);
void wstream_puts_ucs_as_utf8(wstream_t *stream, const u16 *str);

#endif
