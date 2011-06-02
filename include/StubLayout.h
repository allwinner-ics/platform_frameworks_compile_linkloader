#ifndef STUB_LAYOUT_H
#define STUB_LAYOUT_H

#include <stdlib.h>

class StubLayout {
private:
  size_t count;
  unsigned char *table;

public:
  StubLayout();
  ~StubLayout();

  void *allocateStub(void *addr = 0);
  void updateStubAddress(void *stub, void *addr);

private:
  bool allocateTable();
  void setStubAddress(void *stub, void *addr);

};

#endif // STUB_LAYOUT_H
