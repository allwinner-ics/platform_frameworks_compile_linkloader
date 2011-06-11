#ifndef ELF_SECTION_H
#define ELF_SECTION_H

#include "ELFTypes.h"
#include <llvm/ADT/OwningPtr.h>

template <unsigned Bitwidth>
class ELFSection {
public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

protected:
  ELFSection() { }

public:
  virtual ~ELFSection() { }

  virtual void print() const = 0;

  template <typename Archiver>
  static ELFSection *read(Archiver &AR, ELFObjectTy *,
                          ELFSectionHeaderTy const *);
};

#include "impl/ELFSection.hxx"

#endif // ELF_SECTION_H
