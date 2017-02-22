#ifndef _SYNTAX_H_
#define _SYNTAX_H_

#include <stdint.h>
#include <stdbool.h>

// convinience macros
#define countof( arg)       ( sizeof(arg)/sizeof(arg[0]))
#define lastof( arg)        ( countof(arg) - 1)
#define endof( arg)     (&arg[countof(arg)])
#define firstof( arg)   (&arg[0])
#ifndef container_of
    #define container_of(ptr, type, member) ({ const typeof( ((type *)0)->member ) *__mptr = (ptr); (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

// common macro stringifications
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define CAT( a, b)  a##b
#define UNIQUENAME( prefix, line )  CAT( prefix, line)

// static assert
#define PANIC_IF( arg)              extern char UNIQUENAME(AssertAtLine, __LINE__)  [ ( arg) ? -1 : 0 ] __attribute__((unused))

// aliases for common types
typedef unsigned int uint;
typedef signed int sint;

typedef unsigned long ulong;
typedef signed long slong;

typedef unsigned char uchar;
typedef signed char schar;

typedef uint64_t u64;
typedef int64_t s64;

typedef uint32_t u32;
typedef int32_t s32;

typedef uint16_t u16;
typedef int16_t s16;

typedef uint8_t u8;
typedef int8_t s8;

// aliases for gcc builtin functions
#ifndef offsetof
    #define offsetof( obj, field)   __builtin_offsetof( obj, field)
#endif
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#define ctz(arg)          __builtin_ctz(arg)
#define memcpy(d, s, l)   __builtin_memcpy(d, s, l)
#define memmove(d, s, l)  __builtin_memmove(d, s, l)
#define memcmp(a, b, l)   __builtin_memcmp(a, b, l)
#define memset(a, c, l)   __builtin_memset(a, c, l)
#define strcpy(d, s)      __builtin_strcpy(d, s)
#define strcat(d, s)      __builtin_strcat(d, s)
#define memclr(d, size)   __builtin_memset(d, 0, size)

// range clamp, abs
#define CLAMP(min, a, max)  ({ typeof(min) _min = (min); typeof(a) _a = (a); typeof(max) _max = (max); (_a > _max) ? _max : ((_a < min) ? _min : _a);})
#define ABS(x)          (((x) < 0) ? (-(x)) : (x))
#define MIN(a, b)          ({ typeof(a) _a = (a); typeof(b) _b = (b); _a < _b ? _a : _b; })
#define MAX(a, b)          ({ typeof(a) _a = (a); typeof(b) _b = (b); _a > _b ? _a : _b; })

#endif
