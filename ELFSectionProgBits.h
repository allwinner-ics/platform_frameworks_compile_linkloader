#include "ELFTypes.h"
#include "ELFSectionBits.h"
#include "ELFSectionHeader.h"


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

    using namespace std;
    using namespace term;
    using namespace term::color;

    std::ios_base::fmtflags prev_flags = cout.flags();

    cout << endl << setw(79) << setfill('=') << '=' << endl;

    cout << light::white() << "PROGBITS:" <<
            this->section_header->getName() << normal() << endl;

    cout << setw(79) << setfill('-') << '-' << endl << setfill(' ');

    cout << "Size         : " << right << this->size() << endl;
    cout << "Start Address: " << right
         << static_cast<addr_t>((size_t)this->buf) << endl;

    dump_hex(this->buf, this->buf_size, 0, this->buf_size);

    cout << endl << setw(79) << setfill('=') << '=' << endl;

    cout.flags( prev_flags );
  }
};

