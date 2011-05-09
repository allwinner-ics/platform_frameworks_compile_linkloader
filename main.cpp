#include <iostream>
#include <stdexcept>

#include <stdlib.h>

#include <boost/shared_ptr.hpp>

#include "elf_object.h"

using namespace std;
using namespace boost;

int main(int argc, char **argv) {
  if (argc < 2) {
    cerr << "USAGE: " << argv[0] << " [ELFObjectFile]" << endl;
    exit(EXIT_FAILURE);
  }

  try {
    shared_ptr<elf_object> ptr(elf_object::read(argv[1]));

  } catch (runtime_error &e) {
    cerr << "ERROR: " << e.what() << endl;
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
