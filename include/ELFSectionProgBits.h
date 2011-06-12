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
                                  ELFObjectTy const *owner,
                                  ELFSectionHeaderTy const *sh);

private:
  template <typename Archiver>
  bool serialize(Archiver &AR) {
    AR.seek(this->section_header->getOffset(), true);
    AR.prologue(this->section_header->getSize());
    AR.readBytes(this->buf, this->section_header->getSize());
    AR.epilogue(this->section_header->getSize());
    return static_cast<bool>(AR);
  }
};

#include "impl/ELFSectionProgBits.hxx"

#endif // ELF_SECTION_PROGBITS_H
