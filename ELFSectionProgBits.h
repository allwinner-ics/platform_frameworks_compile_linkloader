#include "ELFTypes.h"
#include "ELFSectionBits.h"
#include "ELFSectionHeader.h"
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Format.h>

#include "utils/raw_ostream.h"


template <size_t Bitwidth>
class ELFSectionProgBits :
  public ELFSectionBits<Bitwidth, ELFSectionProgBits<Bitwidth> > {
    friend class ELFSectionBits<Bitwidth, ELFSectionProgBits<Bitwidth> >;
private:
  template <typename Archiver>
  bool serialize(Archiver &AR) {
    AR.seek(this->section_header->getOffset(), true);
    AR.prologue(this->section_header->getSize());
    AR.read_bytes(this->buf, this->section_header->getSize());
    AR.epilogue(this->section_header->getSize());
    return static_cast<bool>(AR);
  }

  void print() const {
    ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

    using namespace llvm;

    out() << '\n' << fillformat('=', 79) << '\n';
    out().changeColor(raw_ostream::WHITE, true);
    out() << "ELF PROGBITS: " << this->section_header->getName() << '\n';
    out().resetColor();
    out() << fillformat('-', 79) << '\n';

    out() << " Size         : " << this->size() << '\n';
    out() << " Start Address: "
          << static_cast<addr_t>((size_t)this->buf) << '\n';
    out() << fillformat('-', 79) << '\n';

    dump_hex(this->buf, this->buf_size, 0, this->buf_size);

    out() << fillformat('=', 79) << '\n';
  }
};

