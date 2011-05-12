#include "elf_object.h"

#include "utils/serialize.h"
#include "elf_header.h"
#include "elf_section_header.h"

#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/shared_ptr.hpp>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace boost;
using namespace serialization;
using namespace std;

elf_object::elf_object(string const &filename) {
  //TODO:constructor
}
shared_ptr<elf_object> elf_object::read(string const &filename) {
  int file_fd = open(filename.c_str(), O_RDONLY);
  if (file_fd < 0) {
    return shared_ptr<elf_object>();
  }

  struct stat sb;
  if (fstat(file_fd, &sb) != 0) {
    close(file_fd);
    return shared_ptr<elf_object>();
  }

  size_t file_size = (size_t)sb.st_size;

  unsigned char const *file = static_cast<unsigned char const *>(
    mmap(0, file_size, PROT_READ, MAP_PRIVATE, file_fd, 0));

  if (file == NULL || file == MAP_FAILED) {
    close(file_fd);
    return shared_ptr<elf_object>();
  }

  shared_ptr<elf_object> elf_obj_ptr(new elf_object(filename));

  archive_reader_le ar(file, file_size);
  elf_obj_ptr->header = elf_header::read(ar);
  const elf_header &idt = elf_obj_ptr->get_header();

  ar.seek(idt.get_section_header_table_offset(), true);
  for(int i=0; i<idt.get_section_header_num(); ++i) {
    elf_obj_ptr->sh_table.push_back(
      elf_section_header::read(ar, idt.is_64bit()));
  }

  if (file != NULL && file != MAP_FAILED) {
    munmap(static_cast<void *>(const_cast<unsigned char *>(file)), file_size);
  }

  if (file_fd >= 0) {
    close(file_fd);
  }

  return elf_obj_ptr;
}

void elf_object::print() const{
  // Print elf header
  get_header().print();

  // Print elf section header
  elf_section_header::print_header();
  for(int i=0; i<get_header().get_section_header_num(); ++i) {
    get_section_header(i).print();
  }
  elf_section_header::print_footer();
}
