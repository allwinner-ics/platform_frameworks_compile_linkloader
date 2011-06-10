#ifndef ELF_SECTION_H
#define ELF_SECTION_H

#include "utils/serialize.h"

#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/raw_ostream.h>

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

#include "ELFSectionHeader.h"
#include "ELFSectionStrTab.h"
#include "ELFSectionSymTab.h"
#include "ELFSectionProgBits.h"
#include "ELFSectionNoBits.h"
#include "ELFSectionRelTable.h"

template <unsigned Bitwidth>
template <typename Archiver>
inline ELFSection<Bitwidth> *
ELFSection<Bitwidth>::read(Archiver &AR,
                           ELFObject<Bitwidth> *owner,
                           ELFSectionHeader<Bitwidth> const *sh) {
  using namespace std;

  switch ((uint32_t)sh->getType()) {
    default:
      // Uknown type of ELF section.  Return NULL.
      llvm::errs() << "WARNING: Unknown section type.\n";
      return 0;

    case SHT_STRTAB:
      return ELFSectionStrTab<Bitwidth>::read(AR, sh);

    case SHT_SYMTAB:
      return ELFSectionSymTab<Bitwidth>::read(AR, owner, sh);

    case SHT_PROGBITS:
    {
#ifdef __arm__
      owner->getStubLayout();
#endif
      return ELFSectionProgBits<Bitwidth>::read(AR, sh);
    }

    case SHT_NOBITS:
      return ELFSectionNoBits<Bitwidth>::read(AR, sh);

    case SHT_REL:
    case SHT_RELA:
      return ELFSectionRelTable<Bitwidth>::read(AR, sh);

    case SHT_NULL:
      // TODO: Not Yet Implemented
      return 0;
  };
}


#endif // ELF_SECTION_H
