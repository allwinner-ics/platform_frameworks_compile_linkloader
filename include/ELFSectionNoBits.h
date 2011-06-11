#ifndef ELF_SECTION_NOBITS_H
#define ELF_SECTION_NOBITS_H

#include "ELFSectionBits.h"
#include "ELFSectionHeader.h"

template <unsigned Bitwidth>
class ELFSectionNoBits : public ELFSectionBits<Bitwidth> {
  friend class ELFSectionBits<Bitwidth>;

public:
  template <typename Archiver>
  static ELFSectionNoBits *
  read(Archiver &AR, ELFSectionHeader<Bitwidth> const *sh) {
    return ELFSectionBits<Bitwidth>::read(AR, sh, new ELFSectionNoBits);
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
