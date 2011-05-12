#include "elf_strtab.h"
#include "utils/serialize.h"

using namespace boost;
using namespace serialization;
using namespace std;


template <typename Archiver>
shared_ptr<elf_strtab> elf_strtab::read(Archiver &AR,
                                        elf_section_header const &sh) {
  return shared_ptr<elf_strtab>();
}

template shared_ptr<elf_strtab>
elf_strtab::read(archive_reader_le &, elf_section_header const &);

template shared_ptr<elf_strtab>
elf_strtab::read(archive_reader_be &, elf_section_header const &);
