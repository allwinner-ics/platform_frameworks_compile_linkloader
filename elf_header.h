#ifndef ELF_HEADER_H
#define ELF_HEADER_H

#include "utils/serialize.h"
#include <boost/shared_ptr.hpp>

class elf_ident;

class elf_header {
public:
  virtual boost::shared_ptr<elf_ident> get_ident() const = 0;

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

  template <typename Archiver>
  static boost::shared_ptr<elf_header> read(Archiver &AR);

  bool is_valid_elf_header() const;

  void print() const;

private:
  char const *get_object_type_name();
  char const *get_machine_name();
  char const *get_version_name();

};

extern template boost::shared_ptr<elf_header>
elf_header::read(serialization::archive_reader_le &);

extern template boost::shared_ptr<elf_header>
elf_header::read(serialization::archive_reader_be &);

#endif // ELF_HEADER_H
