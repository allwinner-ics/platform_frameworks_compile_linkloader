#ifndef ELF_SECTION_BITS_HXX
#define ELF_SECTION_BITS_HXX

#include "utils/flush_cpu_cache.h"
#include "utils/helper.h"

#include <llvm/Support/raw_ostream.h>

#include <sys/mman.h>

#ifndef MAP_32BIT
#define MAP_32BIT 0
// Note: If the <sys/mman.h> does not come with MAP_32BIT, then we
// define it as zero, so that it won't manipulate the flags.
#endif


template <unsigned Bitwidth>
ELFSectionBits<Bitwidth>::~ELFSectionBits() {
  if (buf) {
    munmap(buf, buf_size);
  }
}

template <unsigned Bitwidth>
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

template <unsigned Bitwidth>
inline unsigned char const *
ELFSectionBits<Bitwidth>::memory_protect() const {
  int protect_type = PROT_READ;

  if (section_header->getFlags() & SHF_WRITE) {
    protect_type |= PROT_WRITE;
  }

  if (section_header->getFlags() & SHF_EXECINSTR) {
    protect_type |= PROT_EXEC;
  }

  if (buf_size > 0) {
    int ret = mprotect((void *)buf, buf_size, protect_type);
    if (ret == -1) {
      llvm::errs() << "Error: Can't mprotect.\n";
      return 0;
    }

    if (protect_type & PROT_EXEC) {
      FLUSH_CPU_CACHE(buf, buf + buf_size);
    }
  }

  return buf;
}

#endif // ELF_SECTION_BITS_HXX
