#include "elf_strtab.h"
#include "elf_section_header.h"
#include "utils/serialize.h"

#include "elf.h"

using namespace boost;
using namespace serialization;
using namespace std;


template <typename Archiver>
shared_ptr<elf_strtab> elf_strtab::read(Archiver &AR,
                                        elf_section_header const &sh) {
  // Check section type
  if (!(sh.get_type() & SHT_STRTAB)) {
    // This is not a STRTAB section
    return shared_ptr<elf_strtab>();
  }

  // Reserve enough buffer
  shared_ptr<elf_strtab> result(new elf_strtab());
  result->buf.resize(sh.get_size());

  // Read the buffer from string tab
  AR.seek(sh.get_offset(), true);
  AR.prologue(sh.get_size());
  AR.read_bytes(&*result->buf.begin(), sh.get_size());
  AR.epilogue(sh.get_size());

  if (!AR) {
    // Unable to read the string tab
    return shared_ptr<elf_strtab>();
  }

  // A guard to prevent runtime error from bad string table
  result->buf.push_back('\0');

  return result;
}

template shared_ptr<elf_strtab>
elf_strtab::read(archive_reader_le &, elf_section_header const &);

template shared_ptr<elf_strtab>
elf_strtab::read(archive_reader_be &, elf_section_header const &);
