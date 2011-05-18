#ifndef ELF_HEADER_H
#define ELF_HEADER_H

#include "ELFTypes.h"

#include "utils/term.h"

#include <boost/shared_ptr.hpp>
#include <iomanip>
#include <iostream>

#include <elf.h>
#include <string.h>


template <size_t Bitwidth> class ELFHeader;
template <size_t Bitwidth> class ELFHeader_CRTP;


class ELFHeaderHelperMixin {
protected:
  static char const *getClassStr(int clazz);
  static char const *getEndiannessStr(int endianness);
  static char const *getOSABIStr(int abi);
  static char const *getObjectTypeStr(uint16_t type);
  static char const *getMachineStr(uint16_t machine);
  static char const *getVersionStr(uint32_t version);
};


template <size_t Bitwidth>
class ELFHeader_CRTP : private ELFHeaderHelperMixin {
public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

  typedef ELFHeader<Bitwidth> ConcreteELFHeader;

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
  ELFHeader_CRTP() { }
  ~ELFHeader_CRTP() { }

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
  static boost::shared_ptr<ConcreteELFHeader> read(Archiver &AR) {
    if (!AR) {
      // Archiver is in bad state before calling read function.
      // Return NULL and do nothing.
      return boost::shared_ptr<ConcreteELFHeader>();
    }

    boost::shared_ptr<ConcreteELFHeader> header(new ConcreteELFHeader());
    if (!header->serialize(AR)) {
      // Unable to read the structure.  Return NULL.
      return boost::shared_ptr<ConcreteELFHeader>();
    }

    if (!header->isValid()) {
      // Header read from archiver is not valid.  Return NULL.
      return boost::shared_ptr<ConcreteELFHeader>();
    }

    return header;
  }

  void print() {
    using namespace std;
    using namespace term;
    using namespace term::color;

    cout << endl << setw(79) << setfill('=') << '=' << endl;

    cout << light::white() << "ELF Header" << normal() << endl;

    cout << setw(79) << setfill('-') << '-' << endl << setfill(' ');

    cout << setw(25) << "Class" << " : " << getClassStr(getClass()) << endl;

    cout << setw(25) << "Endianness" << " : "
         << getEndiannessStr(getEndianness()) << endl;

    cout << setw(25) << "Header Version" << " : "
         << (unsigned)getVersion() << endl;

    cout << setw(25) << "OS ABI" << " : "
         << getOSABIStr(getOSABI()) << endl;

    cout << setw(25) << "ABI Version" << " : "
         << (unsigned)getABIVersion() << endl;


    cout << setw(25) << "Object Type" << " : "
         << getObjectTypeStr(getObjectType()) << endl;

    cout << setw(25) << "Machine" << " : "
         << getMachineStr(getMachine()) << endl;

    cout << setw(25) << "Version" << " : "
         << getVersionStr(getVersion()) << endl;

    cout << setw(25) << "Entry Address" << " : "
         << getEntryAddress() << endl;

    cout << setw(25) << "Program Header Offset" << " : "
         << getProgramHeaderTableOffset() << endl;

    cout << setw(25) << "Section Header Offset" << " : "
         << getSectionHeaderTableOffset() << endl;

    cout << setw(25) << "Flags" << " : " << getFlags() << endl;

    cout << setw(25) << "ELF Header Size" << " : "
         << getELFHeaderSize() << endl;

    cout << setw(25) << "Program Header Size" << " : "
         << getProgramHeaderEntrySize() << endl;

    cout << setw(25) << "Program Header Num" << " : "
         << getProgramHeaderNum() << endl;

    cout << setw(25) << "Section Header Size" << " : "
         << getSectionHeaderEntrySize() << endl;

    cout << setw(25) << "Section Header Num" << " : "
         << getSectionHeaderNum() << endl;

    cout << setw(25) << "String Section Index" << " : "
         << getStringSectionIndex() << endl;

    cout << setw(79) << setfill('=') << '=' << endl << endl;
  }

  bool isValid() const {
    return (isValidELFIdent() && isCompatibleHeaderSize());
  }

private:
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


template <>
class ELFHeader<32> : public ELFHeader_CRTP<32> {
  friend class ELFHeader_CRTP<32>;

private:
  ELFHeader() { }

  template <typename Archiver>
  bool serialize(Archiver &AR) {
    AR.prologue(TypeTraits<ELFHeader>::size);

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

    AR.epilogue(TypeTraits<ELFHeader>::size);
    return AR;
  }
};

template <>
class ELFHeader<64> : public ELFHeader_CRTP<64> {
  friend class ELFHeader_CRTP<64>;

private:
  ELFHeader() { }

  template <typename Archiver>
  bool serialize(Archiver &AR) {
    AR.prologue(TypeTraits<ELFHeader>::size);

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

    AR.epilogue(TypeTraits<ELFHeader>::size);
    return AR;
  }
};

#endif // ELF_HEADER_H
