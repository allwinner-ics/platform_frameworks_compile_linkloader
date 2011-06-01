#ifndef ELF_SECTION_NOBITS_H
#define ELF_SECTION_NOBITS_H

#include "ELFTypes.h"
#include "ELFSectionBits.h"
#include "ELFSectionHeader.h"

#include <llvm/Support/Format.h>
#include <llvm/Support/raw_ostream.h>

#include "utils/raw_ostream.h"


template <size_t Bitwidth>
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

  void print() const {
    ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

    using namespace llvm;

    out() << '\n' << fillformat('=', 79) << '\n';
    out().changeColor(raw_ostream::WHITE, true);
    out() << "ELF NOBITS: " << this->section_header->getName() << '\n';
    out().resetColor();
    out() << fillformat('-', 79) << '\n';

    out() << "  Size         : " << this->size() << '\n';
    out() << "  Start Address: "
          << static_cast<addr_t>((size_t)this->buf) << '\n';
    out() << fillformat('-', 79) << '\n';

    dump_hex(this->buf, this->buf_size, 0, 16);

    out() << fillformat('=', 79) << '\n';
  }
};

#endif // ELF_SECTION_NOBITS_H
