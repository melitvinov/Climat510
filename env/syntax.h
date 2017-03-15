#ifndef _SYNTAX_H_
#define _SYNTAX_H_

#include <stdint.h>
#include <stdbool.h>

#ifndef NULL
    #define NULL	((void*) 0)
#endif

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
#define strlen(s)         __builtin_strlen(s)
#define memclr(d, size)   __builtin_memset(d, 0, size)

// range clamp, abs
#define CLAMP(min, a, max)  ({ typeof(min) _min = (min); typeof(a) _a = (a); typeof(max) _max = (max); (_a > _max) ? _max : ((_a < min) ? _min : _a);})
#define ABS(x)          (((x) < 0) ? (-(x)) : (x))
#define MIN(a, b)          ({ typeof(a) _a = (a); typeof(b) _b = (b); _a < _b ? _a : _b; })
#define MAX(a, b)          ({ typeof(a) _a = (a); typeof(b) _b = (b); _a > _b ? _a : _b; })

// aliases for GCC attributes
#define __packed            __attribute__ ((packed))
#define __long_call         __attribute__ ((long_call))
#define __always_inline     __attribute__ ((always_inline))
#define __pure              __attribute__ ((pure))
#define __const_func        __attribute__ ((const))
#define __noinline          __attribute__ ((__noinline__))
#define __noreturn          __attribute__ ((__noreturn__))
#define __aligned(bound)    __attribute__ ((aligned(bound)))
#define __cleanup(dtor)     __attribute__((__cleanup__(dtor)))
#define __naked             __attribute__ ((naked))
#define __flatten           __attribute__ ((flatten))
#define __hot               __attribute__ ((hot))
#define __unused            __attribute__ ((unused))
#define __used              __attribute__ ((used))
#define __weak              __attribute__ ((weak))
#define __optimize(level)   __attribute__ ((optimize(level)))

#endif
