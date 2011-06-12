#include "StubLayout.h"

#include "utils/flush_cpu_cache.h"
#include "utils/raw_ostream.h"

#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>

#ifdef __arm__
#define STUB_SIZE 8 // 8 bytes (2 words)
#endif

StubLayout::StubLayout() : table(NULL), count(0) {
}

void StubLayout::initStubTable(unsigned char *table_, size_t count_) {
  table = table_;
  count = count_;
}

void *StubLayout::allocateStub(void *addr) {
#ifdef __arm__
  if (count == 0) {
    // No free stub slot is available
    return NULL;
  }

  // Initialize the stub
  unsigned char *stub = table;
  setStubAddress(stub, addr);

  // Increase the free stub slot pointer
  table += STUB_SIZE;
  count--;

  return stub;

#else
  return NULL;
#endif
}

void StubLayout::setStubAddress(void *stub_, void *addr) {
#ifdef __arm__
  uint8_t *stub = (uint8_t *)stub_;
  stub[0] = 0x04; // ldr pc, [pc, #-4]
  stub[1] = 0xf0; // ldr pc, [pc, #-4]
  stub[2] = 0x1f; // ldr pc, [pc, #-4]
  stub[3] = 0xe5; // ldr pc, [pc, #-4]

  void **target = (void **)(stub + 4);
  *target = addr;
#endif
}

size_t StubLayout::calcStubTableSize(size_t count) const {
#ifdef __arm__
  return count * STUB_SIZE;
#else
  return 0;
#endif
}
