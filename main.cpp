#include "ELFObject.h"

#include "utils/serialize.h"

#include <llvm/ADT/OwningPtr.h>

#include <elf.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <map>
#include <stdio.h>

using namespace serialization;
using namespace std;

bool open_mmap_file(char const *filename,
                    int &fd,
                    unsigned char const *&image,
                    size_t &size);

void close_mmap_file(int fd,
                     unsigned char const *image,
                     size_t size);

void dump_and_run_file(unsigned char const *image, size_t size,
                       int argc, char **argv);

int main(int argc, char **argv) {
  // Check arguments
  if (argc < 2) {
    llvm::errs() << "USAGE: " << argv[0] << " [ELFObjectFile] [ARGS]\n";
    exit(EXIT_FAILURE);
  }

  // Filename from argument
  char const *filename = argv[1];

  // Open the file
  int fd = -1;
  unsigned char const *image = NULL;
  size_t image_size = 0;

  if (!open_mmap_file(filename, fd, image, image_size)) {
    exit(EXIT_FAILURE);
  }

  // Dump and run the file
  dump_and_run_file(image, image_size, argc - 1, argv + 1);

  // Close the file
  close_mmap_file(fd, image, image_size);

  return EXIT_SUCCESS;
}

void *find_sym(char const *name_, void *context) {
  std::string name = name_;
  std::map<std::string, void *> fptr;

#define DEF_FUNC(FUNC) \
  fptr.insert(make_pair(#FUNC, (void *)&FUNC));

  DEF_FUNC(rand);
  DEF_FUNC(printf);
  DEF_FUNC(scanf);
  DEF_FUNC(srand);
  DEF_FUNC(time);

#undef DEF_FUNC

  fptr.insert(make_pair("__isoc99_scanf", (void*)scanf));

  if (fptr.count(name) > 0) {
    return fptr[name];
  }
  assert(0 && "Can't find symbol.");
  return 0;
}

template <size_t Bitwidth, typename Archiver>
void dump_and_run_object(Archiver &AR, int argc, char **argv) {
  llvm::OwningPtr<ELFObject<Bitwidth> > object(ELFObject<Bitwidth>::read(AR));

  if (!object) {
    llvm::errs() << "ERROR: Unable to load object\n";
  }

  object->print();
  out().flush();

  ELFSectionSymTab<Bitwidth> *symtab =
    static_cast<ELFSectionSymTab<Bitwidth> *>(
        object->getSectionByName(".symtab"));

  object->relocate(find_sym, 0);

  void *main_addr = symtab->getByName("main")->getAddress();
  out() << "main address: " << main_addr << "\n";
  out().flush();

  ((int (*)(int, char **))main_addr)(argc, argv);
}

template <typename Archiver>
void dump_and_run_file_from_archive(bool is32bit, Archiver &AR,
                                    int argc, char **argv) {
  if (is32bit) {
    dump_and_run_object<32>(AR, argc, argv);
  } else {
    dump_and_run_object<64>(AR, argc, argv);
  }
}

void dump_and_run_file(unsigned char const *image, size_t size,
                       int argc, char **argv) {
  if (size < EI_NIDENT) {
    llvm::errs() << "ERROR: ELF identification corrupted.\n";
    return;
  }

  if (image[EI_DATA] != ELFDATA2LSB && image[EI_DATA] != ELFDATA2MSB) {
    llvm::errs() << "ERROR: Unknown endianness.\n";
    return;
  }

  if (image[EI_CLASS] != ELFCLASS32 && image[EI_CLASS] != ELFCLASS64) {
    llvm::errs() << "ERROR: Unknown machine class.\n";
    return;
  }

  bool isLittleEndian = (image[EI_DATA] == ELFDATA2LSB);
  bool is32bit = (image[EI_CLASS] == ELFCLASS32);

  if (isLittleEndian) {
    archive_reader_le AR(image, size);
    dump_and_run_file_from_archive(is32bit, AR, argc, argv);
  } else {
    archive_reader_be AR(image, size);
    dump_and_run_file_from_archive(is32bit, AR, argc, argv);
  }
}

bool open_mmap_file(char const *filename,
                    int &fd,
                    unsigned char const *&image,
                    size_t &size) {
  // Query the file status
  struct stat sb;
  if (stat(filename, &sb) != 0) {
    llvm::errs() << "ERROR: " << filename << " not found.\n";
    return false;
  }

  if (!S_ISREG(sb.st_mode)) {
    llvm::errs() << "ERROR: " << filename << " is not a regular file.\n";
    return false;
  }

  size = (size_t)sb.st_size;

  // Open the file in readonly mode
  fd = open(filename, O_RDONLY);
  if (fd < 0) {
    llvm::errs() << "ERROR: Unable to open " << filename << "\n";
    return false;
  }

  // Map the file image
  image = static_cast<unsigned char const *>(
    mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0));

  if (image == MAP_FAILED) {
    llvm::errs() << "ERROR: Unable to map " << filename << " to memory.\n";
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
