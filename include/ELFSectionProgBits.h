#ifndef ELF_SECTION_PROGBITS_H
#define ELF_SECTION_PROGBITS_H

#include "ELFSectionBits.h"
#include "ELFSectionHeader.h"



template <unsigned Bitwidth>
class ELFSectionProgBits : public ELFSectionBits<Bitwidth> {
  friend class ELFSectionBits<Bitwidth>;

public:
  template <typename Archiver>
  static ELFSectionProgBits *
  read(Archiver &AR, ELFSectionHeader<Bitwidth> const *sh) {
    return ELFSectionBits<Bitwidth>::read(AR, sh, new ELFSectionProgBits);
  }

private:
  template <typename Archiver>
  bool serialize(Archiver &AR) {
    AR.seek(this->section_header->getOffset(), true);
    AR.prologue(this->section_header->getSize());
    AR.read_bytes(this->buf, this->section_header->getSize());
    AR.epilogue(this->section_header->getSize());
    return static_cast<bool>(AR);
  }

  void print() const;
};

#include "impl/ELFSectionProgBits.hxx"

#endif // ELF_SECTION_PROGBITS_H
