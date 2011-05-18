#ifndef ELF_TYPES_H
#define ELF_TYPES_H

#include <iostream>

#include <stddef.h>
#include <stdint.h>

namespace detail {
#define ELF_TYPE_WRAPPER(TYPE, IMPL)                                        \
  struct TYPE {                                                             \
    IMPL value;                                                             \
                                                                            \
    TYPE() : value(0) { }                                                   \
    TYPE(IMPL val) : value(val) { }                                         \
                                                                            \
    TYPE &operator=(TYPE const &with) { value = with.value; return *this; } \
    TYPE &operator=(IMPL val) { value = val; return *this; }                \
                                                                            \
    operator IMPL() const { return value; }                                 \
    operator IMPL &() { return value; }                                     \
  };

  ELF_TYPE_WRAPPER(ELFByte      , uint8_t)
  ELF_TYPE_WRAPPER(ELFHalf      , uint16_t)
  ELF_TYPE_WRAPPER(ELFWord      , uint32_t)
  ELF_TYPE_WRAPPER(ELFSword     , int32_t)
  ELF_TYPE_WRAPPER(ELFXword     , uint64_t)
  ELF_TYPE_WRAPPER(ELFSxword    , int64_t)
  ELF_TYPE_WRAPPER(ELF32Address , uint32_t)
  ELF_TYPE_WRAPPER(ELF32Offset  , uint32_t)
  ELF_TYPE_WRAPPER(ELF64Address , uint64_t)
  ELF_TYPE_WRAPPER(ELF64Offset  , uint64_t)

#undef ELF_TYPE_WRAPPER

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
}


template <typename Type>
struct TypeTraits {
private:
  struct AlignmentTest {
    char pending;
    Type element;
  };

public:
  enum { size = sizeof(Type) };
  enum { align = offsetof(AlignmentTest, element) };
};

// Note: Following TypeTraits specialization MUST be compliant to the
// System V Application Binary Interface, Chap 4, Data Representation.

#define ELF_TYPE_TRAITS_SPECIALIZE(TYPE, SIZE, ALIGN) \
template <> \
struct TypeTraits<detail::TYPE> { \
  enum { size = SIZE }; \
  enum { align = ALIGN }; \
};

ELF_TYPE_TRAITS_SPECIALIZE(ELFByte      , 1, 1)
ELF_TYPE_TRAITS_SPECIALIZE(ELFHalf      , 2, 2)
ELF_TYPE_TRAITS_SPECIALIZE(ELFWord      , 4, 4)
ELF_TYPE_TRAITS_SPECIALIZE(ELFSword     , 4, 4)
ELF_TYPE_TRAITS_SPECIALIZE(ELFXword     , 8, 8)
ELF_TYPE_TRAITS_SPECIALIZE(ELFSxword    , 8, 8)
ELF_TYPE_TRAITS_SPECIALIZE(ELF32Address , 4, 4)
ELF_TYPE_TRAITS_SPECIALIZE(ELF32Offset  , 4, 4)
ELF_TYPE_TRAITS_SPECIALIZE(ELF64Address , 8, 8)
ELF_TYPE_TRAITS_SPECIALIZE(ELF64Offset  , 8, 8)

#undef ELF_TYPE_TRAITS_SPECIALIZE


template <size_t Bitwidth>
struct ELFTypes;

template <>
struct ELFTypes<32> {
  typedef detail::ELF32Address  address;
  typedef detail::ELF32Offset   offset;

  typedef detail::ELFByte       byte;
  typedef detail::ELFHalf       half;
  typedef detail::ELFWord       word;
  typedef detail::ELFSword      sword;

  // Note: Don't use these types.  They are not in the specification of
  // ELF 32.  However, we need these typedefs to define the type introduce
  // macro.
  typedef void xword;
  typedef void sxword;
};

template <>
struct ELFTypes<64> {
  typedef detail::ELF64Address  address;
  typedef detail::ELF64Offset   offset;

  typedef detail::ELFByte       byte;
  typedef detail::ELFHalf       half;
  typedef detail::ELFWord       word;
  typedef detail::ELFSword      sword;
  typedef detail::ELFXword      xword;
  typedef detail::ELFSxword     sxword;
};

#define ELF_TYPE_INTRO_TO_SCOPE(BITWIDTH) \
  typedef ELFTypes<BITWIDTH>::address address_t; \
  typedef ELFTypes<BITWIDTH>::offset  offset_t; \
  typedef ELFTypes<BITWIDTH>::byte    byte_t; \
  typedef ELFTypes<BITWIDTH>::half    half_t; \
  typedef ELFTypes<BITWIDTH>::word    word_t; \
  typedef ELFTypes<BITWIDTH>::sword   sword_t; \
  typedef ELFTypes<BITWIDTH>::xword   xword_t; \
  typedef ELFTypes<BITWIDTH>::sxword  sxword_t;

#define ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(BITWIDTH) \
  typedef typename ELFTypes<BITWIDTH>::address address_t; \
  typedef typename ELFTypes<BITWIDTH>::offset  offset_t; \
  typedef typename ELFTypes<BITWIDTH>::byte    byte_t; \
  typedef typename ELFTypes<BITWIDTH>::half    half_t; \
  typedef typename ELFTypes<BITWIDTH>::word    word_t; \
  typedef typename ELFTypes<BITWIDTH>::sword   sword_t; \
  typedef typename ELFTypes<BITWIDTH>::xword   xword_t; \
  typedef typename ELFTypes<BITWIDTH>::sxword  sxword_t;


#endif // ELF_TYPES_H
