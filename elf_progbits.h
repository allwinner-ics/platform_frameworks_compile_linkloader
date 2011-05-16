#ifndef ELF_PROGBITS_H
#define ELF_PROGBITS_H

#include "elf_section.h"
#include "utils/serialize.h"

#include <boost/shared_ptr.hpp>
#include <vector>

class elf_progbits : public elf_section {
private:
  boost::shared_ptr<elf_section_header const> section_header;
  std::vector<char> buf;

private:
  elf_progbits() { }

public:
  template <typename Archiver>
  static boost::shared_ptr<elf_progbits> read(Archiver &AR,
                                            elf_section_header const &);

  char *operator[](size_t index) {
    return &*buf.begin() + index;
  }

  char const *operator[](size_t index) const {
    return &*buf.begin() + index;
  }

  size_t size() const {
    return buf.size();
  }

  char const *memory_protect() const;

  void print() const;
};

extern template boost::shared_ptr<elf_progbits>
elf_progbits::read(serialization::archive_reader_le &,
                 elf_section_header const &);

extern template boost::shared_ptr<elf_progbits>
elf_progbits::read(serialization::archive_reader_be &,
                 elf_section_header const &);

#endif // ELF_PROGBITS_H
