#ifndef ELF_SECTION_PROGBITS_HXX
#define ELF_SECTION_PROGBITS_HXX

#include "ELFTypes.h"

#include <llvm/Support/Format.h>
#include <llvm/Support/raw_ostream.h>

#include "utils/raw_ostream.h"

template <unsigned Bitwidth>
void ELFSectionProgBits<Bitwidth>::print() const {
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

  using namespace llvm;

  out() << '\n' << fillformat('=', 79) << '\n';
  out().changeColor(raw_ostream::WHITE, true);
  out() << "ELF PROGBITS: " << this->section_header->getName() << '\n';
  out().resetColor();
  out() << fillformat('-', 79) << '\n';

  out() << "  Size         : " << this->size() << '\n';
  out() << "  Start Address: "
    << static_cast<addr_t>((size_t)this->buf) << '\n';
  out() << fillformat('-', 79) << '\n';

  dump_hex(this->buf, this->buf_size, 0, this->buf_size);

  out() << fillformat('=', 79) << '\n';
}

#endif // ELF_SECTION_PROGBITS_HXX
