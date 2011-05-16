#include "elf_nobits.h"
#include "elf_section_header.h"
#include "utils/serialize.h"
#include "utils/term.h"

#include "elf.h"
#include <sys/mman.h>
#include <iostream>
#include <iomanip>
#include <cstdio>

using namespace boost;
using namespace serialization;
using namespace std;

// TODO: So many duplicate codes, progbits can be a special nobits?

template <typename Archiver>
shared_ptr<elf_nobits> elf_nobits::read(Archiver &AR,
                                        elf_section_header const &sh) {
  // Check section type
  if (!(sh.get_type() & SHT_NOBITS)) {
    // This is not a nobits section
    return shared_ptr<elf_nobits>();
  }

  // Reserve enough buffer
  shared_ptr<elf_nobits> result(new elf_nobits());
  result->buf.resize(sh.get_size());

  // Save section_header
  result->section_header = shared_ptr<elf_section_header const>(&sh);

  return result;
}

char const *elf_nobits::memory_protect() const {
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

void elf_nobits::print() const {
  using namespace term::color;

  cout << endl << setw(79) << setfill('=') << '=' << endl;

  cout << light::white() << "NOBITS:" <<
          section_header->get_name() << normal() << endl;

  cout << setw(79) << setfill('-') << '-' << endl << setfill(' ');

  cout << "Size: " << this->size() << endl;

  if (this->size() > 0) {
    char const *start = (*this)[0];
    char const *end = (*this)[this->size()-1];
    printf("%08LX ~ %08LX\n",
           (unsigned long long)start,
           (unsigned long long)end);
  }
  cout << setw(79) << setfill('=') << '=' << endl;

}

template shared_ptr<elf_nobits>
elf_nobits::read(archive_reader_le &, elf_section_header const &);

template shared_ptr<elf_nobits>
elf_nobits::read(archive_reader_be &, elf_section_header const &);
