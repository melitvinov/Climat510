#ifndef _DEBUG_H_
#define _DEBUG_H_
/** Public */

#include "fw_format.h"

// exposed stream
extern wstream_t dbg_stream;

void assertion( const char *desc) __noreturn __noinline;
void print_dump(const void *data, uint len);

/*** Evil hackery :-) */

#ifdef NOASSERT
    #define REQUIRE( expr) do {;} while(0)
#else
#ifndef _PC
    #define REQUIRE( expr)                                                                      \
        do                                                                                      \
            if (unlikely(!(expr)))                                                              \
                assertion(__FILE__":"QUOTE(__LINE__)":"#expr);                                  \
        while ( 0)
#else
    #include <assert.h>
    #define REQUIRE(expr) assert(expr)
#endif
#endif

#ifdef NOLOG
    #define LOG( Fmt, ...)  do {;} while(0)
#else
    #define LOG( Fmt, ...)  printf( ANSI_WHITE __FILE__ ": "ANSI_NORMAL Fmt "\n",  ## __VA_ARGS__)
#endif

#ifdef NOWARN
    #define WARN( Fmt, ...)  do {;} while(0)
#else
    #define WARN( Fmt, ...)  printf( ANSI_RED __FILE__ ":" QUOTE(__LINE__)" Warning: "ANSI_NORMAL Fmt "\n",  ## __VA_ARGS__)
#endif

#ifdef NOERR
    #define ERR( Fmt, ...)  do {;} while(0)
#else
    #define ERR( Fmt, ...)  printf( ANSI_RED __FILE__  ":" QUOTE(__LINE__)" Error: "ANSI_NORMAL Fmt "\n",  ## __VA_ARGS__)
#endif

#define ANSI_BLACK      "\x1B[30m"
#define ANSI_RED        "\x1B[31m"
#define ANSI_GREEN      "\x1B[32m"
#define ANSI_YELLOW     "\x1B[33m"
#define ANSI_BLUE       "\x1B[34m"
#define ANSI_MAGENTA    "\x1B[35m"
#define ANSI_CYAN       "\x1B[36m"
#define ANSI_WHITE      "\x1B[37m"
#define ANSI_NORMAL     "\x1B[0m"
#define ANSI_BLUE_BACK  "\x1B[44m"

#endif
