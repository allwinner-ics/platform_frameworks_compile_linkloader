#ifndef ELF_SECTION_NOBITS_HXX
#define ELF_SECTION_NOBITS_HXX

#include "ELFTypes.h"

#include "utils/raw_ostream.h"

#include <llvm/Support/Format.h>
#include <llvm/Support/raw_ostream.h>

#include <sys/mman.h>

template <unsigned Bitwidth>
template <typename Archiver>
inline ELFSectionNoBits<Bitwidth> *
ELFSectionNoBits<Bitwidth>::read(Archiver &AR, ELFSectionHeaderTy const *sh) {
  llvm::OwningPtr<ELFSectionNoBits> result(new ELFSectionNoBits());

  if (!result->chunk.allocate(sh->getSize())) {
    return NULL;
  }

  result->sh = sh;

  return result.take();
}

#endif // ELF_SECTION_NOBITS_HXX
