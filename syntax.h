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

#endif
