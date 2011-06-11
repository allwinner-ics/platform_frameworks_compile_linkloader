#ifndef ELF_SECTION_STR_TAB_HXX
#define ELF_SECTION_STR_TAB_HXX

#include "utils/helper.h"
#include "utils/raw_ostream.h"

#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/Format.h>
#include <llvm/Support/raw_ostream.h>

template <unsigned Bitwidth>
template <typename Archiver>
ELFSectionStrTab<Bitwidth> *
ELFSectionStrTab<Bitwidth>::read(Archiver &AR,
                                 ELFSectionHeaderTy const *sh) {

  llvm::OwningPtr<ELFSectionStrTab> st(new ELFSectionStrTab());
  st->buf.resize(sh->getSize());

  // Save section_header
  st->section_header = sh;

  AR.seek(sh->getOffset(), true);
  AR.prologue(sh->getSize());
  AR.read_bytes(&*st->buf.begin(), sh->getSize());
  AR.epilogue(sh->getSize());

  if (!AR) {
    // Unable to read the string table.
    return 0;
  }

  return st.take();
}

template <unsigned Bitwidth>
void ELFSectionStrTab<Bitwidth>::print() const {
  using namespace llvm;

  out() << '\n' << fillformat('=', 79) << '\n';
  out().changeColor(raw_ostream::WHITE, true);
  out() << "ELF String Table: " << this->section_header->getName() << '\n';
  out().resetColor();
  out() << fillformat('-', 79) << '\n';

  dump_hex((unsigned char const *)&*buf.begin(), buf.size(), 0, buf.size());

  out() << fillformat('=', 79) << '\n';
}

#endif // ELF_SECTION_STR_TAB_HXX
