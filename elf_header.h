#ifndef ELF_HEADER_H
#define ELF_HEADER_H

#include <boost/shared_ptr.hpp>

class elf_ident;

class elf_header {
private:
  boost::shared_ptr<elf_ident> ident;

priavate:
  elf_header(boost::shared_ptr<elf_ident> idt,
             unsigned char const *buf);

public:
  boost::shared_ptr<elf_header> create(unsigned char const *file,
                                       size_t size,
                                       size_t offset);

  boost::shared_ptr<elf_ident> get_ident() const;

  uint16_t get_object_type() const;
  uint16_t get_machine() const;
  uint32_t get_version() const;
  uint64_t get_entry_vaddr() const;
  uint64_t get_program_header_table_offset() const;
  uint64_t get_section_header_table_offset() const;
  uint32_t get_flags() const;
  uint16_t get_elf_header_size() const;
  uint16_t get_program_header_entry_size() const;
  uint16_t get_program_header_num() const;
  uint16_t get_section_header_entry_size() const;
  uint16_t get_section_header_num() const;
  uint16_t get_str_section_index() const;
};

#endif // ELF_HEADER_H
