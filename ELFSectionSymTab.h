#ifndef ELF_SECTION_SYMTAB_H
#define ELF_SECTION_SYMTAB_H

#include "ELFSection.h"

#include "utils/serialize.h"
#include "utils/term.h"

#include <boost/shared_ptr.hpp>
#include <vector>

template <size_t Bitwidth> class ELFObject;
template <size_t Bitwidth> class ELFSectionHeader;
template <size_t Bitwidth> class ELFSectionSymTabEntry;

template <size_t Bitwidth>
class ELFSectionSymTab : public ELFSection<Bitwidth> {
public:
  typedef ELFSectionSymTabEntry<Bitwidth> SymTabEntry;

private:
  std::vector<boost::shared_ptr<SymTabEntry> > symbol_table;

private:
  ELFSectionSymTab() { }

public:
  template <typename Archiver>
  static boost::shared_ptr<ELFSectionSymTab>
  read(Archiver &AR,
       ELFObject<Bitwidth> *owner,
       ELFSectionHeader<Bitwidth> const *sh);

  virtual void print() const;

  size_t size() const {
    return symbol_table.size();
  }

  SymTabEntry const *operator[](size_t index) const {
    return symbol_table[index].get();
  }

  SymTabEntry const *operator[](const std::string &str) const;
};


//==================Inline Member Function Definition==========================

//#include "ELFObject.h"
//#include "ELFHeader.h"
#include "ELFSectionSymTabEntry.h"
//#include "ELFSection.h"
#include "ELFTypes.h"

template <size_t Bitwidth>
template <typename Archiver>
inline boost::shared_ptr<ELFSectionSymTab<Bitwidth> >
ELFSectionSymTab<Bitwidth>::read(Archiver &AR,
                                 ELFObject<Bitwidth> *owner,
                                 ELFSectionHeader<Bitwidth> const *sh) {
  using namespace boost;

  shared_ptr<ELFSectionSymTab> st(new ELFSectionSymTab());

  // Assert that entry size will be the same as standard.
  assert(sh->getEntrySize() ==
         TypeTraits<ELFSectionSymTabEntry<Bitwidth> >::size);

  // Read all symbol table entry

  size_t tsize = sh->getSize() / sh->getEntrySize();
  for (size_t i = 0; i < tsize; ++i) {
    // Seek to symbol table start address
    AR.seek(sh->getOffset() + i*sh->getEntrySize(), true);
    st->symbol_table.push_back(SymTabEntry::read(AR, owner, i));
  }

  if (!AR) {
    // Unable to read the string table.
    return shared_ptr<ELFSectionSymTab>();
  }

  return st;
}

template <size_t Bitwidth>
inline void ELFSectionSymTab<Bitwidth>::print() const {
  using namespace std;
  using namespace term::color;

  cout << endl << setw(79) << setfill('=') << '=' << endl;
  cout << light::white() << "Symbol Table" << normal() << endl;

  for (size_t i = 0; i < this->size(); ++i) {
      (*this)[i]->print();
  }

  cout << setw(79) << setfill('=') << '=' << endl << endl;

// TODO: Horizontal type or vertical type can use option to decide.
#if 0
  cout << endl << setw(79) << setfill('=') << '=' << endl;

  cout << light::white() << "Symbol Table" << normal() << endl;

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
      (*this)[i]->print();
  }

  cout << setw(79) << setfill('=') << '=' << endl;
#endif
}

template <size_t Bitwidth>
inline ELFSectionSymTabEntry<Bitwidth> const *
ELFSectionSymTab<Bitwidth>::operator[](const std::string &str) const {
  // TODO: Use map
  for (size_t i = 0; i < symbol_table.size(); ++i) {
//    if (str == string(symbol_table[i]->getName())) {
//      return symbol_table[i].get();
//    }
  }
  // Return STN_UNDEF entry.
  return symbol_table[0].get();
}

#endif // ELF_SECTION_SYMTAB_H
