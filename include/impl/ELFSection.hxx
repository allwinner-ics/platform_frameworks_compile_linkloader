#ifndef ELF_SECTION_HXX
#define ELF_SECTION_HXX

#include "utils/raw_ostream.h"

#include <llvm/Support/raw_ostream.h>

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
                           ELFObjectTy *owner,
                           ELFSectionHeaderTy const *sh) {
  using namespace std;

  switch (sh->getType()) {
    default:
      // Uknown type of ELF section.  Return NULL.
      //llvm::errs() << "WARNING: Unknown section type.\n";
      return 0;

    case SHT_STRTAB:
      return ELFSectionStrTabTy::read(AR, sh);

    case SHT_SYMTAB:
      return ELFSectionSymTabTy::read(AR, owner, sh);

    case SHT_PROGBITS:
      return ELFSectionProgBitsTy::read(AR, owner, sh);

    case SHT_NOBITS:
      return ELFSectionNoBitsTy::read(AR, sh);

    case SHT_REL:
    case SHT_RELA:
      return ELFSectionRelTableTy::read(AR, sh);

    case SHT_NULL:
      // TODO: Not Yet Implemented
      return 0;
  };
}

#endif // ELF_SECTION_HXX
