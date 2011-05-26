#ifndef ELF_SECTION_STRTAB_H
#define ELF_SECTION_STRTAB_H

#include "ELFSection.h"

#include "utils/helper.h"
#include "utils/serialize.h"
#include "utils/raw_ostream.h"

#include <boost/shared_ptr.hpp>
#include <vector>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Format.h>

template <size_t Bitwidth> class ELFSectionHeader;

template <size_t Bitwidth>
class ELFSectionStrTab : public ELFSection<Bitwidth> {
private:
  ELFSectionHeader<Bitwidth> const*section_header;
  std::vector<char> buf;

private:
  ELFSectionStrTab() { }

public:
  template <typename Archiver>
  static boost::shared_ptr<ELFSectionStrTab>
  read(Archiver &AR, ELFSectionHeader<Bitwidth> const *sh) {
    using namespace boost;

    shared_ptr<ELFSectionStrTab> st(new ELFSectionStrTab());
    st->buf.resize(sh->getSize());

    // Save section_header
    st->section_header = sh;

    AR.seek(sh->getOffset(), true);
    AR.prologue(sh->getSize());
    AR.read_bytes(&*st->buf.begin(), sh->getSize());
    AR.epilogue(sh->getSize());

    if (!AR) {
      // Unable to read the string table.
      return shared_ptr<ELFSectionStrTab>();
    }

    return st;
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
