#ifndef ELF_SECTION_TABLE_CRTP_H
#define ELF_SECTION_TABLE_CRTP_H

/*
 * This class is a table prototype.
 */
#include "ELFTypes.h"
#include "ELFHeader.h"

#include "utils/serialize.h"

#include <boost/shared_ptr.hpp>
#include <vector>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Format.h>

template <size_t Bitwidth> class ELFObject;
template <size_t Bitwidth> class ELFSection;
template <size_t Bitwidth> class ELFSectionHeader;

template <size_t Bitwidth, typename ConcreteTable, typename TableEntry>
class ELFSectionTable_CRTP : public ELFSection<Bitwidth> {
//  friend class ConcreteTable;

protected:
  std::vector<boost::shared_ptr<TableEntry> > table;

  ELFSectionTable_CRTP() { }

public:
  template <typename Archiver>
  static boost::shared_ptr<ConcreteTable>
  read(Archiver &AR,
       ELFObject<Bitwidth> *owner,
       ELFSectionHeader<Bitwidth> const *sh);

  virtual void print() const;

  size_t size() const {
    return table.size();
  }

  TableEntry const *operator[](size_t index) const {
    return table[index].get();
  }

  TableEntry *operator[](size_t index) {
    return table[index].get();
  }
};


//==================Inline Member Function Definition==========================


#include "ELFSection.h"
#include "ELFSectionHeader.h"

template <size_t Bitwidth, typename ConcreteTable, typename TableEntry>
template <typename Archiver>
inline boost::shared_ptr<ConcreteTable>
ELFSectionTable_CRTP<Bitwidth, ConcreteTable, TableEntry>::
  read(Archiver &AR,
       ELFObject<Bitwidth> *owner,
       ELFSectionHeader<Bitwidth> const *sh) {
  using namespace boost;

  shared_ptr<ConcreteTable> st(new ConcreteTable());

  // Assert that entry size will be the same as standard.
  assert(sh->getEntrySize() ==
         TypeTraits<TableEntry>::size);

  // Read all symbol table entry

  size_t tsize = sh->getSize() / sh->getEntrySize();
  for (size_t i = 0; i < tsize; ++i) {
    // Seek to symbol table start address
    AR.seek(sh->getOffset() + i*sh->getEntrySize(), true);
    st->table.push_back(TableEntry::read(AR, owner, i));
  }

  if (!AR) {
    // Unable to read the string table.
    return shared_ptr<ConcreteTable>();
  }

  return st;
}

template <size_t Bitwidth, typename ConcreteTable, typename TableEntry>
inline void
ELFSectionTable_CRTP<Bitwidth, ConcreteTable, TableEntry>::print() const {
  using namespace llvm;

  out() << '\n' << fillformat('=', 79) << '\n';
  out().changeColor(raw_ostream::WHITE, true);
  out() << ConcreteTable::TABLE_NAME;
  out().resetColor();
  out() << '\n';

  for (size_t i = 0; i < this->size(); ++i) {
      (*this)[i]->print();
  }

  out() << fillformat('=', 79) << '\n';
}

#endif // ELF_SECTION_TABLE_CRTP_H
