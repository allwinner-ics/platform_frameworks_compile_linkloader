#ifndef ELF_SECTION_TABLE_CRTP_H
#define ELF_SECTION_TABLE_CRTP_H

#include "utils/serialize.h"
#include "utils/raw_ostream.h"

#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/Format.h>
#include <llvm/Support/raw_ostream.h>

#include <vector>

template <unsigned Bitwidth> class ELFSection;

template <unsigned Bitwidth, typename ConcreteTable, typename TableEntry>
class ELFSectionTable_CRTP : public ELFSection<Bitwidth> {
protected:
  std::vector<TableEntry *> table;

  ELFSectionTable_CRTP() { }

public:
  template <typename Archiver>
  static ConcreteTable *
  read(Archiver &AR,
       ELFObject<Bitwidth> *owner,
       ELFSectionHeader<Bitwidth> const *sh);

  virtual void print() const;

  size_t size() const {
    return table.size();
  }

  TableEntry const *operator[](size_t index) const {
    return table[index];
  }

  TableEntry *operator[](size_t index) {
    return table[index];
  }

  ~ELFSectionTable_CRTP() {
    for (size_t i = 0; i < table.size(); ++i) {
      // Delete will check the pointer is nullptr or not by himself.
      delete table[i];
    }
  }
};


//==================Inline Member Function Definition==========================


#include "ELFSection.h"
#include "ELFSectionHeader.h"

template <unsigned Bitwidth, typename ConcreteTable, typename TableEntry>
template <typename Archiver>
inline ConcreteTable *
ELFSectionTable_CRTP<Bitwidth, ConcreteTable, TableEntry>::
read(Archiver &AR,
     ELFObject<Bitwidth> *owner,
     ELFSectionHeader<Bitwidth> const *sh) {

  llvm::OwningPtr<ConcreteTable> st(new ConcreteTable());

  // Assert that entry size will be the same as standard.
  assert(sh->getEntrySize() == TypeTraits<TableEntry>::size);

  // Read all symbol table entry
  size_t tsize = sh->getSize() / sh->getEntrySize();
  for (size_t i = 0; i < tsize; ++i) {
    // Seek to symbol table start address
    AR.seek(sh->getOffset() + i * sh->getEntrySize(), true);
    st->table.push_back(TableEntry::read(AR, owner, i));
  }

  if (!AR) {
    // Unable to read the table.
    return 0;
  }

  return st.take();
}

template <unsigned Bitwidth, typename ConcreteTable, typename TableEntry>
inline void
ELFSectionTable_CRTP<Bitwidth, ConcreteTable, TableEntry>::print() const {
  using namespace llvm;

  out() << '\n' << fillformat('=', 79) << '\n';
  out().changeColor(raw_ostream::WHITE, true);
  out() << ConcreteTable::TABLE_NAME << '\n';
  out().resetColor();

  for (size_t i = 0; i < this->size(); ++i) {
    (*this)[i]->print();
  }

  out() << fillformat('=', 79) << '\n';
}

#endif // ELF_SECTION_TABLE_CRTP_H
