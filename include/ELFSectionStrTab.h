#ifndef ELF_SECTION_STRTAB_H
#define ELF_SECTION_STRTAB_H

#include "ELFSection.h"

#include "utils/helper.h"
#include "utils/serialize.h"
#include "utils/raw_ostream.h"

#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/Format.h>
#include <llvm/Support/raw_ostream.h>

#include <vector>

template <unsigned Bitwidth> class ELFSectionHeader;

template <unsigned Bitwidth>
class ELFSectionStrTab : public ELFSection<Bitwidth> {
private:
  ELFSectionHeader<Bitwidth> const *section_header;
  std::vector<char> buf;

private:
  ELFSectionStrTab() { }

public:
  template <typename Archiver>
  static ELFSectionStrTab *
  read(Archiver &AR, ELFSectionHeader<Bitwidth> const *sh) {
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

  virtual void print() const {
    using namespace llvm;

    out() << '\n' << fillformat('=', 79) << '\n';
    out().changeColor(raw_ostream::WHITE, true);
    out() << "ELF String Table: " << this->section_header->getName() << '\n';
    out().resetColor();
    out() << fillformat('-', 79) << '\n';

    dump_hex((unsigned char const *)&*buf.begin(), buf.size(), 0, buf.size());

    out() << fillformat('=', 79) << '\n';
  }

  char const *operator[](size_t index) const {
    return &*buf.begin() + index;
  }
};

#endif // ELF_SECTION_STRTAB_H
