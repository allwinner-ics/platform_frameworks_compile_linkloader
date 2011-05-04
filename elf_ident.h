#ifndef ELF_IDENT_H
#define ELF_IDENT_H

#include <boost/shared_ptr.hpp>

#include <elf.h>
#include <stdlib.h>

class elf_ident {
private:
  unsigned char ident[EI_NIDENT];

public:
  static boost::shared_ptr<elf_ident> create(unsigned char const *file,
                                             size_t size,
                                             size_t offset = 0);

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

private:
  elf_ident(unsigned char const *buf);

  void assert_valid_magic_word() const;
  void assert_valid_class() const;
  void assert_valid_endianness() const;
  void assert_valid_header_version() const;
  void assert_zeroed_padding() const;

  static char const *get_class_name(int clazz);
  static char const *get_endianness_name(int endianness);
  static char const *get_os_abi_name(int abi);
};

#endif // ELF_IDENT_H
