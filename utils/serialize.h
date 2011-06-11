#if !defined(SERIALIZE_H)
#define SERIALIZE_H

#include "traits.h"

#include <algorithm>
#include <vector>

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

namespace detail {
  inline bool is_host_little_endian() {
    unsigned long one = 0x1UL;
    return *reinterpret_cast<unsigned char *>(&one);
  }

  inline void swap_byte_order(unsigned char (&array)[1]) {
    // Nothing to do
  }

  inline void swap_byte_order(unsigned char (&array)[2]) {
    std::swap(array[0], array[1]);
  }

  inline void swap_byte_order(unsigned char (&array)[4]) {
    std::swap(array[0], array[3]);
    std::swap(array[1], array[2]);
  }

  inline void swap_byte_order(unsigned char (&array)[8]) {
    std::swap(array[0], array[7]);
    std::swap(array[1], array[6]);
    std::swap(array[2], array[5]);
    std::swap(array[3], array[4]);
  }
}


template <bool isArchiveLittleEndian>
class ArchiveReader {
private:
  unsigned char const *buf_begin;
  unsigned char const *buf_end;
  unsigned char const *cursor;
  unsigned char const *cursor_base;

  bool good;

public:
  ArchiveReader(unsigned char const *buf = NULL, size_t size = 0)
  : buf_begin(buf), buf_end(buf + size),
    cursor(buf), cursor_base(NULL), good(buf != NULL) {
  }

  void prologue(size_t size) {
    assert(cursor_base == NULL);
    cursor_base = cursor;
  }

  void epilogue(size_t size) {
    assert(cursor_base != NULL);
    assert(cursor_base + size >= cursor);
    cursor = cursor_base + size;
    cursor_base = NULL;
  }

  void seek(off_t off, bool from_begin = false) {
    if (from_begin) {
      cursor = buf_begin + off;
    } else {
      cursor += off;
    }
  }

  void readBytes(void *array, size_t size) {
    if (!good || cursor + size > buf_end) {
      good = false;
    } else {
      memcpy(array, cursor, size);
    }
  }

  template <size_t size>
  void operator&(char (&array)[size]) {
    readBytes(array, size);
    seek(size);
  }

  template <size_t size>
  void operator&(unsigned char (&array)[size]) {
    readBytes(array, size);
    seek(size);
  }

#define SERIALIZE_ARCHIVE_READER_READ_AND_SWAP_BYTE_ORDER(T)                  \
  void operator&(T &v) {                                                      \
    using namespace detail;                                                   \
                                                                              \
    seekToNextAddress<T>();                                                   \
    readBytes(&v, sizeof(T));                                                 \
    seek(sizeof(T));                                                          \
                                                                              \
    if (isArchiveLittleEndian != is_host_little_endian()) {                   \
      swap_byte_order(reinterpret_cast<unsigned char (&)[sizeof(T)]>(v));     \
    }                                                                         \
  }

  SERIALIZE_ARCHIVE_READER_READ_AND_SWAP_BYTE_ORDER(bool)
  SERIALIZE_ARCHIVE_READER_READ_AND_SWAP_BYTE_ORDER(int8_t)
  SERIALIZE_ARCHIVE_READER_READ_AND_SWAP_BYTE_ORDER(int16_t)
  SERIALIZE_ARCHIVE_READER_READ_AND_SWAP_BYTE_ORDER(int32_t)
  SERIALIZE_ARCHIVE_READER_READ_AND_SWAP_BYTE_ORDER(int64_t)
  SERIALIZE_ARCHIVE_READER_READ_AND_SWAP_BYTE_ORDER(uint8_t)
  SERIALIZE_ARCHIVE_READER_READ_AND_SWAP_BYTE_ORDER(uint16_t)
  SERIALIZE_ARCHIVE_READER_READ_AND_SWAP_BYTE_ORDER(uint32_t)
  SERIALIZE_ARCHIVE_READER_READ_AND_SWAP_BYTE_ORDER(uint64_t)
  SERIALIZE_ARCHIVE_READER_READ_AND_SWAP_BYTE_ORDER(float)
  SERIALIZE_ARCHIVE_READER_READ_AND_SWAP_BYTE_ORDER(double)
  SERIALIZE_ARCHIVE_READER_READ_AND_SWAP_BYTE_ORDER(void *)
  SERIALIZE_ARCHIVE_READER_READ_AND_SWAP_BYTE_ORDER(void const *)

#undef SERIALIZE_ARCHIVE_READER_READ_AND_SWAP_BYTE_ORDER

  operator void const *() const {
    return good ? this : 0;
  }

  bool operator!() const {
    return !good;
  }

private:
  template <typename T>
  void seekToNextAddress() {
    size_t align = TypeTraits<T>::align;
    size_t delta = static_cast<size_t>(cursor - buf_begin) % align;

    if (delta > 0) {
      seek(align - delta);
    }
  }

};

typedef ArchiveReader<true>  ArchiveReaderLE;
typedef ArchiveReader<false> ArchiveReaderBE;

#endif // SERIALIZE_H
