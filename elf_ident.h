#ifndef ELF_IDENT_H
#define ELF_IDENT_H

#include "utils/serialize.h"

#include <boost/shared_ptr.hpp>

#include <elf.h>
#include <stdlib.h>

class elf_ident {
private:
  unsigned char ident[EI_NIDENT];

public:
  elf_ident();

  bool is_valid_elf_ident() const;

  int get_class() const;
  int get_endianness() const;
  int get_header_version() const;
  int get_os_abi() const;
  int get_abi_version() const;

  bool is_32bit() const;
  bool is_64bit() const;

  bool is_big_endian() const;
  bool is_little_endian() const;

  void print() const;

  template <typename Archiver>
  void serialize(Archiver &AR);

  template <typename Archiver>
  static boost::shared_ptr<elf_ident> read(Archiver &AR);

private:
  inline bool is_valid_magic_word() const;
  inline bool is_valid_class() const;
  inline bool is_valid_endianness() const;
  inline bool is_valid_header_version() const;
  inline bool is_unused_zeroed_padding() const;

  static char const *get_class_name(int clazz);
  static char const *get_endianness_name(int endianness);
  static char const *get_os_abi_name(int abi);
};

template <typename Archiver>
inline void elf_ident::serialize(Archiver &AR) {
  AR <<= *this;
  AR & ident;
  AR >>= *this;
}

extern template boost::shared_ptr<elf_ident>
elf_ident::read<serialization::archive_reader_le>(
  serialization::archive_reader_le &);

extern template boost::shared_ptr<elf_ident>
elf_ident::read<serialization::archive_reader_be>(
  serialization::archive_reader_be &);

#endif // ELF_IDENT_H
