#include "endianness.h"

#include <algorithm>
#include <stdint.h>

namespace {

inline bool is_host_big_endian() {
  unsigned long i = 0x0001UL;
  return (!reinterpret_cast<char *>(&i)[0]);
}

inline void swap(unsigned char (&c)[2]) {
  std::swap(c[0], c[1]);
}

inline void swap(unsigned char (&c)[4]) {
  std::swap(c[0], c[3]);
  std::swap(c[1], c[2]);
}

inline void swap(unsigned char (&c)[8]) {
  std::swap(c[0], c[7]);
  std::swap(c[1], c[6]);
  std::swap(c[2], c[5]);
  std::swap(c[3], c[4]);
}

} // end anonymous namespace

#define IMPL_ONE_CONV(BE, TYPE) \
void endianness_conv<BE>::operator|=(TYPE &val) { \
  if (is_host_big_endian() != (BE)) { \
    swap(reinterpret_cast<unsigned char (&)[sizeof(TYPE)]>(val)); \
  } \
}

#define IMPL_ENDIAN_CONV(BE) \
IMPL_ONE_CONV(BE, int16_t) \
IMPL_ONE_CONV(BE, int32_t) \
IMPL_ONE_CONV(BE, int64_t) \
IMPL_ONE_CONV(BE, uint16_t) \
IMPL_ONE_CONV(BE, uint32_t) \
IMPL_ONE_CONV(BE, uint64_t)

IMPL_ENDIAN_CONV(false) // Little Endian
IMPL_ENDIAN_CONV(true)  // Big Endian
