#ifndef ELF_READER_H
#define ELF_READER_H

#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

#include <stdio.h>

class elf_ident;
class elf_header;
class elf_section_header;
class elf_program_header;
class elf_section;

class elf_object {
private:
  boost::shared_ptr<elf_header> header;
  std::vector<boost::shared_ptr<elf_section_header> > sh_table;
  std::vector<boost::shared_ptr<elf_section> > s_table;
  std::vector<boost::shared_ptr<elf_program_header> > ph_table;

  elf_section_header const &get_section_header(size_t index) const { return *sh_table[index]; }
  elf_section const &get_section(size_t index) const { return *s_table[index]; }

  elf_object() {
  }

public:
  static boost::shared_ptr<elf_object> read(std::string const &filename);

  elf_header const &get_header() const { return *header; }

  void print() const;

private:
  elf_object(std::string const &filename);
};

#endif // ELF_READER_H
