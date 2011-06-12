#ifndef ELF_SECTION_BITS_H
#define ELF_SECTION_BITS_H

#include "ELFTypes.h"
#include "ELFSection.h"
#include "MemChunk.h"

#include <llvm/ADT/OwningPtr.h>

template <unsigned Bitwidth>
class ELFSectionBits : public ELFSection<Bitwidth> {
protected:
  ELFSectionHeader<Bitwidth> const *sh;
  MemChunk chunk;

protected:
  ELFSectionBits() : sh(NULL) { }

public:
  virtual void print() const;

  bool protect();

  unsigned char const *getBuffer() const {
    return chunk.getBuffer();
  }

  unsigned char *getBuffer() {
    return chunk.getBuffer();
  }

  unsigned char &operator[](size_t index) {
    return chunk[index];
  }

  unsigned char const &operator[](size_t index) const {
    return chunk[index];
  }

  size_t size() const {
    return chunk.size();
  }

};

#include "impl/ELFSectionBits.hxx"

#endif // ELF_SECTION_BITS_H
