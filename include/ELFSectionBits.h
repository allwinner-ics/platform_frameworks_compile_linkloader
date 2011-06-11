#ifndef ELF_SECTION_BITS_H
#define ELF_SECTION_BITS_H

#include "ELFSection.h"

#include <llvm/ADT/OwningPtr.h>

template <unsigned Bitwidth> class ELFSectionHeader;

template <unsigned Bitwidth>
class ELFSectionBits : public ELFSection<Bitwidth> {
protected:
  ELFSectionHeader<Bitwidth> const *section_header;
  unsigned char *buf;
  size_t buf_size;

protected:
  ELFSectionBits() : buf(NULL), buf_size(0) { }

  ~ELFSectionBits();

public:
  template <typename Archiver, typename ConcreteELFSectionBits>
  static ConcreteELFSectionBits *
  read(Archiver &AR,
       ELFSectionHeader<Bitwidth> const *sh,
       ConcreteELFSectionBits *concrete);

  virtual void print() const {}

  unsigned char &operator[](size_t index) {
    return buf[index];
  }

  unsigned char const &operator[](size_t index) const {
    return buf[index];
  }

  size_t size() const {
    return buf_size;
  }

  unsigned char const *memory_protect() const;

};

#include "impl/ELFSectionBits.hxx"

#endif // ELF_SECTION_BITS_H
