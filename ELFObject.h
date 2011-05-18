#ifndef ELF_OBJECT_H
#define ELF_OBJECT_H

#include "ELFHeader.h"

#include <boost/shared_ptr.hpp>

template <size_t Bitwidth>
class ELFObject {
private:
  boost::shared_ptr<ELFHeader<Bitwidth> > header;

private:
  ELFObject() { }

public:
  template <typename Archiver>
  static boost::shared_ptr<ELFObject> read(Archiver &AR) {
    boost::shared_ptr<ELFObject> object(new ELFObject());

    object->header = ELFHeader<Bitwidth>::read(AR);
    if (!object->header) {
      return boost::shared_ptr<ELFObject>();
    }

    return object;
  }

  void print() {
    header->print();
  }
};

#endif // ELF_OBJECT_H
