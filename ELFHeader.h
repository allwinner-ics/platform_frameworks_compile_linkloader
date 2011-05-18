#ifndef ELF_HEADER_H
#define ELF_HEADER_H

#include "ELFTypes.h"

#include <boost/shared_ptr.hpp>
#include <elf.h>

template <size_t Bitwidth, class ConcreteELFHeader>
class ELFHeader_CRTP {
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
      return 0;
    }

    boost::shared_ptr<ConcreteELFHeader> header(new ConcreteELFHeader());
    if (!header->serialize(AR)) {
      // Unable to read the structure.  Return NULL.
      return 0;
    }

    if (!header->isValid()) {
      // Header read from archiver is not valid.  Return NULL.
      return 0;
    }

    return header;
  }
};


template <size_t Bitwidth>
class ELFHeader;

template <>
class ELFHeader<32> : public ELFHeader_CRTP<32, ELFHeader<32> > {
private:
  enum { ELF_HEADER_SIZE = 52 };

protected:
  ELFHeader() { }

public:
  template <typename Archiver>
  bool serialize(Archiver &AR) {
    AR.prologue(ELF_HEADER_SIZE);

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

    AR.epilogue(ELF_HEADER_SIZE);
    return AR;
  }
};

template <>
class ELFHeader<64> : public ELFHeader_CRTP<64, ELFHeader<64> > {
private:
  enum { ELF_HEADER_SIZE = 64 };

protected:
  ELFHeader() { }

public:
  template <typename Archiver>
  bool serialize(Archiver &AR) {
    AR.prologue(ELF_HEADER_SIZE);

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

    AR.epilogue(ELF_HEADER_SIZE);
    return AR;
  }
};

#endif // ELF_HEADER_H
