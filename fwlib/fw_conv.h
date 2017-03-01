#ifndef _FW_CONV_H_
#define _FW_CONV_H_
/** Public */

int fw_uitoa(char *str, u32 val);
int fw_uxtoa(char *str, u32 val);

uint fw_atoi(const char *str, s32 *val);
uint fw_atou(const char *str, u32 *val);

long fw_strtol10(const char *str, char **tail);

uint hexstr_to_uint(const char *str, u32 *val);
uint decstr_to_uint(const char *str, uint *val);
uint decstr_to_int(const char *str, int *val);
const char *short_to_decstr(char *str, s32 val);
const char *char_to_decstr(char *str, s32 val);
const char *char_to_decstr_nozeroes(char *str, s32 val);
const char *char_to_decstr_nozeroes_nopad(char *str, s32 val);
const char *short_to_str(s32 val);
u32 short_to_bcd(s32 val);

int base64_decode(void *dst, const char *src, uint dst_size);
uint base64_to_u32(const char *str, u32 *val);
uint base64_to_u64(const char *str, u64 *val);

char *jsonstr_to_utf8(char *out, const char *in);

int utf8_chr(char *out, uint16_t input, int out_size);
void utf16_str_to_utf8(char *out, const u16 *in, uint out_size);
void utf8_str_to_utf16(u16 *out, const char *in, uint out_size);

#endif
