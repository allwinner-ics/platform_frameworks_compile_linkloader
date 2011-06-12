#ifndef ELF_SECTION_BITS_HXX
#define ELF_SECTION_BITS_HXX

#include "utils/flush_cpu_cache.h"
#include "utils/helper.h"

#include <llvm/Support/raw_ostream.h>

#include <sys/mman.h>

template <unsigned Bitwidth>
inline void ELFSectionBits<Bitwidth>::print() const {
  using namespace llvm;

  char const *section_type_str =
    (sh->getType() == SHT_NOBITS) ? "NOBITS" : "PROGBITS";

  out() << '\n' << fillformat('=', 79) << '\n';
  out().changeColor(raw_ostream::WHITE, true);
  out() << "ELF " << section_type_str << ": " << sh->getName() << '\n';
  out().resetColor();
  out() << fillformat('-', 79) << '\n';

  out() << "  Size         : " << sh->getSize() << '\n';
  out() << "  Start Address: " << (void *)chunk.getBuffer() << '\n';
  out() << fillformat('-', 79) << '\n';

  chunk.print();

  out() << fillformat('=', 79) << '\n';
}

template <unsigned Bitwidth>
inline bool ELFSectionBits<Bitwidth>::protect() {
  int prot = PROT_READ;

  if (sh->getFlags() & SHF_WRITE) {
    prot |= PROT_WRITE;
  }

  if (sh->getFlags() & SHF_EXECINSTR) {
    prot |= PROT_EXEC;
  }

  return chunk.protect(prot);
}

#endif // ELF_SECTION_BITS_HXX
