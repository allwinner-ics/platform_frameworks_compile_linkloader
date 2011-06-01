#ifndef ELF_SECTION_BITS_H
#define ELF_SECTION_BITS_H

/*
 * This file is bits section CRTP class, only for progbits and nobits.
 */

#include "ELFSection.h"

#include "utils/flush_cpu_cache.h"
#include "utils/helper.h"
#include "utils/serialize.h"

#include <sys/mman.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/raw_ostream.h>

#ifndef MAP_32BIT
#define MAP_32BIT 0
// Note: If the <sys/mman.h> does not come with MAP_32BIT, then we
// define it as zero, so that it won't manipulate the flags.
#endif

template <size_t Bitwidth> class ELFSectionHeader;

template <size_t Bitwidth>
class ELFSectionBits : public ELFSection<Bitwidth> {
protected:
  ELFSectionHeader<Bitwidth> const *section_header;
  unsigned char *buf;
  size_t buf_size;

protected:
  ELFSectionBits() : buf(NULL), buf_size(0) { }

  ~ELFSectionBits() {
    if (buf) {
      munmap(buf, buf_size);
    }
  }

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



//==================Inline Member Function Definition==========================


template <size_t Bitwidth>
template <typename Archiver, typename ConcreteELFSectionBits>
inline ConcreteELFSectionBits *
ELFSectionBits<Bitwidth>::
read(Archiver &AR,
     ELFSectionHeader<Bitwidth> const *sh,
     ConcreteELFSectionBits *concrete) {
  llvm::OwningPtr<ConcreteELFSectionBits> result(concrete);
  // TODO: Align.
  result->buf_size = sh->getSize();
  if (result->buf_size > 0) {
    result->buf = (unsigned char *)mmap(0, result->buf_size,
                                        PROT_READ | PROT_WRITE,
                                        MAP_PRIVATE |
                                        MAP_ANONYMOUS |
                                        MAP_32BIT, -1, 0);
  }

  // Check map success.
  if (result->buf == MAP_FAILED) {
    assert(0 && "MAP_FAILED");
    return 0;
  }

  // Save section_header
  result->section_header = sh;

  // Read the buffer from string tab
  if (!result->serialize(AR)) {
    // Unable to read the structure.  Return NULL.
    return 0;
  }

  if (!AR) {
    // Unable to read the progbits tab
    return 0;
  }

  return result.take();
}

template <size_t Bitwidth>
inline unsigned char const *
ELFSectionBits<Bitwidth>::memory_protect() const {
  int protect_type = PROT_READ;
  if (this->section_header->getFlags() & SHF_WRITE) {
    protect_type |= PROT_WRITE;
  }
  if (this->section_header->getFlags() &SHF_EXECINSTR) {
    protect_type |= PROT_EXEC;
  }
  if (buf_size > 0) {
    int ret = mprotect((void *)buf, buf_size, protect_type);
    if (ret == -1) {
      // FIXME: Throws excetion?
      llvm::errs()<<"Error: Can't mprotect.\n";
      return 0;
    }

    if (protect_type & PROT_EXEC) {
      FLUSH_CPU_CACHE(buf, buf + buf_size);
    }
  }
  return buf;
}

#endif // ELF_SECTION_BITS_H
