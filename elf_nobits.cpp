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
  // TODO: Align.
  result->buf_size = sh.get_size();
  if (result->buf_size > 0) {
    result->buf = (char *)mmap(0, result->buf_size, PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  }

  // Check map success.
  if (result->buf == MAP_FAILED) {
    return shared_ptr<elf_nobits>();
  }

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
  if (buf_size > 0) {
    int ret = mprotect((void *)buf, buf_size, protect_type);
    if (ret == -1) {
      // FIXME: Throws excetion?
      std::cerr<<"Error: Can't mprotect."<<std::endl;
      return 0;
    }
  }
  return buf;
}

void elf_nobits::print() const {
  using namespace term::color;

  cout << endl << setw(79) << setfill('=') << '=' << endl;

  cout << light::white() << "NOBITS:" <<
          section_header->get_name() << normal() << endl;

  cout << setw(79) << setfill('-') << '-' << endl << setfill(' ');

  cout << "Size: " << this->size() << endl;

  if (this->size() > 0) {
    char const *start = buf;
    char const *end = buf + buf_size - 1;
    printf("%08LX ~ %08LX\n",
           (unsigned long long)start,
           (unsigned long long)end);
  }
  if (this->size() > 0) {
    char const *start = buf;
    char const *end = buf + buf_size - 1;
    char const *line_start = (char const *)((uint64_t)start & (~0xFLL));
    char const *line_end = (char const *)((uint64_t)end | (0xFLL));
#define CHECK_IN_BOUND(x) (start <= (x) && (x) <= end)
    for (char const *i = line_start; i <= line_end;) {
      // FIXME: Use std::cout.
      printf("%08LX: ", (unsigned long long)i);
      for (int k = 0; k < 2;++k) {
        printf(" ");
        for (int j = 0; j < 8; ++j) {
          if (CHECK_IN_BOUND(i)) {
            printf(" %.2x", (uint8_t)*i);
          } else {
            printf("   ");
          }
          ++i;
        }
      }
      printf("\n");
    }
#undef CHECK_IN_BOUND
  }
  cout << setw(79) << setfill('=') << '=' << endl;

}

template shared_ptr<elf_nobits>
elf_nobits::read(archive_reader_le &, elf_section_header const &);

template shared_ptr<elf_nobits>
elf_nobits::read(archive_reader_be &, elf_section_header const &);
