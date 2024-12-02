#ifndef DEBUG_H
#define DEBUG_H

#ifdef NDEBUG

// disable functions
#define DLOG(...) ((void)0)

#define INIT_LOG

#else

#include <stdio.h>

#define DLOG(fmt, ...)                                                         \
        fprintf(stderr, "DEBUG: ");                                            \
        fprintf(stderr, fmt, ##__VA_ARGS__);                                   \
        fprintf(stderr, "\n");
#endif // NDEBUG

#endif
