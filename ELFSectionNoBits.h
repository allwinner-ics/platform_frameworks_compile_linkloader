#include "ELFTypes.h"
#include "ELFSectionBits.h"
#include "ELFSectionHeader.h"


template <size_t Bitwidth>
class ELFSectionNoBits :
  public ELFSectionBits<Bitwidth, ELFSectionNoBits<Bitwidth> > {
    friend class ELFSectionBits<Bitwidth, ELFSectionNoBits<Bitwidth> >;
private:
  template <typename Archiver>
  bool serialize(Archiver &AR) {
    // Is mmap init all bytes to 0?
    return true;
  }

  void print() const {
    ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

    using namespace std;
    using namespace term;
    using namespace term::color;

    std::ios_base::fmtflags prev_flags = cout.flags();

    cout << endl << setw(79) << setfill('=') << '=' << endl;

    cout << light::white() << "NOGBITS:" <<
            this->section_header->getName() << normal() << endl;

    cout << setw(79) << setfill('-') << '-' << endl << setfill(' ');

    cout << "  Size         : " << right << this->size() << endl;
    cout << "  Start Address: " << right
         << static_cast<addr_t>((size_t)this->buf) << endl;

    dump_hex(this->buf, this->buf_size, 0, 16);

    cout << endl << setw(79) << setfill('=') << '=' << endl;

    cout.flags( prev_flags );
  }
};

