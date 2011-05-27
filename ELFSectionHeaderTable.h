#ifndef ELF_SECTION_HEADER_TABLE_H
#define ELF_SECTION_HEADER_TABLE_H

#include "ELFSectionHeader.h"
#include "ELFSectionTable_CRTP.h"

#include <llvm/ADT/OwningPtr.h>
#include <vector>
#include <string>

#include <assert.h>

template <size_t Bitwidth> class ELFObject;

template <size_t Bitwidth>
class ELFSectionHeaderTable :
  public ELFSectionTable_CRTP<Bitwidth,
                              ELFSectionHeaderTable<Bitwidth>,
                              ELFSectionHeader<Bitwidth> > {
  friend class ELFSectionTable_CRTP<Bitwidth,
                                    ELFSectionHeaderTable<Bitwidth>,
                                    ELFSectionHeader<Bitwidth> >;
private:
  static std::string const TABLE_NAME;
private:
  //std::vector<ELFSectionHeader<Bitwidth> *> table;

private:
  ELFSectionHeaderTable() {
  }

public:
  template <typename Archiver>
  static ELFSectionHeaderTable<Bitwidth> *
  read(Archiver &AR, ELFObject<Bitwidth> *owner);

  //ELFSectionHeader<Bitwidth> const *operator[](size_t i) const;
  //ELFSectionHeader<Bitwidth> *operator[](size_t i);

  ELFSectionHeader<Bitwidth> const *getByName(const std::string &str) const;
  ELFSectionHeader<Bitwidth> *getByName(const std::string &str);

  //virtual void print() const;
};


#include "ELFObject.h"
#include "ELFHeader.h"

template <size_t Bitwidth>
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

template <size_t Bitwidth>
std::string const ELFSectionHeaderTable<Bitwidth>::
  TABLE_NAME("ELF Section Header Table");

template <size_t Bitwidth>
inline ELFSectionHeader<Bitwidth> const *
ELFSectionHeaderTable<Bitwidth>::getByName(const std::string &str) const {
  // TODO: Use map
  for (size_t i = 0; i < this->table.size(); ++i) {
    if (str == std::string(this->table[i]->getName())) {
      return this->table[i].get();
    }
  }
  // Return SHN_UNDEF section header;
  return this->table[0].get();
}

template <size_t Bitwidth>
inline ELFSectionHeader<Bitwidth> *
ELFSectionHeaderTable<Bitwidth>::getByName(const std::string &str) {
  ELFSectionHeader<Bitwidth> const *shptr = (*this)[str];
  // Const cast for the same API's const and non-const versions.
  return const_cast<ELFSectionHeader<Bitwidth> *>(shptr);
}
#endif // ELF_SECTION_HEADER_TABLE_H
