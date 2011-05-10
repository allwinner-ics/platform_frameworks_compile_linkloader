#ifndef ELF_SECTION_HEADER_H
#define ELF_SECTION_HEADER_H

#include "utils/serialize.h"
#include <boost/shared_ptr.hpp>

#include <elf.h>

class elf_section_header {

public:
  template <typename Archiver>
  static boost::shared_ptr<elf_section_header> read(Archiver &AR, bool is_64bit);

  static void print_header();
  static void print_footer();
  void print() const;

  virtual char const* get_name() const = 0;
  virtual uint32_t get_type() const = 0;
  virtual uint64_t get_flags() const = 0;
  virtual uint64_t get_address() const = 0;
  virtual uint64_t get_offset() const = 0;
  virtual uint64_t get_size() const = 0;

private:
  // Read ELF header
  template <typename Archiver>
  static boost::shared_ptr<elf_section_header> read_32(Archiver &AR);

  template <typename Archiver>
  static boost::shared_ptr<elf_section_header> read_64(Archiver &AR);

};

extern template boost::shared_ptr<elf_section_header>
elf_section_header::read(serialization::archive_reader_le &, bool);

extern template boost::shared_ptr<elf_section_header>
elf_section_header::read(serialization::archive_reader_be &, bool);


#endif // ELF_SECTION_HEADER_H
