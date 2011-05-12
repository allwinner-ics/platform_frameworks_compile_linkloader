#ifndef ELF_SECTION_H
#define ELF_SECTION_H

#include "utils/serialize.h"
#include <boost/shared_ptr.hpp>

class elf_section_header;

class elf_section {
public:
  template <typename Archiver>
  static boost::shared_ptr<elf_section> read(Archiver &AR,
                                             elf_section_header const &);
};

extern template boost::shared_ptr<elf_section>
elf_section::read(serialization::archive_reader_le &,
                  elf_section_header const &);

extern template boost::shared_ptr<elf_section>
elf_section::read(serialization::archive_reader_be &,
                  elf_section_header const &);



#endif // ELF_SECTION_H
