#ifndef ELF_SECTION_NOBITS_H
#define ELF_SECTION_NOBITS_H

#include "ELFTypes.h"
#include "ELFSectionBits.h"
#include "ELFSectionHeader.h"

template <unsigned Bitwidth>
class ELFSectionNoBits : public ELFSectionBits<Bitwidth> {
  friend class ELFSectionBits<Bitwidth>;

public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

public:
  template <typename Archiver>
  static ELFSectionNoBits *
  read(Archiver &AR, ELFSectionHeaderTy const *sh) {
    return ELFSectionBitsTy::read(AR, sh, new ELFSectionNoBits);
  }

private:
  template <typename Archiver>
  bool serialize(Archiver &AR) {
    // Is mmap init all bytes to 0?
    return true;
  }

  void print() const;
};

#include "impl/ELFSectionNoBits.hxx"

#endif // ELF_SECTION_NOBITS_H
