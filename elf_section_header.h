#ifndef ELF_SECTION_HEADER_H
#define ELF_SECTION_HEADER_H

#include "utils/serialize.h"

#include <boost/shared_ptr.hpp>
#include <string>

#include <elf.h>

class elf_object;

class elf_section_header {
private:
  elf_object const &owner;

protected:
  elf_section_header(elf_object const &obj) : owner(obj) {
  }

public:
  template <typename Archiver>
  static boost::shared_ptr<elf_section_header> read(Archiver &AR,
                                                    elf_object const &);

  static void print_header();
  static void print_footer();
  void print() const;

  char const *get_name() const;

  virtual uint32_t get_name_index() const = 0;
  virtual uint32_t get_type() const = 0;
  virtual uint64_t get_flags() const = 0;
  virtual uint64_t get_address() const = 0;
  virtual uint64_t get_offset() const = 0;
  virtual uint64_t get_size() const = 0;
  virtual uint32_t get_link() const = 0;
  virtual uint32_t get_extra_info() const = 0;
  virtual uint64_t get_address_align() const = 0;
  virtual uint64_t get_entry_size() const = 0;

private:
  // Read ELF header
  template <typename Archiver>
  static boost::shared_ptr<elf_section_header> read_32(Archiver &AR,
                                                       elf_object const &);

  template <typename Archiver>
  static boost::shared_ptr<elf_section_header> read_64(Archiver &AR,
                                                       elf_object const &);

  static char const *get_type_name(uint32_t type);

};

extern template boost::shared_ptr<elf_section_header>
elf_section_header::read(serialization::archive_reader_le &,
                         elf_object const &);

extern template boost::shared_ptr<elf_section_header>
elf_section_header::read(serialization::archive_reader_be &,
                         elf_object const &);


#endif // ELF_SECTION_HEADER_H
