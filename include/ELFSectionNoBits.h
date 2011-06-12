#ifndef ELF_SECTION_NOBITS_H
#define ELF_SECTION_NOBITS_H

#include "ELFTypes.h"
#include "ELFSectionBits.h"
#include "ELFSectionHeader.h"
#include "MemChunk.h"

template <unsigned Bitwidth>
class ELFSectionNoBits : public ELFSectionBits<Bitwidth> {
public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

public:
  template <typename Archiver>
  static ELFSectionNoBits *read(Archiver &AR, ELFSectionHeaderTy const *sh);
};

#include "impl/ELFSectionNoBits.hxx"

#endif // ELF_SECTION_NOBITS_H
