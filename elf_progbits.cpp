#include "elf_progbits.h"
#include "elf_section_header.h"
#include "utils/serialize.h"

#include "elf.h"
#include <sys/mman.h>
#include <iostream>
#include <cstdio>

using namespace boost;
using namespace serialization;
using namespace std;


template <typename Archiver>
shared_ptr<elf_progbits> elf_progbits::read(Archiver &AR,
                                        elf_section_header const &sh) {
  // Check section type
  if (!(sh.get_type() & SHT_PROGBITS)) {
    // This is not a PROGBITS section
    return shared_ptr<elf_progbits>();
  }

  // Reserve enough buffer
  shared_ptr<elf_progbits> result(new elf_progbits());
  result->buf.resize(sh.get_size());

  // Read the buffer from string tab
  AR.seek(sh.get_offset(), true);
  AR.prologue(sh.get_size());
  AR.read_bytes(&*result->buf.begin(), sh.get_size());
  AR.epilogue(sh.get_size());

  if (!AR) {
    // Unable to read the progbits tab
    return shared_ptr<elf_progbits>();
  }

  return result;
}

char const *elf_progbits::memory_protect() const {
  int protect_type = PROT_READ;
  if (this->section_header->get_flags() & SHF_WRITE) {
    protect_type &= PROT_WRITE;
  }
  if (this->section_header->get_flags() &SHF_EXECINSTR) {
    protect_type &= PROT_EXEC;
  }
  int ret = mprotect((void *)&*buf.begin(), buf.size(), protect_type);
  if (ret == 0) {
    // FIXME: Throws excetion?
    std::cerr<<"Error: Can't mprotect."<<std::endl;
    return 0;
  } else {
    return &*buf.begin();
  }
}

void elf_progbits::dump() const {
  typedef vector<char>::const_iterator iter;
  int t = 0;
  for (iter i = buf.begin(); i != buf.end(); ++i) {
    // FIXME: Use std::cout.
    printf("%.2x ", (unsigned char)*i);
    // XXX: Magic number = =.
    if (++t == 10) {
      t = 0;
      printf("\n");
    }
  }
  printf("\n");
}

template shared_ptr<elf_progbits>
elf_progbits::read(archive_reader_le &, elf_section_header const &);

template shared_ptr<elf_progbits>
elf_progbits::read(archive_reader_be &, elf_section_header const &);
