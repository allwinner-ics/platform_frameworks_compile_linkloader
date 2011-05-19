#ifndef ELF_OBJECT_H
#define ELF_OBJECT_H

#include <boost/shared_ptr.hpp>
#include <vector>

template <size_t Bitwidth> class ELFHeader;
template <size_t Bitwidth> class ELFSection;
template <size_t Bitwidth> class ELFSectionHeaderTable;

template <size_t Bitwidth>
class ELFObject {
private:
  boost::shared_ptr<ELFHeader<Bitwidth> > header;
  boost::shared_ptr<ELFSectionHeaderTable<Bitwidth> > shtab;
  std::vector<boost::shared_ptr<ELFSection<Bitwidth> > > stab;

private:
  ELFObject() { }

public:
  template <typename Archiver>
  static boost::shared_ptr<ELFObject> read(Archiver &AR);

  ELFHeader<Bitwidth> const *getHeader() const {
    return header.get();
  }

  void print() const;
};

#include "ELFHeader.h"
#include "ELFSection.h"
#include "ELFSectionHeaderTable.h"

template <size_t Bitwidth>
template <typename Archiver>
inline boost::shared_ptr<ELFObject<Bitwidth> >
ELFObject<Bitwidth>::read(Archiver &AR) {
  using namespace boost;

  shared_ptr<ELFObject<Bitwidth> > object(new ELFObject<Bitwidth>());

  // Read header
  object->header = ELFHeader<Bitwidth>::read(AR);
  if (!object->header) {
    return shared_ptr<ELFObject<Bitwidth> >();
  }

  // Read section table
  object->shtab = ELFSectionHeaderTable<Bitwidth>::read(AR, object.get());
  if (!object->shtab) {
    return shared_ptr<ELFObject<Bitwidth> >();
  }

  // Read each section
  for (size_t i = 0; i < object->header->getSectionHeaderNum(); ++i) {
    shared_ptr<ELFSection<Bitwidth> > sec(
      ELFSection<Bitwidth>::read(AR, (*object->shtab)[i]));
    object->stab.push_back(sec);
  }

  return object;
}

template <size_t Bitwidth>
inline void ELFObject<Bitwidth>::print() const {
  header->print();
  shtab->print();

  for (size_t i = 0; i < stab.size(); ++i) {
    ELFSection<Bitwidth> *sec = stab[i].get();
    if (sec) {
      sec->print();
    }
  }
}

#endif // ELF_OBJECT_H
