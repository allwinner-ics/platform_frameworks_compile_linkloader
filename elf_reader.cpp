#include "elf_reader.h"

#include "elf_ident.h"

#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/shared_ptr.hpp>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace boost;
using namespace std;

elf_reader::elf_reader(string const &filename) {
  file_fd = open(filename.c_str(), O_RDONLY);
  if (file_fd < 0) {
    throw runtime_error("Unable to open the file");
  }

  struct stat sb;
  if (fstat(file_fd, &sb) != 0) {
    close(file_fd);
    throw runtime_error("Unable to stat the file");
  }

  file_size = sb.st_size;

  file = static_cast<unsigned char *>(
    mmap(0, file_size, PROT_READ, MAP_PRIVATE, file_fd, 0));

  if (file == NULL || file == MAP_FAILED) {
    close(file_fd);
    throw runtime_error("Unable to mmap the file");
  }

  shared_ptr<elf_ident> idt = elf_ident::create(file, file_size, 0);
  idt->print();
}

elf_reader::~elf_reader() {
  if (file != NULL && file != MAP_FAILED) {
    munmap(static_cast<void *>(const_cast<unsigned char *>(file)), file_size);
  }

  if (file_fd >= 0) {
    close(file_fd);
  }
}
