#ifndef ELF_SECTION_PROGBITS_HXX
#define ELF_SECTION_PROGBITS_HXX

#include "ELFTypes.h"

#include <llvm/Support/Format.h>
#include <llvm/Support/raw_ostream.h>

#include "utils/raw_ostream.h"

template <unsigned Bitwidth>
template <typename Archiver>
ELFSectionProgBits<Bitwidth> *
ELFSectionProgBits<Bitwidth>::read(Archiver &AR,
                                   ELFObjectTy const *owner,
                                   ELFSectionHeaderTy const *sh) {

  llvm::OwningPtr<ELFSectionProgBits> result(new ELFSectionProgBits());

  // FIXME: We should add extra space for stub.
  if (!result->chunk.allocate(sh->getSize())) {
    return NULL;
  }

  result->sh = sh;

  return result.take();
}

#endif // ELF_SECTION_PROGBITS_HXX
