#ifndef FLUSH_CPU_CACHE_H
#define FLUSH_CPU_CACHE_H

#ifdef __arm__

// Note: Though we wish to use the gcc builtin function __clear_cache to
// invalidate the instruction cache; however, the toolchain of Android
// has not supported it properly.  We are going to use cacheflush system
// call to invalidate the instruction cache.
//
// As a side note, Dalvik VM use the same system call to invalidate the
// instruction as well.

#include <unistd.h>

#define FLUSH_CPU_CACHE(BEGIN, END) \
  cacheflush(((long)(BEGIN)), ((long)(END)), 0)

#else

#define FLUSH_CPU_CACHE(BEGIN, END) do { } while (0)

#endif

#endif // FLUSH_CPU_CACHE_H
