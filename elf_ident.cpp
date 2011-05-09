#include "elf_ident.h"

#include "utils/term.h"
#include "utils/serialize.h"

#include <iostream>
#include <iomanip>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <elf.h>
#include <string.h>

using namespace boost;
using namespace serialization;
using namespace std;

elf_ident::elf_ident() {
  memset(ident, '\0', sizeof(ident));
}

template <typename Archiver>
shared_ptr<elf_ident> elf_ident::read(Archiver &AR) {
  shared_ptr<elf_ident> result(new elf_ident());

  // Read the ELF identifier from the archive
  result->serialize(AR);
  if (!AR) {
    return shared_ptr<elf_ident>();
  }

  // Check the correctness of the ELF identifier
  if (!result->is_valid_elf_ident()) {
    return shared_ptr<elf_ident>();
  }

  return result;
}

template shared_ptr<elf_ident> elf_ident::read(archive_reader_le &);
template shared_ptr<elf_ident> elf_ident::read(archive_reader_be &);

int elf_ident::get_class() const {
  return ident[EI_CLASS];
}

int elf_ident::get_endianness() const {
  return ident[EI_DATA];
}

int elf_ident::get_header_version() const {
  return ident[EI_VERSION];
}

int elf_ident::get_os_abi() const {
  return ident[EI_OSABI];
}

int elf_ident::get_abi_version() const {
  return ident[EI_ABIVERSION];
}

bool elf_ident::is_32bit() const {
  return ident[EI_CLASS] == ELFCLASS32;
}

bool elf_ident::is_64bit() const {
  return ident[EI_CLASS] == ELFCLASS64;
}

bool elf_ident::is_big_endian() const {
  return ident[EI_CLASS] == ELFDATA2MSB;
}

bool elf_ident::is_little_endian() const {
  return ident[EI_CLASS] == ELFDATA2LSB;
}

void elf_ident::print() const {
  using namespace term::color;

  cout << endl << setw(79) << setfill('=') << '=' << endl;

  cout << light::white() << "ELF Identification" << normal() << endl;

  cout << setw(79) << setfill('-') << '-' << endl << setfill(' ');

  cout << setw(20) << "Class" << " : " << get_class_name(get_class()) << endl;

  cout << setw(20) << "Endianness" << " : "
       << get_endianness_name(get_endianness()) << endl;

  cout << setw(20) << "Header Version" << " : " << get_header_version() << endl;

  cout << setw(20) << "OS ABI" << " : "
       << get_os_abi_name(get_os_abi()) << endl;

  cout << setw(20) << "ABI Version" << " : " << get_abi_version() << endl;

  cout << setw(79) << setfill('=') << '=' << endl << endl;
}

inline bool elf_ident::is_valid_magic_word() const {
  return (memcmp(ident, "\x7f" "ELF", 4) == 0);
}

inline bool elf_ident::is_valid_class() const {
  return (is_32bit() || is_64bit());
}

inline bool elf_ident::is_valid_endianness() const {
  return (is_big_endian() || is_little_endian());
}

inline bool elf_ident::is_valid_header_version() const {
  return (get_header_version() == EV_CURRENT);
}

inline bool elf_ident::is_unused_zeroed_padding() const {
  for (size_t i = EI_PAD; i < EI_NIDENT; ++i) {
    if (ident[i] != 0) {
      return false;
    }
  }
  return true;
}

bool elf_ident::is_valid_elf_ident() const {
  return (is_valid_magic_word() &&
          is_valid_class() &&
          is_valid_endianness() &&
          is_valid_header_version() &&
          is_unused_zeroed_padding());
}

char const *elf_ident::get_class_name(int clazz) {
  switch (clazz) {
  default:
#define CASE_PAIR(A, B) case A: return B;
  CASE_PAIR(ELFCLASSNONE, "Invalid class")
  CASE_PAIR(ELFCLASS32, "32bit")
  CASE_PAIR(ELFCLASS64, "64bit")
#undef CASE_PAIR
  }
}

char const *elf_ident::get_endianness_name(int endianness) {
  switch (endianness) {
  default:
#define CASE_PAIR(A, B) case A: return B;
  CASE_PAIR(ELFDATANONE, "Invalid endianness")
  CASE_PAIR(ELFDATA2LSB, "Little endian")
  CASE_PAIR(ELFDATA2MSB, "Big endian")
#undef CASE_PAIR
  }
}

char const *elf_ident::get_os_abi_name(int abi) {
  if (abi >= 64 && abi <= 255) {
    return "Architecture specific";
  }

  switch (abi) {
  default: return "Unknown OS ABI";
#define CASE_PAIR(A, B) case A: return B;
  CASE_PAIR(ELFOSABI_NONE, "No extensions or not specified")
  CASE_PAIR(ELFOSABI_HPUX, "HP-UX")
  CASE_PAIR(ELFOSABI_NETBSD, "NetBSD")
  CASE_PAIR(ELFOSABI_LINUX, "Linux")
  CASE_PAIR(ELFOSABI_SOLARIS, "Solaris")
  CASE_PAIR(ELFOSABI_AIX, "AIX")
  CASE_PAIR(ELFOSABI_FREEBSD, "FreeBSD")
  CASE_PAIR(ELFOSABI_TRU64, "Tru64")
  CASE_PAIR(ELFOSABI_MODESTO, "Modesto")
  CASE_PAIR(ELFOSABI_OPENBSD, "OpenBSD")
#undef CASE_PAIR
  }
}
