#ifndef ELF_HEADER_H
#define ELF_HEADER_H

#include "ELFTypes.h"

#include "utils/raw_ostream.h"

#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Format.h>

#include <elf.h>
#include <string.h>


template <unsigned Bitwidth> class ELFHeader;


class ELFHeaderHelperMixin {
protected:
  static char const *getClassStr(int clazz);
  static char const *getEndiannessStr(int endianness);
  static char const *getOSABIStr(int abi);
  static char const *getObjectTypeStr(uint16_t type);
  static char const *getMachineStr(uint16_t machine);
  static char const *getVersionStr(uint32_t version);
};


template <unsigned Bitwidth>
class ELFHeader : private ELFHeaderHelperMixin {
public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

protected:
  byte_t   e_ident[EI_NIDENT];
  half_t   e_type;
  half_t   e_machine;
  word_t   e_version;
  addr_t   e_entry;
  offset_t e_phoff;
  offset_t e_shoff;
  word_t   e_flags;
  half_t   e_ehsize;
  half_t   e_phentsize;
  half_t   e_phnum;
  half_t   e_shentsize;
  half_t   e_shnum;
  half_t   e_shstrndx;

protected:
  ELFHeader() { }

public:
  byte_t getClass() const {
    return e_ident[EI_CLASS];
  }

  byte_t getEndianness() const {
    return e_ident[EI_DATA];
  }

  byte_t getVersionFromIdent() const {
    return e_ident[EI_VERSION];
  }

  byte_t getOSABI() const {
    return e_ident[EI_OSABI];
  }

  byte_t getABIVersion() const {
    return e_ident[EI_ABIVERSION];
  }

  bool is32bit() const {
    return e_ident[EI_CLASS] == ELFCLASS32;
  }

  bool is64bit() const {
    return e_ident[EI_CLASS] == ELFCLASS64;
  }

  bool isBigEndian() const {
    return e_ident[EI_DATA] == ELFDATA2MSB;
  }

  bool isLittleEndian() const {
    return e_ident[EI_DATA] == ELFDATA2LSB;
  }

  half_t getObjectType() const {
    return e_type;
  }

  half_t getMachine() const {
    return e_machine;
  }

  word_t getVersion() const {
    return e_version;
  }

  addr_t getEntryAddress() const {
    return e_entry;
  }

  offset_t getProgramHeaderTableOffset() const {
    return e_phoff;
  }

  offset_t getSectionHeaderTableOffset() const {
    return e_shoff;
  }

  word_t getFlags() const {
    return e_flags;
  }

  half_t getELFHeaderSize() const {
    return e_ehsize;
  }

  half_t getProgramHeaderEntrySize() const {
    return e_phentsize;
  }

  half_t getProgramHeaderNum() const {
    return e_phnum;
  }

  half_t getSectionHeaderEntrySize() const {
    return e_shentsize;
  }

  half_t getSectionHeaderNum() const {
    return e_shnum;
  }

  half_t getStringSectionIndex() const {
    return e_shstrndx;
  }

  template <typename Archiver>
  static ELFHeader *read(Archiver &AR) {
    if (!AR) {
      // Archiver is in bad state before calling read function.
      // Return NULL and do nothing.
      return 0;
    }

    llvm::OwningPtr<ELFHeader> header(new ELFHeader());
    if (!header->serialize(AR)) {
      // Unable to read the structure.  Return NULL.
      return 0;
    }

    if (!header->isValid()) {
      // Header read from archiver is not valid.  Return NULL.
      return 0;
    }

    return header.take();
  }

  void print() {
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

  bool isValid() const {
    return (isValidELFIdent() && isCompatibleHeaderSize());
  }

private:
  template <typename Archiver>
  bool serialize(Archiver &AR) {
    AR.prologue(TypeTraits<ELFHeader<Bitwidth> >::size);

    AR & e_ident;
    AR & e_type;
    AR & e_machine;
    AR & e_version;
    AR & e_entry;
    AR & e_phoff;
    AR & e_shoff;
    AR & e_flags;
    AR & e_ehsize;
    AR & e_phentsize;
    AR & e_phnum;
    AR & e_shentsize;
    AR & e_shnum;
    AR & e_shstrndx;

    AR.epilogue(TypeTraits<ELFHeader<Bitwidth> >::size);
    return AR;
  }

  bool isValidMagicWord() const {
    return (memcmp(e_ident, "\x7f" "ELF", 4) == 0);
  }

  bool isValidClass() const {
    return ((Bitwidth == 32 && is32bit()) ||
            (Bitwidth == 64 && is64bit()));
  }

  bool isValidEndianness() const {
    return (isBigEndian() || isLittleEndian());
  }

  bool isValidHeaderVersion() const {
    return (getVersion() == EV_CURRENT);
  }

  bool isUnusedZeroedPadding() const {
    for (size_t i = EI_PAD; i < EI_NIDENT; ++i) {
      if (e_ident[i] != 0) {
        return false;
      }
    }
    return true;
  }

  bool isValidELFIdent() const {
    return (isValidMagicWord() &&
            isValidClass() &&
            isValidEndianness() &&
            isValidHeaderVersion() &&
            isUnusedZeroedPadding());
  }

  bool isCompatibleHeaderSize() const {
    if (e_ehsize < TypeTraits<ELFHeader<Bitwidth> >::size) {
      return false;
    }

    if (e_phnum > 0 &&
        e_phentsize < TypeTraits<ELFProgramHeader<Bitwidth> >::size) {
      return false;
    }

    if (e_shnum > 0 &&
        e_shentsize < TypeTraits<ELFSectionHeader<Bitwidth> >::size) {
      return false;
    }

    return true;
  }
};

#endif // ELF_HEADER_H
