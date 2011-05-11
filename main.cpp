#include <iostream>

#include <stdlib.h>

#include <boost/shared_ptr.hpp>

#include "elf_object.h"

using namespace std;
using namespace boost;

int main(int argc, char **argv) {
  string filename;

  if (argc >= 2) {
    filename = argv[1];
  } else {
    filename = argv[0];
  }

  shared_ptr<elf_object> ptr(elf_object::read(filename));

  if (!ptr) {
    cerr << "ERROR: Unable to read ELF executable." << endl;
  }else{
	  ptr->print();
  }

  return EXIT_SUCCESS;
}
