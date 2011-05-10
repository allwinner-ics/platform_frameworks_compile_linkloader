#ifndef ELF_HEADER_H
#define ELF_HEADER_H

#include "utils/serialize.h"
#include <boost/shared_ptr.hpp>

#include <elf.h>

class elf_header {
protected:
  unsigned char e_ident[EI_NIDENT];

public:
  template <typename Archiver>
  static boost::shared_ptr<elf_header> read(Archiver &AR);

  void print() const;

  // ELF identification related functions
  bool is_32bit() const;
  bool is_64bit() const;

  bool is_big_endian() const;
  bool is_little_endian() const;

  int get_class() const;
  int get_endianness() const;
  int get_header_version() const;
  int get_os_abi() const;
  int get_abi_version() const;

  // ELF header related functions
  bool is_valid_elf_header() const;

  virtual uint16_t get_object_type() const = 0;
  virtual uint16_t get_machine() const = 0;
  virtual uint32_t get_version() const = 0;
  virtual uint64_t get_entry_vaddr() const = 0;
  virtual uint64_t get_program_header_table_offset() const = 0;
  virtual uint64_t get_section_header_table_offset() const = 0;
  virtual uint32_t get_flags() const = 0;
  virtual uint16_t get_elf_header_size() const = 0;
  virtual uint16_t get_program_header_entry_size() const = 0;
  virtual uint16_t get_program_header_num() const = 0;
  virtual uint16_t get_section_header_entry_size() const = 0;
  virtual uint16_t get_section_header_num() const = 0;
  virtual uint16_t get_str_section_index() const = 0;

private:
  // Read ELF header
  template <typename Archiver>
  static boost::shared_ptr<elf_header> read_32(Archiver &AR);

  template <typename Archiver>
  static boost::shared_ptr<elf_header> read_64(Archiver &AR);

  // ELF identification related private functions
  inline bool is_valid_elf_ident() const;

  inline bool is_valid_magic_word() const;
  inline bool is_valid_class() const;
  inline bool is_valid_endianness() const;
  inline bool is_valid_header_version() const;
  inline bool is_unused_zeroed_padding() const;

  static char const *get_class_name(int clazz);
  static char const *get_endianness_name(int endianness);
  static char const *get_os_abi_name(int abi);

  // ELF header related privat functions
  inline bool is_compatible_header_size() const;

  static char const *get_object_type_name(uint16_t type);
  static char const *get_machine_name(uint16_t machine);
  static char const *get_version_name(uint32_t version);

};

extern template boost::shared_ptr<elf_header>
elf_header::read(serialization::archive_reader_le &);

extern template boost::shared_ptr<elf_header>
elf_header::read(serialization::archive_reader_be &);


#endif // ELF_HEADER_H
