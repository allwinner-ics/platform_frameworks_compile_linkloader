#ifndef ELF_SECTION_HEADER_TABLE_H
#define ELF_SECTION_HEADER_TABLE_H

#include "ELFSectionHeader.h"

#include <llvm/ADT/OwningPtr.h>

#include <vector>
#include <string>

#include <assert.h>

template <unsigned Bitwidth> class ELFObject;

template <unsigned Bitwidth>
class ELFSectionHeaderTable {
private:
  std::vector<ELFSectionHeader<Bitwidth> *> table;

private:
  ELFSectionHeaderTable() {
  }

public:
  template <typename Archiver>
  static ELFSectionHeaderTable<Bitwidth> *
  read(Archiver &AR, ELFObject<Bitwidth> *owner);

  ELFSectionHeader<Bitwidth> const *operator[](size_t i) const {
    return table[i];
  }

  ELFSectionHeader<Bitwidth> *operator[](size_t i) {
    return table[i];
  }

  ELFSectionHeader<Bitwidth> const *getByName(const std::string &str) const;
  ELFSectionHeader<Bitwidth> *getByName(const std::string &str);

  void print() const;
};


#include "ELFObject.h"
#include "ELFHeader.h"

template <unsigned Bitwidth>
template <typename Archiver>
inline ELFSectionHeaderTable<Bitwidth> *
ELFSectionHeaderTable<Bitwidth>::read(Archiver &AR,
                                      ELFObject<Bitwidth> *owner) {
  if (!AR) {
    // Archiver is in bad state before calling read function.
    // Return NULL and do nothing.
    return 0;
  }

  // Allocate a new section header table and assign the owner.
  llvm::OwningPtr<ELFSectionHeaderTable> tab(new ELFSectionHeaderTable());

  // Get ELF header
  ELFHeader<Bitwidth> const *header = owner->getHeader();

  assert(header->getSectionHeaderEntrySize() >=
         TypeTraits<ELFSectionHeader<Bitwidth> >::size);

  size_t pending = TypeTraits<ELFSectionHeader<Bitwidth> >::size -
                   header->getSectionHeaderEntrySize();

  // Seek to the address of section header
  AR.seek(header->getSectionHeaderTableOffset(), true);

  for (size_t i = 0; i < header->getSectionHeaderNum(); ++i) {
    llvm::OwningPtr<ELFSectionHeader<Bitwidth> > sh(
      ELFSectionHeader<Bitwidth>::read(AR, owner, i));

    if (!sh) {
      // Something wrong while reading the section header.
      return 0;
    }

    AR.seek(pending);
    tab->table.push_back(sh.take());
  }

  return tab.take();
}

template <unsigned Bitwidth>
inline void ELFSectionHeaderTable<Bitwidth>::print() const {
  using namespace llvm;

  out() << '\n' << fillformat('=', 79) << '\n';
  out().changeColor(raw_ostream::WHITE, true);
  out() << "ELF Section Header Table" << '\n';
  out().resetColor();

  for (size_t i = 0; i < table.size(); ++i) {
    (*this)[i]->print();
  }

  out() << fillformat('=', 79) << '\n';
}

template <unsigned Bitwidth>
inline ELFSectionHeader<Bitwidth> const *
ELFSectionHeaderTable<Bitwidth>::getByName(const std::string &str) const {
  // TODO: Use map
  for (size_t i = 0; i < table.size(); ++i) {
    if (str == std::string(table[i]->getName())) {
      return table[i];
    }
  }
  // Return SHN_UNDEF section header;
  return table[0];
}

template <unsigned Bitwidth>
inline ELFSectionHeader<Bitwidth> *
ELFSectionHeaderTable<Bitwidth>::getByName(const std::string &str) {
  ELFSectionHeaderTable<Bitwidth> const *const_this = this;
  ELFSectionHeader<Bitwidth> const *shptr = const_this->getByName(str);
  // Const cast for the same API's const and non-const versions.
  return const_cast<ELFSectionHeader<Bitwidth> *>(shptr);
}

#endif // ELF_SECTION_HEADER_TABLE_H
