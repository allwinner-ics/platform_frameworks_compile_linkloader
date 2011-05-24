#ifndef ELF_SECTION_RELA_H
#define ELF_SECTION_RELA_H

#include "ELFTypes.h"

#include "utils/term.h"

#include <boost/shared_ptr.hpp>
#include <iomanip>
#include <iostream>
#include <string>

#include <stdint.h>

template <size_t Bitwidth> class ELFObject;
template <size_t Bitwidth> class ELFSectionRela;
template <size_t Bitwidth> class ELFSectionRela_CRTP;


class ELFSectionRelaHelperMixin {
protected:
//  static char const *getTypeStr(uint8_t);
//  static char const *getBindingAttributeStr(uint8_t);
//  static char const *getVisibilityStr(uint8_t);
};


template <size_t Bitwidth>
class ELFSectionRela : public ELFSectionRel<Bitwidth> {
public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

protected:
  sxword_t r_addend;

private:
  ELFSectionRela() { }

public:
  sxword_t getAddend() const {
    return r_addend;
  }

  bool isValid() const {
    // FIXME: Should check the correctness of the section header.
    return true;
  }

  template <typename Archiver>
  static boost::shared_ptr<ELFSectionRela>
  read(Archiver &AR, ELFObject<Bitwidth> const *owner, size_t index = 0);

  void print(bool shouldPrintHeader = false) const;

private:
  template <typename Archiver>
  bool serialize(Archiver &AR) {
    AR.prologue(TypeTraits<ELFSectionRela>::size);

    AR & this->r_offset;
    AR & this->r_info;
    AR & this->r_addend;

    AR.epilogue(TypeTraits<ELFSectionRela>::size);
    return AR;
  }

};

//==================Inline Member Function Definition==========================

template <size_t Bitwidth>
template <typename Archiver>
inline boost::shared_ptr<ELFSectionRela<Bitwidth> >
ELFSectionRela<Bitwidth>::read(Archiver &AR,
                               ELFObject<Bitwidth> const *owner,
                               size_t index) {
  if (!AR) {
    // Archiver is in bad state before calling read function.
    // Return NULL and do nothing.
    return boost::shared_ptr<ELFSectionRela>();
  }

  boost::shared_ptr<ELFSectionRela> sh(
    new ELFSectionRela());

  if (!sh->serialize(AR)) {
    // Unable to read the structure.  Return NULL.
    return boost::shared_ptr<ELFSectionRela>();
  }

  if (!sh->isValid()) {
    // Rela read from archiver is not valid.  Return NULL.
    return boost::shared_ptr<ELFSectionRela>();
  }

  // Set the section header index
  sh->index = index;

  // Set the owner elf object
  sh->owner = owner;

  return sh;
}

template <size_t Bitwidth>
inline void ELFSectionRela<Bitwidth>::
  print(bool shouldPrintHeader) const {
  using namespace term::color;
  using namespace std;

  std::ios_base::fmtflags prev_flags = cout.flags();

  if (shouldPrintHeader) {
    cout << endl << setw(79) << setfill('=') << '=' << setfill(' ') << endl;
    cout << light::white()
         << "ELF Relaocation Table(with Addend) Entry "
         << this->getIndex() << normal() << endl;
    cout << setw(79) << setfill('-') << '-' << setfill(' ') << endl;
  } else {
    cout << setw(79) << setfill('-') << '-' << setfill(' ') << endl;
    cout << light::yellow()
         << "ELF Relaocation Table(with Addend) Entry "
         << this->getIndex() << " : "
         << normal() << endl;
  }

  cout << setw(79) << setfill('-') << '-' << endl << setfill(' ');
#define PRINT_LINT(title, value) \
  cout << left << "  " << setw(13) \
       << (title) << " : " << right << (value) << endl
  PRINT_LINT("Offset",       this->getOffset()       );
  PRINT_LINT("SymTab Index", this->getSymTabIndex()  );
  PRINT_LINT("Type",         this->getType()         );
  PRINT_LINT("Addend",       this->getAddend()       );
#undef PRINT_LINT

  cout.flags( prev_flags );
}

#endif // ELF_SECTION_RELA_H
