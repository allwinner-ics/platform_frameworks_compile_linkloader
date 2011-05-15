#ifndef ELF_SYMTAB_H
#define ELF_SYMTAB_H

#include "utils/serialize.h"

#include <boost/shared_ptr.hpp>
#include <vector>

#include "elf_section.h"
#include "elf_sym_entry.h"

class elf_object;
class elf_section_header;

class elf_symtab : elf_section {
private:
  std::vector<boost::shared_ptr<elf_sym_entry> > symbol_table;

private:
  elf_symtab() { }

public:
  template <typename Archiver>
  static boost::shared_ptr<elf_symtab> read(Archiver &AR,
                                            elf_section_header const &,
                                            elf_object const &);

  void print() const;

  size_t size() const {
    return symbol_table.size();
  }

  elf_sym_entry const &operator[](size_t index) const {
    return *symbol_table[index];
  }

  elf_sym_entry const &operator[](const std::string &str) const;
};

extern template boost::shared_ptr<elf_symtab>
elf_symtab::read(serialization::archive_reader_le &,
                 elf_section_header const &,
                 elf_object const &);

extern template boost::shared_ptr<elf_symtab>
elf_symtab::read(serialization::archive_reader_be &,
                 elf_section_header const &,
                 elf_object const &);

#endif // ELF_SYMTAB_H
