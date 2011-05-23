#ifndef ELF_SECTION_STRTAB_H
#define ELF_SECTION_STRTAB_H

#include "ELFSection.h"

#include "utils/helper.h"
#include "utils/serialize.h"
#include "utils/term.h"

#include <boost/shared_ptr.hpp>
#include <vector>

template <size_t Bitwidth> class ELFSectionHeader;

template <size_t Bitwidth>
class ELFSectionStrTab : public ELFSection<Bitwidth> {
private:
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
    using namespace std;
    using namespace term;
    using namespace term::color;

    std::ios_base::fmtflags prev_flags = cout.flags();

    cout << endl << setw(79) << setfill('=') << '=' << endl;

    cout << light::white() << "ELF String Table" << normal() << endl;

    cout << setw(79) << setfill('-') << '-' << endl << setfill(' ');

    dump_hex((unsigned char const *)&*buf.begin(), buf.size(), 0, buf.size());

    cout << setw(79) << setfill('=') << '=' << endl << endl;

    cout.flags( prev_flags );
  }

  char const *operator[](size_t index) const {
    return &*buf.begin() + index;
  }
};

#endif // ELF_SECTION_STRTAB_H
