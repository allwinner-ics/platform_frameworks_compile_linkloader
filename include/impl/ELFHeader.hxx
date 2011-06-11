#ifndef ELF_HEADER_HXX
#define ELF_HEADER_HXX

#include "utils/raw_ostream.h"

#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Format.h>

template <unsigned Bitwidth>
void ELFHeader<Bitwidth>::print() {
  using namespace llvm;

  out() << fillformat('=', 79) << '\n';
  out().changeColor(raw_ostream::WHITE, true);
  out() << "ELF Header\n";
  out().resetColor();
  out() << fillformat('-', 79) << '\n';

#define PRINT_LINT(title, value) \
  out() << format("  %-32s : ", (char const *)(title)) << (value) << '\n'
  PRINT_LINT("Class",                 getClassStr(getClass()));
  PRINT_LINT("Endianness",            getEndiannessStr(getEndianness()));
  PRINT_LINT("Header Version",        (unsigned)getVersion());
  PRINT_LINT("OS ABI",                getOSABIStr(getOSABI()));
  PRINT_LINT("ABI Version",           (unsigned)getABIVersion());
  PRINT_LINT("Object Type",           getObjectTypeStr(getObjectType()));
  PRINT_LINT("Machine",               getMachineStr(getMachine()));
  PRINT_LINT("Version",               getVersionStr(getVersion()));
  PRINT_LINT("Entry Address",         getEntryAddress());
  PRINT_LINT("Program Header Offset", getProgramHeaderTableOffset());
  PRINT_LINT("Section Header Offset", getSectionHeaderTableOffset());
  PRINT_LINT("Flags",                 getFlags());
  PRINT_LINT("ELF Header Size",       getELFHeaderSize());
  PRINT_LINT("Program Header Size",   getProgramHeaderEntrySize());
  PRINT_LINT("Program Header Num",    getProgramHeaderNum());
  PRINT_LINT("Section Header Size",   getSectionHeaderEntrySize());
  PRINT_LINT("Section Header Num",    getSectionHeaderNum());
  PRINT_LINT("String Section Index",  getStringSectionIndex());
#undef PRINT_LINT

  out() << fillformat('=', 79) << "\n\n";
}

#endif // ELF_HEADER_HXX
