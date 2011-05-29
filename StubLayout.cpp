#include "StubLayout.h"

#include "utils/raw_ostream.h"

#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>

#ifdef __arm__
#define STUB_SIZE 16 // 16 bytes (4 words)
#define STUB_TABLE_COUNT 256
#define STUB_TABLE_SIZE (STUB_SIZE * STUB_TABLE_COUNT)
#endif

StubLayout::StubLayout() : count(0), table((unsigned char *)MAP_FAILED) {
}

StubLayout::~StubLayout() {
#ifdef __arm__
  if (table != MAP_FAILED) {
    munmap(table, STUB_TABLE_SIZE);
  }
#endif
}

void *StubLayout::allocateStub(void *addr) {
#ifdef __arm__
  if (table == MAP_FAILED && !allocateTable()) {
    return NULL;
  }

  if (count >= STUB_TABLE_COUNT) {
    return NULL;
  }

  void *stub = table + STUB_SIZE * count;
  count++;

  updateStubAddress(stub, addr);

  return stub;
#else
  return NULL;
#endif
}

void StubLayout::updateStubAddress(void *stub, void *addr) {
#ifdef __arm__
  mprotect(table, STUB_TABLE_SIZE, PROT_WRITE);
  setStubAddress(stub, addr);
  __clear_cache(table, table + STUB_TABLE_SIZE);
  mprotect(table, STUB_TABLE_SIZE, PROT_EXEC);
#endif
}

bool StubLayout::allocateTable() {
#ifdef __arm__
  table = (unsigned char *)mmap(0, STUB_TABLE_SIZE, PROT_READ | PROT_WRITE,
                                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  return table != MAP_FAILED;
#else
  return false;
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
