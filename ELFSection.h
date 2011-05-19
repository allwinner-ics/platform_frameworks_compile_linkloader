#ifndef ELF_SECTION_H
#define ELF_SECTION_H

#include "utils/serialize.h"

#include <boost/shared_ptr.hpp>
#include <iostream>

template <size_t Bitwidth> class ELFSectionHeader;

template <size_t Bitwidth>
class ELFSection {
protected:
  ELFSection() { }

public:
  virtual ~ELFSection() { }

  virtual void print() const = 0;

  template <typename Archiver>
  static boost::shared_ptr<ELFSection>
  read(Archiver &AR, ELFSectionHeader<Bitwidth> const *);
};

#include "ELFSectionHeader.h"
#include "ELFSectionStrTab.h"

template <size_t Bitwidth>
template <typename Archiver>
inline boost::shared_ptr<ELFSection<Bitwidth> >
ELFSection<Bitwidth>::read(Archiver &AR,
                           ELFSectionHeader<Bitwidth> const *sh) {
  using namespace boost;
  using namespace std;

  switch ((uint32_t)sh->getType()) {
    default:
      // Uknown type of ELF section.  Return NULL.
      cerr << "WARNING: Unknown section type." << endl;
      return shared_ptr<ELFSection>();

    case SHT_STRTAB:
      return ELFSectionStrTab<Bitwidth>::read(AR, sh);

    case SHT_SYMTAB:
    case SHT_PROGBITS:
    case SHT_NOBITS:
    case SHT_REL:
    case SHT_RELA:
      // TODO: Not Yet Implemented
      return shared_ptr<ELFSection>();
  };
}


#endif // ELF_SECTION_H
