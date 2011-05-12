#include "elf_section.h"
#include "utils/serialize.h"

using namespace boost;
using namespace serialization;
using namespace std;


template <typename Archiver>
shared_ptr<elf_section> elf_section::read(Archiver &AR,
                                          elf_section_header const &sh) {
  return shared_ptr<elf_section>();
}

template shared_ptr<elf_section>
elf_section::read(archive_reader_le &, elf_section_header const &);

template shared_ptr<elf_section>
elf_section::read(archive_reader_be &, elf_section_header const &);
