#ifndef ELF_STRTAB_H
#define ELF_STRTAB_H

#include "elf_section.h"
#include "utils/serialize.h"

#include <boost/shared_ptr.hpp>
#include <vector>

class elf_section_header;

class elf_strtab : public elf_section {
private:
  std::vector<char> buf;

private:
  elf_strtab() { }

public:
  template <typename Archiver>
  static boost::shared_ptr<elf_strtab> read(Archiver &AR,
                                            elf_section_header const &);

  char const *operator[](size_t index) const {
    return &*buf.begin() + index;
  }
};

extern template boost::shared_ptr<elf_strtab>
elf_strtab::read(serialization::archive_reader_le &,
                 elf_section_header const &);

extern template boost::shared_ptr<elf_strtab>
elf_strtab::read(serialization::archive_reader_be &,
                 elf_section_header const &);

#endif // ELF_STRTAB_H
