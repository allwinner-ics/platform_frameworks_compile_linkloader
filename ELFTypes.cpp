#include "ELFTypes.h"
#include <iostream>

namespace detail {

#define ELF_TYPE_PRINT_OPERATOR(TYPE, FORMAT_WIDTH)                         \
  std::ostream &operator<<(std::ostream &os, TYPE const &val) {             \
    char prev_fill = os.fill('0');                                          \
    std::ios_base::fmtflags prev_flags = os.setf(std::ios_base::hex,        \
                                                 std::ios_base::basefield); \
    os.width(FORMAT_WIDTH);                                                 \
    os << val.value;                                                        \
    os.setf(prev_flags);                                                    \
    os.fill(prev_fill);                                                     \
    return os;                                                              \
  }

  ELF_TYPE_PRINT_OPERATOR(ELF32Address, 8)
  ELF_TYPE_PRINT_OPERATOR(ELF32Offset,  8)
  ELF_TYPE_PRINT_OPERATOR(ELF64Address, 16)
  ELF_TYPE_PRINT_OPERATOR(ELF64Offset,  16)

#undef ELF_TYPE_PRINT_OPERATOR

} // end namespace detail
