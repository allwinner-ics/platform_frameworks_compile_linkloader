#include "helper.h"

#include <iostream>
#include <iomanip>

#include <stdlib.h>

#include "term.h"

using namespace std;
using namespace term::color;

void dump_hex(unsigned char const *data,
              size_t size, size_t begin, size_t end) {
  if (end <= begin) {
    // Nothing to print now.  Return directly.
    return;
  }

  size_t lower = begin & (~0xfUL);
  size_t upper = (end & (~0xfUL)) ? end : ((end + 16UL) & (~0xfUL));

  for (size_t i = lower; i < upper; i += 16) {
    cout << hex << setfill('0') << setw(8) << right << i << left << ':';

    if (i < begin) {
      cout << dark::magenta();
    }

    for (size_t j = i, k = i + 16; j < k; ++j) {
      if (j == begin) {
        cout << normal();
      }

      if (j == end) {
        cout << dark::magenta();
      }

      if (j < size) {
        cout << ' ' << hex << setfill('0') << setw(2) << (unsigned)data[j];
      }
    }

    cout << normal() << endl;
  }
}
