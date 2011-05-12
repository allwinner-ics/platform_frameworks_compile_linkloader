#include "elf_symtab.h"
#include "elf_section_header.h"
#include "elf_object.h"
#include "elf_strtab.h"
#include "utils/serialize.h"
#include "utils/term.h"

#include <boost/shared_ptr.hpp>

#include <iomanip>
#include <iostream>
#include <string>

#include <elf.h>

using namespace boost;
using namespace serialization;
using namespace std;

template <typename Archiver>
shared_ptr<elf_symtab> elf_symtab::read(Archiver &AR,
                                        elf_section_header const &sh,
                                        elf_object const &obj) {
  // Check section type
  if (!(sh.get_type() & SHT_SYMTAB)) {
    // This is not a STRTAB section
    return shared_ptr<elf_symtab>();
  }

  shared_ptr<elf_symtab> result(new elf_symtab());

  // Read all symbol table entry
  size_t tsize = sh.get_size() / sh.get_entry_size();
  for (size_t i = 0; i < tsize; ++i) {
    // Seek to symbol table start address
    AR.seek(sh.get_offset() + i*sh.get_entry_size(), true);
    result->symbol_table.push_back(elf_sym_entry::read(AR, obj));
  }

  if (!AR) {
    // Unable to read the symbol table
    return shared_ptr<elf_symtab>();
  }

  return result;
}

template boost::shared_ptr<elf_symtab>
elf_symtab::read(serialization::archive_reader_le &,
                 elf_section_header const &,
                 elf_object const &);

template boost::shared_ptr<elf_symtab>
elf_symtab::read(serialization::archive_reader_be &,
                 elf_section_header const &,
                 elf_object const &);


//=============================================================================

void elf_symtab::print() const {
  using namespace term::color;

  cout << endl << setw(79) << setfill('=') << '=' << endl;

  cout << light::white() << "Section Header" << normal() << endl;

  cout << setw(79) << setfill('-') << '-' << endl << setfill(' ');
  cout << setw(20) << "Name" <<
          setw(10) << "Type" <<
          setw(10) << "Bind" <<
          setw(15) << "Visibility" <<
          setw(10) << "SIndex" <<
          setw(7) << "Value" <<
          setw(7) << "Size" <<
          endl;

  for (size_t i = 0; i < this->size(); ++i) {
      (*this)[i].print();
  }

  cout << setw(79) << setfill('=') << '=' << endl;
}

