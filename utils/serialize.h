#if !defined(SERIALIZE_H)
#define SERIALIZE_H

#include <string.h>
#include <stdint.h>

namespace serialization {

namespace detail {
  inline void swap(unsigned char &a, unsigned char &b) {
    unsigned char tmp = a;
    a = b;
    b = tmp;
  }

  inline bool is_host_little_endian() {
    unsigned long one = 0x1UL;
    return *reinterpret_cast<unsigned char *>(&one);
  }

  inline void mirror(unsigned char (&array)[1]) {
    // Nothing to do
  }

  inline void mirror(unsigned char (&array)[2]) {
    swap(array[0], array[1]);
  }

  inline void mirror(unsigned char (&array)[4]) {
    swap(array[0], array[3]);
    swap(array[1], array[2]);
  }

  inline void mirror(unsigned char (&array)[8]) {
    swap(array[0], array[7]);
    swap(array[1], array[6]);
    swap(array[2], array[5]);
    swap(array[3], array[4]);
  }

  struct set_seek_request {
    off_t off;
    bool from_begin;

    set_seek_request(off_t off_in, bool from_begin_in)
    : off(off_in), from_begin(from_begin_in) {
    }
  };

  struct read_chunk_request {
    unsigned char *ptr;
    size_t size;

    read_chunk_request(unsigned char *ptr_in, size_t size_in)
    : ptr(ptr_in), size(size_in) {
    }
  };
}

template <class reader, bool is_archive_little_endian>
class unserializer {
public:
#define SERIALIZATION_ARCHIVE_READER_OPERATOR_SERIAL(TYPE)                  \
  void operator|=(TYPE &val) {                                              \
    unsigned char (&ref)[sizeof(TYPE)] =                                    \
      reinterpret_cast<unsigned char (&)[sizeof(TYPE)]>(val);               \
                                                                            \
    static_cast<reader *>(this)->read(ref);                                 \
                                                                            \
    if (is_archive_little_endian != detail::is_host_little_endian()) {      \
      detail::mirror(ref);                                                  \
    }                                                                       \
  }

  SERIALIZATION_ARCHIVE_READER_OPERATOR_SERIAL(int8_t);
  SERIALIZATION_ARCHIVE_READER_OPERATOR_SERIAL(int16_t);
  SERIALIZATION_ARCHIVE_READER_OPERATOR_SERIAL(int32_t);
  SERIALIZATION_ARCHIVE_READER_OPERATOR_SERIAL(int64_t);

  SERIALIZATION_ARCHIVE_READER_OPERATOR_SERIAL(uint8_t);
  SERIALIZATION_ARCHIVE_READER_OPERATOR_SERIAL(uint16_t);
  SERIALIZATION_ARCHIVE_READER_OPERATOR_SERIAL(uint32_t);
  SERIALIZATION_ARCHIVE_READER_OPERATOR_SERIAL(uint64_t);

#undef SERIALIZATION_ARCHIVE_READER_OPERATOR_SERIAL
};

template <bool is_archive_little_endian>
class archive_reader
: public unserializer<archive_reader<is_archive_little_endian>,
                      is_archive_little_endian> {
private:
  bool good;

  unsigned char *cursor;
  unsigned char *buf;
  unsigned char *buf_end;

public:
  archive_reader(unsigned char *buf_in = 0, size_t size_in = 0)
  : cursor(buf_in), buf(buf_in), buf_end(buf_in + size_in)
  {
    good = buf_in ? true : false;
  }

  void read(unsigned char *ptr, size_t size) {
    if (good && cursor + size <= buf_end) {
      memcpy(ptr, cursor, size);
      cursor += size;
    } else {
      good = false;
    }
  }

  template <size_t size>
  void read(unsigned char (&ref)[size]) {
    read(ref, size);
  }

  void seek(off_t off, bool from_begin = false) {
    if (from_begin) {
      cursor = buf + off;
    } else {
      cursor += off;
    }
  }

  using unserializer<archive_reader, is_archive_little_endian>::operator|=;

  void operator|=(detail::set_seek_request const &request) {
    seek(request.off, request.from_begin);
  }

  void operator|=(detail::read_chunk_request const &request) {
    read(request.ptr, request.size);
  }

  operator void *() const {
    return good ? this : 0;
  }

  bool operator!() const {
    return !good;
  }
};

inline detail::set_seek_request set_seek(off_t off, bool from_begin = false) {
  return detail::set_seek_request(off, from_begin);
}

inline detail::read_chunk_request read(unsigned char *ptr, size_t size) {
  return detail::read_chunk_request(ptr, size);
}

typedef archive_reader<true>  archive_reader_from_le;
typedef archive_reader<false> archive_reader_from_be;

}

#endif
