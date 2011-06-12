#ifndef STUB_LAYOUT_H
#define STUB_LAYOUT_H

#include <stdlib.h>

class StubLayout {
private:
  unsigned char *table;
  size_t count;

public:
  StubLayout();

  void initStubTable(unsigned char *table, size_t count);
  void *allocateStub(void *addr = 0);

  size_t calcStubTableSize(size_t count) const;

private:
  void setStubAddress(void *stub, void *addr);

};

#endif // STUB_LAYOUT_H
