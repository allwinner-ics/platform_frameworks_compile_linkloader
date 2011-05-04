#ifndef ELF_READER_H
#define ELF_READER_H

#include <boost/shared_ptr.hpp>

#include <string>

#include <stdio.h>

class elf_ident;
class elf_header;
class elf_section_header_table;
class elf_program_header_table;

class elf_reader {
private:
  int file_fd;
  unsigned char *file;
  off_t file_size;

  boost::shared_ptr<elf_ident> ident;
  boost::shared_ptr<elf_header> header;
  boost::shared_ptr<elf_section_header_table> sh_table;
  boost::shared_ptr<elf_program_header_table> ph_table;

public:
  elf_reader(std::string const &filename);
  ~elf_reader();

private:
  elf_reader(elf_reader const &); // DON'T IMPLEMENT
  elf_reader &operator=(elf_reader const &); // DON'T IMPLEMENT
};

#endif // ELF_READER_H
