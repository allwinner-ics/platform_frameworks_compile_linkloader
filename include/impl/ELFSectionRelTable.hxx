#ifndef ELF_SECTION_REL_TABLE_HXX
#define ELF_SECTION_REL_TABLE_HXX

#include "ELFReloc.h"

template <unsigned Bitwidth>
ELFSectionRelTable<Bitwidth>::~ELFSectionRelTable() {
  using namespace std;
  for (size_t i = 0; i < table.size(); ++i) {
    delete table[i];
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
    assert(sh->getEntrySize() == TypeTraits<ELFRelocRel<Bitwidth> >::size);
    for (size_t i = 0; i < size; ++i) {
      rt->table.push_back(ELFReloc<Bitwidth>::readRel(AR, i));
    }

  } else {
    assert(sh->getEntrySize() == TypeTraits<ELFRelocRela<Bitwidth> >::size);
    for (size_t i = 0; i < size; ++i) {
      rt->table.push_back(ELFReloc<Bitwidth>::readRela(AR, i));
    }
  }

  if (!AR) {
    // Unable to read the table.
    return 0;
  }

  return rt.take();
}

#endif // ELF_SECTION_REL_TABLE_HXX
