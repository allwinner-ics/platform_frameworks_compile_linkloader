#ifndef ELF_SECTION_SYMTAB_H
#define ELF_SECTION_SYMTAB_H

#include <vector>
#include <string>

template <unsigned Bitwidth> class ELFSymbol;

template <unsigned Bitwidth>
class ELFSectionSymTab : public ELFSection<Bitwidth> {
private:
  std::vector<ELFSymbol<Bitwidth> *> table;

private:
  ELFSectionSymTab() { }

public:
  ~ELFSectionSymTab();

  template <typename Archiver>
  static ELFSectionSymTab *
  read(Archiver &AR,
       ELFObject<Bitwidth> *owner,
       ELFSectionHeader<Bitwidth> const *sh);

  virtual void print() const;

  size_t size() const {
    return table.size();
  }

  ELFSymbol<Bitwidth> const *operator[](size_t index) const {
    return table[index];
  }

  ELFSymbol<Bitwidth> *operator[](size_t index) {
    return table[index];
  }

  ELFSymbol<Bitwidth> const *getByName(std::string const &name) const {
    for (size_t i = 0; i < table.size(); ++i) {
      if (table[i]->getName() == name){
        return table[i];
      }
    }
    return NULL;
  }

  ELFSymbol<Bitwidth> *getByName(std::string const &name) {
    return const_cast<ELFSymbol<Bitwidth> *>(
           const_cast<ELFSectionSymTab<Bitwidth> const *>(this)->getByName(name));
  }

};


//==================Inline Member Function Definition==========================

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

#endif // ELF_SECTION_SYMTAB_H
