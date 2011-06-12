#ifndef ELF_SECTION_PROGBITS_H
#define ELF_SECTION_PROGBITS_H

#include "ELFTypes.h"
#include "ELFSectionBits.h"
#include "ELFSectionHeader.h"
#include "MemChunk.h"

template <unsigned Bitwidth>
class ELFSectionProgBits : public ELFSectionBits<Bitwidth> {
public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

public:
  template <typename Archiver>
  static ELFSectionProgBits *read(Archiver &AR,
                                  ELFObjectTy *owner,
                                  ELFSectionHeaderTy const *sh);

private:
  template <typename Archiver>
  bool serialize(Archiver &AR) {
    ELFSectionHeaderTy const *sh = this->sh;
    MemChunk &chunk = this->chunk;

    AR.seek(sh->getOffset(), true);
    AR.prologue(sh->getSize());
    AR.readBytes(chunk.getBuffer(), sh->getSize());
    AR.epilogue(sh->getSize());

    return AR;
  }
};

#include "impl/ELFSectionProgBits.hxx"

#endif // ELF_SECTION_PROGBITS_H
