#include <stdio.h>
#include <stdlib.h>

#include "../StubLayout.h"

void function1() {
  printf("hello ");
}

void function2() {
  printf("world!\n");
}

int main() {
  StubLayout stubs;

  void (*func1)() = (void (*)())stubs.allocateStub((void *)&function1);
  void (*func2)() = (void (*)())stubs.allocateStub((void *)&function2);

  if (!func1) {
    fprintf(stderr, "ERROR: Unable to allocate stub for function1\n");
    exit(EXIT_FAILURE);
  }

  if (!func2) {
    fprintf(stderr, "ERROR: Unable to allocate stub for function2\n");
    exit(EXIT_FAILURE);
  }

  function1();
  function2();

  func1();
  func2();

  return EXIT_SUCCESS;
}
