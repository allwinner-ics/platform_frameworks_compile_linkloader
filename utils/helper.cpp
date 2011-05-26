#include "helper.h"
#include "raw_ostream.h"

#include <stdlib.h>

using namespace llvm;

void dump_hex(unsigned char const *data,
              size_t size, size_t begin, size_t end) {
  if (end <= begin) {
    // Nothing to print now.  Return directly.
    return;
  }

  size_t lower = begin & (~0xfUL);
  size_t upper = (end & (~0xfUL)) ? end : ((end + 16UL) & (~0xfUL));

  for (size_t i = lower; i < upper; i += 16) {
    out() << format("%08x", i) << ':';

    if (i < begin) {
      out().changeColor(raw_ostream::MAGENTA);
    }

    for (size_t j = i, k = i + 16; j < k; ++j) {
      if (j == begin) {
        out().resetColor();
      }

      if (j == end) {
        out().changeColor(raw_ostream::MAGENTA);
      }

      if (j < size) {
        out() << ' ' << format("%02x", (unsigned)data[j]);
      }
    }

    out().resetColor();
    out() << '\n';
  }
}
