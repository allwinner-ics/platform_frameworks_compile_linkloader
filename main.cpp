#include <iostream>
#include <stdexcept>

#include <stdlib.h>

#include "elf_reader.h"

using namespace std;

int main(int argc, char **argv) {
  if (argc < 2) {
    cerr << "USAGE: " << argv[0] << " [ELFObjectFile]" << endl;
    exit(EXIT_FAILURE);
  }

  try {
    elf_reader r(argv[1]);

  } catch (exception &e) {
    cerr << "ERROR: " << e.what() << endl;
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
