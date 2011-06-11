#ifndef ELF_SECTION_H
#define ELF_SECTION_H

#include <llvm/ADT/OwningPtr.h>

template <unsigned Bitwidth> class ELFObject;
template <unsigned Bitwidth> class ELFSectionHeader;

template <unsigned Bitwidth>
class ELFSection {
protected:
  ELFSection() { }

public:
  virtual ~ELFSection() { }

  virtual void print() const = 0;

  template <typename Archiver>
  static ELFSection *read(Archiver &AR,
                          ELFObject<Bitwidth> *owner,
                          ELFSectionHeader<Bitwidth> const *);
};

#include "impl/ELFSection.hxx"

#endif // ELF_SECTION_H
