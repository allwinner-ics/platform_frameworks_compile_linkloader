#ifndef ELF_OBJECT_H
#define ELF_OBJECT_H

#include <llvm/ADT/OwningPtr.h>
#include <vector>

template <size_t Bitwidth> class ELFHeader;
template <size_t Bitwidth> class ELFSection;
template <size_t Bitwidth> class ELFSectionHeaderTable;

template <size_t Bitwidth>
class ELFObject {
private:
  llvm::OwningPtr<ELFHeader<Bitwidth> > header;
  llvm::OwningPtr<ELFSectionHeaderTable<Bitwidth> > shtab;
  std::vector<ELFSection<Bitwidth> *> stab;

private:
  ELFObject() { }

public:
  template <typename Archiver>
  static ELFObject *read(Archiver &AR);

  ELFHeader<Bitwidth> const *getHeader() const {
    return header.get();
  }

  ELFSectionHeaderTable<Bitwidth> const *getSectionHeaderTable() const {
    return shtab.get();
  }

  char const *getSectionName(size_t i) const;
  ELFSection<Bitwidth> const *getSectionByIndex(size_t i) const;

  void print() const;

  ~ELFObject() {
    for (size_t i = 0; i < stab.size(); ++i) {
      // Delete will check the pointer is nullptr or not by himself.
      delete stab[i];
    }
  }
};

#include "ELFHeader.h"
#include "ELFSection.h"
#include "ELFSectionHeaderTable.h"

template <size_t Bitwidth>
template <typename Archiver>
inline ELFObject<Bitwidth> *
ELFObject<Bitwidth>::read(Archiver &AR) {
  llvm::OwningPtr<ELFObject<Bitwidth> > object(new ELFObject<Bitwidth>());

  // Read header
  object->header.reset(ELFHeader<Bitwidth>::read(AR));
  if (!object->header) {
    return 0;
  }

  // Read section table
  object->shtab.reset(ELFSectionHeaderTable<Bitwidth>::read(AR, object.get()));
  if (!object->shtab) {
    return 0;
  }

  // Read each section
  for (size_t i = 0; i < object->header->getSectionHeaderNum(); ++i) {
    llvm::OwningPtr<ELFSection<Bitwidth> > sec(
      ELFSection<Bitwidth>::read(AR, object.get(), (*object->shtab)[i]));
    object->stab.push_back(sec.take());
  }

  return object.take();
}

template <size_t Bitwidth>
inline char const *ELFObject<Bitwidth>::getSectionName(size_t i) const {
  ELFSection<Bitwidth> const *sec =
    stab[header->getStringSectionIndex()];

  if (sec) {
    ELFSectionStrTab<Bitwidth> const &st =
      static_cast<ELFSectionStrTab<Bitwidth> const &>(*sec);
    return st[i];
  }

  return NULL;
}

template <size_t Bitwidth>
inline ELFSection<Bitwidth> const *
ELFObject<Bitwidth>::getSectionByIndex(size_t i) const {
  return stab[i];
}

template <size_t Bitwidth>
inline void ELFObject<Bitwidth>::print() const {
  header->print();
  shtab->print();

  for (size_t i = 0; i < stab.size(); ++i) {
    ELFSection<Bitwidth> *sec = stab[i];
    if (sec) {
      sec->print();
    }
  }
}

#endif // ELF_OBJECT_H
