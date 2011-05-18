#include "ELFObject.h"

#include "utils/serialize.h"

#include <boost/shared_ptr.hpp>
#include <iostream>

#include <elf.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace boost;
using namespace serialization;
using namespace std;

bool open_mmap_file(char const *filename,
                    int &fd,
                    unsigned char const *&image,
                    size_t &size);

void close_mmap_file(int fd,
                     unsigned char const *image,
                     size_t size);

void dump_file(unsigned char const *image, size_t size);

int main(int argc, char **argv) {
  // Filename from argument
  char const *filename = argv[0];
  if (argc >= 2) {
    filename = argv[1];
  }

  // Open the file
  int fd = -1;
  unsigned char const *image = NULL;
  size_t image_size = 0;

  if (!open_mmap_file(filename, fd, image, image_size)) {
    cerr << "ERROR: Unable to open the file: " << filename << endl;
  }

  // Dump the file
  dump_file(image, image_size);

  // Close the file
  close_mmap_file(fd, image, image_size);

  return EXIT_SUCCESS;
}

template <size_t Bitwidth, typename Archiver>
void dump_object(Archiver &AR) {
  shared_ptr<ELFObject<Bitwidth> > object(ELFObject<Bitwidth>::read(AR));

  if (!object) {
    cerr << "ERROR: Unable to load object" << endl;
  }

  object->print();
}

template <typename Archiver>
void dump_file_from_archive(bool is32bit, Archiver &AR) {
  if (is32bit) {
    dump_object<32>(AR);
  } else {
    dump_object<64>(AR);
  }
}

void dump_file(unsigned char const *image, size_t size) {
  if (size < EI_NIDENT) {
    cerr << "ERROR: ELF identification corrupted." << endl;
    return;
  }

  if (image[EI_DATA] != ELFDATA2LSB && image[EI_DATA] != ELFDATA2MSB) {
    cerr << "ERROR: Unknown endianness." << endl;
    return;
  }

  if (image[EI_CLASS] != ELFCLASS32 && image[EI_CLASS] != ELFCLASS64) {
    cerr << "ERROR: Unknown machine class." << endl;
    return;
  }

  bool isLittleEndian = (image[EI_DATA] == ELFDATA2LSB);
  bool is32bit = (image[EI_DATA] == ELFCLASS32);

  if (isLittleEndian) {
    archive_reader_le AR(image, size);
    dump_file_from_archive(is32bit, AR);
  } else {
    archive_reader_be AR(image, size);
    dump_file_from_archive(is32bit, AR);
  }
}

bool open_mmap_file(char const *filename,
                    int &fd,
                    unsigned char const *&image,
                    size_t &size) {
  // Open the file in readonly mode
  fd = open(filename, O_RDONLY);
  if (fd < 0) {
    return false;
  }

  // Query the file size
  struct stat sb;
  if (fstat(fd, &sb) != 0) {
    close(fd);
    return false;
  }

  size = (size_t)sb.st_size;

  // Map the file image
  image = static_cast<unsigned char const *>(
    mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0));

  if (image == NULL || image == MAP_FAILED) {
    close(fd);
    return false;
  }

  return true;
}

void close_mmap_file(int fd,
                     unsigned char const *image,
                     size_t size) {
  if (image) {
    munmap((void *)image, size);
  }

  if (fd >= 0) {
    close(fd);
  }
}
