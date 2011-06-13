#include "MemChunk.h"

#include "utils/flush_cpu_cache.h"
#include "utils/helper.h"

#include <llvm/Support/raw_ostream.h>

#include <sys/mman.h>

#include <stdlib.h>

#ifndef MAP_32BIT
#define MAP_32BIT 0
// Note: If the <sys/mman.h> does not come with MAP_32BIT, then we
// define it as zero, so that it won't manipulate the flags.
#endif

static uintptr_t StartAddr = 0x7e000000UL;

MemChunk::MemChunk() : buf((unsigned char *)MAP_FAILED), buf_size(0) {
}

MemChunk::~MemChunk() {
  if (buf != MAP_FAILED) {
    munmap(buf, buf_size);
  }
}

bool MemChunk::allocate(size_t size) {
  buf = (unsigned char *)mmap((void *)StartAddr, size, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT,
                              -1, 0);

  if (buf == MAP_FAILED) {
    return false;
  }

  StartAddr += (size + 4095) / 4096 * 4096;

  buf_size = size;
  return true;
}

void MemChunk::print() const {
  if (buf != MAP_FAILED) {
    dump_hex(buf, buf_size, 0, 0);
  }
}

bool MemChunk::protect(int prot) {
  if (buf_size > 0) {
    int ret = mprotect((void *)buf, buf_size, prot);
    if (ret == -1) {
      llvm::errs() << "Error: Can't mprotect.\n";
      return false;
    }

    if (prot & PROT_EXEC) {
      FLUSH_CPU_CACHE(buf, buf + buf_size);
    }
  }

  return true;
}
