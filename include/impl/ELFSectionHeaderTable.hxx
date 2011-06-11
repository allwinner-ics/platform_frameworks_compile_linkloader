#ifndef ELF_SECTION_HEADER_TABLE_HXX
#define ELF_SECTION_HEADER_TABLE_HXX

#include "ELFHeader.h"
#include "ELFObject.h"
#include "ELFSectionHeader.h"

#include <assert.h>

template <unsigned Bitwidth>
ELFSectionHeaderTable<Bitwidth>::~ELFSectionHeaderTable() {
  for (size_t i = 0; i < table.size(); ++i) {
    delete table[i];
  }
}

template <unsigned Bitwidth>
template <typename Archiver>
inline ELFSectionHeaderTable<Bitwidth> *
ELFSectionHeaderTable<Bitwidth>::read(Archiver &AR, ELFObjectTy *owner) {
  if (!AR) {
    // Archiver is in bad state before calling read function.
    // Return NULL and do nothing.
    return 0;
  }

  // Allocate a new section header table and assign the owner.
  llvm::OwningPtr<ELFSectionHeaderTable> tab(new ELFSectionHeaderTable());

  // Get ELF header
  ELFHeaderTy const *header = owner->getHeader();

  assert(header->getSectionHeaderEntrySize() ==
         TypeTraits<ELFSectionHeaderTy>::size);

  // Seek to the address of section header
  AR.seek(header->getSectionHeaderTableOffset(), true);

  for (size_t i = 0; i < header->getSectionHeaderNum(); ++i) {
    llvm::OwningPtr<ELFSectionHeaderTy> sh(
      ELFSectionHeaderTy::read(AR, owner, i));

    if (!sh) {
      // Something wrong while reading the section header.
      return 0;
    }

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
  ELFSectionHeaderTableTy const *const_this = this;
  ELFSectionHeaderTy const *shptr = const_this->getByName(str);
  // Const cast for the same API's const and non-const versions.
  return const_cast<ELFSectionHeaderTy *>(shptr);
}

#endif // ELF_SECTION_HEADER_TABLE_HXX
