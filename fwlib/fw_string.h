#ifndef _FW_STRING_H_
#define _FW_STRING_H_
/** Public */

#define streq_n_const(s1, s2)   streq_n(s1, s2, sizeof(s2) - 1)

char *strfcpy( char *dst, const char *src, uint len);
void strsafecpy(char *dst, const char *src, uint size);
char *rstrip_spaces( char *str);
char *lowcase( char *str);
char *upcase(char *str);
const char *skip_lspaces(const char *str);
char *strip_spaces(char *str);
char *lstrip_spaces(char *str);
char *compress_spaces(char *str);
char *remove_spaces(char *str);
int streq(const char *s1, const char *s2);

int streq_n(const char *s1, const char *s2, uint len);
char *split_by_newline(char *start, char **end);

#endif
