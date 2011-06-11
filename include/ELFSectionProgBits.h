#ifndef ELF_SECTION_PROGBITS_H
#define ELF_SECTION_PROGBITS_H

#include "ELFTypes.h"
#include "ELFSectionBits.h"
#include "ELFSectionHeader.h"

template <unsigned Bitwidth>
class ELFSectionProgBits : public ELFSectionBits<Bitwidth> {
  friend class ELFSectionBits<Bitwidth>;

public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

public:
  template <typename Archiver>
  static ELFSectionProgBits *
  read(Archiver &AR, ELFSectionHeaderTy const *sh) {
    return ELFSectionBitsTy::read(AR, sh, new ELFSectionProgBits);
  }

private:
  template <typename Archiver>
  bool serialize(Archiver &AR) {
    AR.seek(this->section_header->getOffset(), true);
    AR.prologue(this->section_header->getSize());
    AR.readBytes(this->buf, this->section_header->getSize());
    AR.epilogue(this->section_header->getSize());
    return static_cast<bool>(AR);
  }

  void print() const;
};

#include "impl/ELFSectionProgBits.hxx"

#endif // ELF_SECTION_PROGBITS_H
