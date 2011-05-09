#include <iostream>

#include <stdlib.h>

#include <boost/shared_ptr.hpp>

#include "elf_object.h"

using namespace std;
using namespace boost;

int main(int argc, char **argv) {
  shared_ptr<elf_object> ptr(elf_object::read(argv[0]));

  if (!ptr) {
    cerr << "ERROR: Unable to read ELF executable." << endl;
  }

  return EXIT_SUCCESS;
}
