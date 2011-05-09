#if !defined(SERIALIZE_H)
#define SERIALIZE_H

#include <algorithm>
#include <vector>

#include <assert.h>
#include <string.h>
#include <stdint.h>

namespace serialization {

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


template <bool is_archive_little_endian>
class archive_reader {
private:
  unsigned char const *buf_begin;
  unsigned char const *buf_end;
  unsigned char const *cursor;

  bool good;

private:
  void prologue_bytes(unsigned char *dest, size_t size) {
    if (good) {
      if (cursor + size > buf_end) {
        good = false;
      } else {
        memcpy(dest, cursor, size);
        cursor += size;
      }
    }
  }

  void epilogue_bytes(unsigned char *dest, size_t size) {
    // Nothing to in epilogue in archive reader
  }

public:
  archive_reader(unsigned char const *buf = NULL, size_t size = 0)
  : buf_begin(buf), buf_end(buf + size), cursor(buf), good(buf != NULL) {
  }

  template <typename T> void prologue(T &dest) {
    prologue_bytes(reinterpret_cast<unsigned char *>(&dest), sizeof(T));
  }

  template <typename T> void epilogue(T &dest) {
    epilogue_bytes(reinterpret_cast<unsigned char *>(&dest), sizeof(T));
  }

  void seek(off_t off, bool from_begin = false) {
    if (from_begin) {
      cursor = buf_begin + off;
    } else {
      cursor += off;
    }
  }

#define SERIALIZATION_ARCHIVE_READER_SWAP_BYTE_ORDER_OPERATOR(T)              \
  void operator|=(T &val) {                                                   \
    if (is_archive_little_endian != detail::is_host_little_endian()) {        \
      detail::swap_byte_order(                                                \
        reinterpret_cast<unsigned char (&)[sizeof(T)]>(val));                 \
    }                                                                         \
  }

  SERIALIZATION_ARCHIVE_READER_SWAP_BYTE_ORDER_OPERATOR(int8_t)
  SERIALIZATION_ARCHIVE_READER_SWAP_BYTE_ORDER_OPERATOR(int16_t)
  SERIALIZATION_ARCHIVE_READER_SWAP_BYTE_ORDER_OPERATOR(int32_t)
  SERIALIZATION_ARCHIVE_READER_SWAP_BYTE_ORDER_OPERATOR(int64_t)

  SERIALIZATION_ARCHIVE_READER_SWAP_BYTE_ORDER_OPERATOR(uint8_t)
  SERIALIZATION_ARCHIVE_READER_SWAP_BYTE_ORDER_OPERATOR(uint16_t)
  SERIALIZATION_ARCHIVE_READER_SWAP_BYTE_ORDER_OPERATOR(uint32_t)
  SERIALIZATION_ARCHIVE_READER_SWAP_BYTE_ORDER_OPERATOR(uint64_t)

  SERIALIZATION_ARCHIVE_READER_SWAP_BYTE_ORDER_OPERATOR(float)
  SERIALIZATION_ARCHIVE_READER_SWAP_BYTE_ORDER_OPERATOR(double)

#undef SERIALIZATION_ARCHIVE_READER_SWAP_BYTE_ORDER_OPERATOR

  operator void *() const {
    return good ? this : 0;
  }

  bool operator!() const {
    return !good;
  }
};

typedef archive_reader<true>  archive_reader_from_little_endian;
typedef archive_reader<false> archive_reader_from_big_endian;


template <bool is_archive_little_endian>
class archive_writer {
private:
  std::vector<unsigned char> buf;
  unsigned char const *prev_src;
  size_t cursor;

private:
  void prologue_bytes(unsigned char const *src, size_t size) {
    assert(prev_src == NULL); // Note: prologue/epilogue is not reentrant.
    prev_src = src;

    if (cursor + size > buf.size()) {
      buf.resize(cursor + size);
    }

    memcpy(&*buf.begin() + cursor, src, size);
  }

  void epilogue_bytes(unsigned char const *src, size_t size) {
    prev_src = NULL;
    cursor += size;
  }

public:
  archive_writer() : cursor(0), prev_src(NULL) {
  }

  unsigned char const *begin() const {
    return &*buf.begin();
  }

  unsigned char const *end() const {
    return &*buf.end();
  }

  size_t size() const {
    return buf.size();
  }

  template <typename T> void prologue(T const &src) {
    prologue_bytes(reinterpret_cast<unsigned char const *>(&src), sizeof(T));
  }

  template <typename T> void epilogue(T const &src) {
    epilogue_bytes(reinterpret_cast<unsigned char const *>(&src), sizeof(T));
  }

  void seek(off_t off, bool from_begin = false) {
    if (from_begin) {
      cursor = off;
    } else {
      cursor += off;
    }
  }

#define SERIALIZATION_ARCHIVE_WRITER_SWAP_BYTE_ORDER_OPERATOR(T)              \
  void operator|=(T const &val) {                                             \
    if (is_archive_little_endian != detail::is_host_little_endian()) {        \
      ptrdiff_t off =                                                         \
        reinterpret_cast<unsigned char const *>(&val) - prev_src;             \
                                                                              \
      detail::swap_byte_order(reinterpret_cast<unsigned char (&)[sizeof(T)]>( \
          *(&*buf.begin() + cursor + off)));                                  \
    }                                                                         \
  }

  SERIALIZATION_ARCHIVE_WRITER_SWAP_BYTE_ORDER_OPERATOR(int8_t)
  SERIALIZATION_ARCHIVE_WRITER_SWAP_BYTE_ORDER_OPERATOR(int16_t)
  SERIALIZATION_ARCHIVE_WRITER_SWAP_BYTE_ORDER_OPERATOR(int32_t)
  SERIALIZATION_ARCHIVE_WRITER_SWAP_BYTE_ORDER_OPERATOR(int64_t)

  SERIALIZATION_ARCHIVE_WRITER_SWAP_BYTE_ORDER_OPERATOR(uint8_t)
  SERIALIZATION_ARCHIVE_WRITER_SWAP_BYTE_ORDER_OPERATOR(uint16_t)
  SERIALIZATION_ARCHIVE_WRITER_SWAP_BYTE_ORDER_OPERATOR(uint32_t)
  SERIALIZATION_ARCHIVE_WRITER_SWAP_BYTE_ORDER_OPERATOR(uint64_t)

  SERIALIZATION_ARCHIVE_WRITER_SWAP_BYTE_ORDER_OPERATOR(float)
  SERIALIZATION_ARCHIVE_WRITER_SWAP_BYTE_ORDER_OPERATOR(double)

#undef SERIALIZATION_ARCHIVE_WRITER_SWAP_BYTE_ORDER_OPERATOR


  // Note: Archive writer is always considered in "good" state, unless the
  // buffer is not big enough, and an exception will be thrown.

  operator void *() const {
    return this;
  }

  bool operator!() const {
    return false;
  }
};

typedef archive_writer<true>  archive_writer_to_little_endian;
typedef archive_writer<false> archive_writer_to_big_endian;

}

#endif
