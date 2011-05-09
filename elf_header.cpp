#include "elf_header.h"

#include <elf.h>

class elf_header_32 : public elf_header {
private:
  unsigned char e_ident[EI_NIDENT];
  Elf32_Half e_type;
  Elf32_Half e_machine;
  Elf32_Word e_version;
  Elf32_Addr e_entry;
  Elf32_Off e_phoff;
  Elf32_Off e_shoff;
  Elf32_Word e_flags;
  Elf32_Half e_ehsize;
  Elf32_Half e_phentsize;
  Elf32_Half e_phnum;
  Elf32_Half e_shentsize;
  Elf32_Half e_shnum;
  Elf32_Half e_shstrndx;

public:
  template <typename Archiver>
  void serialize(Archiver &AR);

  virtual uint16_t get_object_type() const;
  virtual uint16_t get_machine() const;
  virtual uint32_t get_version() const;
  virtual uint64_t get_entry_vaddr() const;
  virtual uint64_t get_program_header_table_offset() const;
  virtual uint64_t get_section_header_table_offset() const;
  virtual uint32_t get_flags() const;
  virtual uint16_t get_elf_header_size() const;
  virtual uint16_t get_program_header_entry_size() const;
  virtual uint16_t get_program_header_num() const;
  virtual uint16_t get_section_header_entry_size() const;
  virtual uint16_t get_section_header_num() const;
  virtual uint16_t get_str_section_index() const;
};

class elf_header_64 : public elf_header {
private:
  unsigned char e_ident[EI_NIDENT];
  Elf64_Half e_type;
  Elf64_Half e_machine;
  Elf64_Word e_version;
  Elf64_Addr e_entry;
  Elf64_Off e_phoff;
  Elf64_Off e_shoff;
  Elf64_Word e_flags;
  Elf64_Half e_ehsize;
  Elf64_Half e_phentsize;
  Elf64_Half e_phnum;
  Elf64_Half e_shentsize;
  Elf64_Half e_shnum;
  Elf64_Half e_shstrndx;

public:
  template <typename Archiver>
  void serialize(Archiver &AR);

  virtual uint16_t get_object_type() const;
  virtual uint16_t get_machine() const;
  virtual uint32_t get_version() const;
  virtual uint64_t get_entry_vaddr() const;
  virtual uint64_t get_program_header_table_offset() const;
  virtual uint64_t get_section_header_table_offset() const;
  virtual uint32_t get_flags() const;
  virtual uint16_t get_elf_header_size() const;
  virtual uint16_t get_program_header_entry_size() const;
  virtual uint16_t get_program_header_num() const;
  virtual uint16_t get_section_header_entry_size() const;
  virtual uint16_t get_section_header_num() const;
  virtual uint16_t get_str_section_index() const;
};
