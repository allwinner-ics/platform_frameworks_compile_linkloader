#ifndef ELF_SECTION_SYM_TAB_HXX
#define ELF_SECTION_SYM_TAB_HXX

#include "ELFSectionHeader.h"
#include "ELFSymbol.h"

template <unsigned Bitwidth>
ELFSectionSymTab<Bitwidth>::~ELFSectionSymTab() {
  for (size_t i = 0; i < table.size(); ++i) {
    delete table[i];
  }
}

template <unsigned Bitwidth>
template <typename Archiver>
ELFSectionSymTab<Bitwidth> *
ELFSectionSymTab<Bitwidth>::read(Archiver &AR,
                                 ELFObject<Bitwidth> *owner,
                                 ELFSectionHeader<Bitwidth> const *sh) {

  llvm::OwningPtr<ELFSectionSymTab<Bitwidth> > st(
    new ELFSectionSymTab<Bitwidth>());

  // Assert that entry size will be the same as standard.
  assert(sh->getEntrySize() == TypeTraits<ELFSymbol<Bitwidth> >::size);

  // Seek to the start of symbol table
  AR.seek(sh->getOffset(), true);

  // Read all symbol table entry
  size_t size = sh->getSize() / sh->getEntrySize();
  for (size_t i = 0; i < size; ++i) {
    st->table.push_back(ELFSymbol<Bitwidth>::read(AR, owner, i));
  }

  if (!AR) {
    // Unable to read the table.
    return 0;
  }

  return st.take();
}

template <unsigned Bitwidth>
void ELFSectionSymTab<Bitwidth>::print() const {
  using namespace llvm;

  out() << '\n' << fillformat('=', 79) << '\n';
  out().changeColor(raw_ostream::WHITE, true);
  out() << "Symbol Table" << '\n';
  out().resetColor();

  for (size_t i = 0; i < table.size(); ++i) {
    table[i]->print();
  }

  out() << fillformat('=', 79) << '\n';
}

#endif // ELF_SECTION_SYM_TAB_HXX
