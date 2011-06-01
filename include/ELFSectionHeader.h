#ifndef ELF_SECTION_HEADER_H
#define ELF_SECTION_HEADER_H

#include "ELFTypes.h"

#include "utils/raw_ostream.h"

#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/Format.h>
#include <llvm/Support/raw_ostream.h>

#include <stdint.h>

template <unsigned Bitwidth> class ELFObject;
template <unsigned Bitwidth> class ELFSectionHeader;
template <unsigned Bitwidth> class ELFSectionHeader_CRTP;


class ELFSectionHeaderHelperMixin {
protected:
  static char const *getSectionTypeStr(uint32_t type);
};


template <unsigned Bitwidth>
class ELFSectionHeader_CRTP : private ELFSectionHeaderHelperMixin {
public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

  typedef ELFSectionHeader<Bitwidth> ConcreteELFSectionHeader;

protected:
  ELFObject<Bitwidth> const *owner;

  size_t index;

  word_t sh_name;
  word_t sh_type;
  addr_t sh_addr;
  offset_t sh_offset;
  word_t sh_link;
  word_t sh_info;

protected:
  ELFSectionHeader_CRTP() { }
  ~ELFSectionHeader_CRTP() { }

public:
  size_t getIndex() const {
    return index;
  }

  word_t getNameIndex() const {
    return sh_name;
  }

  char const *getName() const {
    return owner->getSectionName(getNameIndex());
  }

  word_t getType() const {
    return sh_type;
  }

  addr_t getAddress() const {
    return sh_addr;
  }

  offset_t getOffset() const {
    return sh_offset;
  }

  word_t getLink() const {
    return sh_link;
  }

  word_t getExtraInfo() const {
    return sh_info;
  }

  bool isValid() const {
    // FIXME: Should check the correctness of the section header.
    return true;
  }

  template <typename Archiver>
  static ConcreteELFSectionHeader *
  read(Archiver &AR,
       ELFObject<Bitwidth> const *owner,
       size_t index = 0) {

    if (!AR) {
      // Archiver is in bad state before calling read function.
      // Return NULL and do nothing.
      return 0;
    }

    llvm::OwningPtr<ConcreteELFSectionHeader> sh(
      new ConcreteELFSectionHeader());

    if (!sh->serialize(AR)) {
      // Unable to read the structure.  Return NULL.
      return 0;
    }

    if (!sh->isValid()) {
      // Header read from archiver is not valid.  Return NULL.
      return 0;
    }

    // Set the section header index
    sh->index = index;

    // Set the owner elf object
    sh->owner = owner;

    return sh.take();
  }

  void print(bool shouldPrintHeader = false) const {
    using namespace llvm;

    if (shouldPrintHeader) {
      out() << '\n' << fillformat('=', 79) << '\n';
      out().changeColor(raw_ostream::WHITE, true);
      out() << "ELF Section Header "
            << this->getIndex() << '\n';
      out().resetColor();
      out() << fillformat('-', 79) << '\n';
    } else {
      out() << fillformat('-', 79) << '\n';
      out().changeColor(raw_ostream::YELLOW, true);
      out() << "ELF Section Header "
            << this->getIndex() << " : " << '\n';
      out().resetColor();
    }

#define PRINT_LINT(title, value) \
  out() << format("  %-13s : ", (char const *)(title)) << (value) << '\n'
    PRINT_LINT("Name",          getName() );
    PRINT_LINT("Type",          getSectionTypeStr(getType()));
    PRINT_LINT("Flags",         concrete()->getFlags());
    PRINT_LINT("Address",       getAddress());
    PRINT_LINT("Offset",        getOffset());
    PRINT_LINT("Size",          concrete()->getSize());
    PRINT_LINT("Link",          getLink());
    PRINT_LINT("Extra Info",    getExtraInfo());
    PRINT_LINT("Address Align", concrete()->getAddressAlign());
    PRINT_LINT("Entry Size",    concrete()->getEntrySize());
#undef PRINT_LINT

    if (shouldPrintHeader) {
      out() << fillformat('=', 79) << '\n';
    }
  }

private:
  ConcreteELFSectionHeader *concrete() {
    return static_cast<ConcreteELFSectionHeader *>(this);
  }

  ConcreteELFSectionHeader const *concrete() const {
    return static_cast<ConcreteELFSectionHeader const *>(this);
  }
};


template <>
class ELFSectionHeader<32> : public ELFSectionHeader_CRTP<32> {
  friend class ELFSectionHeader_CRTP<32>;

private:
  word_t sh_flags;
  word_t sh_size;
  word_t sh_addralign;
  word_t sh_entsize;

private:
  ELFSectionHeader() {
  }

  template <typename Archiver>
  bool serialize(Archiver &AR) {
    AR.prologue(TypeTraits<ELFSectionHeader>::size);

    AR & sh_name;
    AR & sh_type;
    AR & sh_flags;
    AR & sh_addr;
    AR & sh_offset;
    AR & sh_size;
    AR & sh_link;
    AR & sh_info;
    AR & sh_addralign;
    AR & sh_entsize;

    AR.epilogue(TypeTraits<ELFSectionHeader>::size);
    return AR;
  }

public:
  word_t getFlags() const {
    return sh_flags;
  }

  word_t getSize() const {
    return sh_size;
  }

  word_t getAddressAlign() const {
    return sh_addralign;
  }

  word_t getEntrySize() const {
    return sh_entsize;
  }
};

template <>
class ELFSectionHeader<64> : public ELFSectionHeader_CRTP<64> {
  friend class ELFSectionHeader_CRTP<64>;

private:
  xword_t sh_flags;
  xword_t sh_size;
  xword_t sh_addralign;
  xword_t sh_entsize;

private:
  ELFSectionHeader() {
  }

  template <typename Archiver>
  bool serialize(Archiver &AR) {
    AR.prologue(TypeTraits<ELFSectionHeader>::size);

    AR & sh_name;
    AR & sh_type;
    AR & sh_flags;
    AR & sh_addr;
    AR & sh_offset;
    AR & sh_size;
    AR & sh_link;
    AR & sh_info;
    AR & sh_addralign;
    AR & sh_entsize;

    AR.epilogue(TypeTraits<ELFSectionHeader>::size);
    return AR;
  }

public:
  xword_t getFlags() const {
    return sh_flags;
  }

  xword_t getSize() const {
    return sh_size;
  }

  xword_t getAddressAlign() const {
    return sh_addralign;
  }

  xword_t getEntrySize() const {
    return sh_entsize;
  }
};


#endif // ELF_SECTION_HEADER_H
