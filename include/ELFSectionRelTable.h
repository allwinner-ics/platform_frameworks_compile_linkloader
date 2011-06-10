#ifndef ELF_SECTION_REL_TABLE_H
#define ELF_SECTION_REL_TABLE_H

#include <string>
#include <vector>

#include "ELFSection.h"

template <unsigned Bitwidth> class ELFRel;

template <unsigned Bitwidth>
class ELFSectionRelTable : public ELFSection<Bitwidth> {
private:
  std::vector<ELFRel<Bitwidth> *> rel_table;

private:
  ELFSectionRelTable() { }

public:
  virtual ~ELFSectionRelTable();

  virtual void print() const;

  template <typename Archiver>
  static ELFSectionRelTable *
  read(Archiver &AR,
       ELFSectionHeader<Bitwidth> const *sh);

  size_t size() const {
    return rel_table.size();
  }

  ELFRel<Bitwidth> const *operator[](size_t index) const {
    return rel_table[index];
  }

  ELFRel<Bitwidth> *operator[](size_t index) {
    return rel_table[index];
  }
};


#include "ELFRel.h"

template <unsigned Bitwidth>
ELFSectionRelTable<Bitwidth>::~ELFSectionRelTable() {
  using namespace std;
  for (size_t i = 0; i < rel_table.size(); ++i) {
    delete rel_table[i];
  }
}

template <unsigned Bitwidth>
void ELFSectionRelTable<Bitwidth>::print() const {
  using namespace llvm;

  out() << '\n' << fillformat('=', 79) << '\n';
  out().changeColor(raw_ostream::WHITE, true);
  out() << "Relocation Table" << '\n';
  out().resetColor();

  for (size_t i = 0; i < this->size(); ++i) {
    (*this)[i]->print();
  }

  out() << fillformat('=', 79) << '\n';
}

#include "ELFRel.h"

template <unsigned Bitwidth>
template <typename Archiver>
ELFSectionRelTable<Bitwidth> *
ELFSectionRelTable<Bitwidth>::read(Archiver &AR,
                                   ELFSectionHeader<Bitwidth> const *sh) {

  assert(sh->getType() == SHT_REL || sh->getType() == SHT_RELA);

  llvm::OwningPtr<ELFSectionRelTable> rt(new ELFSectionRelTable());

  // Seek to the start of the table
  AR.seek(sh->getOffset(), true);

  // Count the relocation entries
  size_t size = sh->getSize() / sh->getEntrySize();

  // Read every relocation entries
  if (sh->getType() == SHT_REL) {
    assert(sh->getEntrySize() == TypeTraits<ELFRel<Bitwidth> >::size);
    for (size_t i = 0; i < size; ++i) {
      rt->rel_table.push_back(ELFRel<Bitwidth>::readRel(AR, i));
    }

  } else {
    assert(sh->getEntrySize() == TypeTraits<ELFRela<Bitwidth> >::size);
    for (size_t i = 0; i < size; ++i) {
      rt->rel_table.push_back(ELFRel<Bitwidth>::readRela(AR, i));
    }
  }

  if (!AR) {
    // Unable to read the table.
    return 0;
  }

  return rt.take();
}

#endif // ELF_SECTION_REL_TABLE_H
