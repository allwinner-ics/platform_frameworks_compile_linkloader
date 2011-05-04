#include "elf_ident.h"

#include "term.h"

#include <iostream>
#include <iomanip>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <elf.h>
#include <string.h>

#if !defined(ELFOSABI_OPENVMS)
#define ELFOSABI_OPENVMS 13
#endif

#if !defined(ELFOSABI_NSK)
#define ELFOSABI_NSK 14
#endif

#if !defined(ELFOSABI_AROS)
#define ELFOSABI_AROS 15
#endif

#if !defined(ELFOSABI_FENIXOS)
#define ELFOSABI_FENIXOS 16
#endif

using namespace boost;
using namespace std;

shared_ptr<elf_ident> elf_ident::create(unsigned char const *file,
                                        size_t size,
                                        size_t offset) {
  if (size <= offset + EI_NIDENT) {
    throw runtime_error("ELF identification truncated");
  }

  return shared_ptr<elf_ident>(new elf_ident(file + offset));
}

elf_ident::elf_ident(unsigned char const *buf) {
  // Make a copy of ELF identification
  memcpy(ident, buf, EI_NIDENT);

  // Some assertions
  assert_valid_magic_word();
  assert_valid_class();
  assert_valid_endianness();
  assert_valid_header_version();
  assert_zeroed_padding();
}

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

void elf_ident::assert_valid_magic_word() const {
  if (memcmp(ident, "\x7f" "ELF", 4) != 0) {
    throw runtime_error("Bad ELF magic word");
  }
}

void elf_ident::assert_valid_class() const {
  if (!is_32bit() && !is_64bit()) {
    throw runtime_error("Unknown or invalid class");
  }
}

void elf_ident::assert_valid_endianness() const {
  if (!is_big_endian() && !is_little_endian()) {
    throw runtime_error("Unknown or invalid endianness");
  }
}

void elf_ident::assert_valid_header_version() const {
  if (get_header_version() != EV_CURRENT) {
    throw runtime_error("Unknown or invalid ELF header version");
  }
}

void elf_ident::assert_zeroed_padding() const {
  for (size_t i = EI_PAD; i < EI_NIDENT; ++i) {
    if (ident[i] != 0) {
      throw runtime_error("Padding should be zeroed out");
    }
  }
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
  CASE_PAIR(ELFOSABI_OPENVMS, "OpenVMS")
  CASE_PAIR(ELFOSABI_NSK, "NSK")
  CASE_PAIR(ELFOSABI_AROS, "AmigaOS")
  CASE_PAIR(ELFOSABI_FENIXOS, "FenixOS")
#undef CASE_PAIR
  }
}
