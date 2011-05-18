#ifndef ELF_OBJECT_H
#define ELF_OBJECT_H

#include <boost/shared_ptr.hpp>

template <size_t Bitwidth> class ELFHeader;
template <size_t Bitwidth> class ELFSectionHeaderTable;

template <size_t Bitwidth>
class ELFObject {
private:
  boost::shared_ptr<ELFHeader<Bitwidth> > header;
  boost::shared_ptr<ELFSectionHeaderTable<Bitwidth> > shtab;

private:
  ELFObject() { }

public:
  template <typename Archiver>
  static boost::shared_ptr<ELFObject> read(Archiver &AR);

  ELFHeader<Bitwidth> const *getHeader() const {
    return header.get();
  }

  void print() {
    header->print();
    shtab->print();
  }
};

#include "ELFHeader.h"
#include "ELFSectionHeaderTable.h"

template <size_t Bitwidth>
template <typename Archiver>
inline boost::shared_ptr<ELFObject<Bitwidth> >
ELFObject<Bitwidth>::read(Archiver &AR) {
  using namespace boost;

  shared_ptr<ELFObject<Bitwidth> > object(new ELFObject<Bitwidth>());

  object->header = ELFHeader<Bitwidth>::read(AR);
  if (!object->header) {
    return shared_ptr<ELFObject<Bitwidth> >();
  }

  object->shtab = ELFSectionHeaderTable<Bitwidth>::read(AR, object.get());
  if (!object->shtab) {
    return shared_ptr<ELFObject<Bitwidth> >();
  }

  return object;
}

#endif // ELF_OBJECT_H
