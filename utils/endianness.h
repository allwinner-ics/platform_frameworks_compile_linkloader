#ifndef ENDIANNESS_H
#define ENDIANNESS_H

#include <stdint.h>

template <bool is_big_endian> class endianness_conv;


// Little Endian

template <> class endianness_conv<false> {
public:
  void operator|=(int16_t &v);
  void operator|=(int32_t &v);
  void operator|=(int64_t &v);

  void operator|=(uint16_t &val);
  void operator|=(uint32_t &v);
  void operator|=(uint64_t &v);
};

extern template class endianness_conv<false>;


// Big Endian

template <> class endianness_conv<true> {
public:
  void operator|=(int16_t &);
  void operator|=(int32_t &);
  void operator|=(int64_t &);

  void operator|=(uint16_t &);
  void operator|=(uint32_t &);
  void operator|=(uint64_t &);
};

extern template class endianness_conv<true>;


#endif // ENDIANNESS_H
