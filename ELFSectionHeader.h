#ifndef ELF_SECTION_HEADER_H
#define ELF_SECTION_HEADER_H

#include "ELFTypes.h"

#include "utils/term.h"

#include <boost/shared_ptr.hpp>
#include <iomanip>
#include <iostream>

#include <stdint.h>

template <size_t Bitwidth> class ELFObject;
template <size_t Bitwidth> class ELFSectionHeader;
template <size_t Bitwidth> class ELFSectionHeader_CRTP;


class ELFSectionHeaderHelperMixin {
protected:
  static char const *getSectionTypeStr(uint32_t type);
};


template <size_t Bitwidth>
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
  static boost::shared_ptr<ConcreteELFSectionHeader>
  read(Archiver &AR,
       ELFObject<Bitwidth> const *owner,
       size_t index = 0) {

    if (!AR) {
      // Archiver is in bad state before calling read function.
      // Return NULL and do nothing.
      return boost::shared_ptr<ConcreteELFSectionHeader>();
    }

    boost::shared_ptr<ConcreteELFSectionHeader> sh(
      new ConcreteELFSectionHeader());

    if (!sh->serialize(AR)) {
      // Unable to read the structure.  Return NULL.
      return boost::shared_ptr<ConcreteELFSectionHeader>();
    }

    if (!sh->isValid()) {
      // Header read from archiver is not valid.  Return NULL.
      return boost::shared_ptr<ConcreteELFSectionHeader>();
    }

    // Set the section header index
    sh->index = index;

    // Set the owner elf object
    sh->owner = owner;

    return sh;
  }

  void print(bool shouldPrintHeader = false) const {
    using namespace std;
    using namespace term;
    using namespace term::color;

    if (shouldPrintHeader) {
      cout << endl << setw(79) << setfill('=') << '=' << setfill(' ') << endl;
      cout << light::white()
           << "ELF Section Header " << index << normal() << endl;
      cout << setw(79) << setfill('-') << '-' << setfill(' ') << endl;
    } else {
      cout << setw(79) << setfill('-') << '-' << setfill(' ') << endl;
      cout << light::yellow()
           << "ELF Section Header " << index << " : " << normal() << endl;
    }

    cout << "  " << setw(13) << "Name" << " : " << getName() << endl;

    cout << "  " << setw(13) << "Type" << " : "
         << getSectionTypeStr(getType()) << endl;

    cout << "  " << setw(13) << "Flags" << " : "
         << concrete()->getFlags() << endl;

    cout << "  " << setw(13) << "Address" << " : " << getAddress() << endl;

    cout << "  " << setw(13) << "Offset" << " : " << getOffset() << endl;

    cout << "  " << setw(13) << "Size" << " : "
         << concrete()->getSize() << endl;

    cout << "  " << setw(13) << "Link" << " : " << getLink() << endl;

    cout << "  " << setw(13) << "Extra Info" << " : "
         << getExtraInfo() << endl;

    cout << "  " << setw(13) << "Address Align" << " : "
         << concrete()->getAddressAlign() << endl;

    cout << "  " << setw(13) << "Entry Size" << " : "
         << concrete()->getEntrySize() << endl;

    if (shouldPrintHeader) {
      cout << setw(79) << setfill('=') << '=' << setfill(' ') << endl << endl;
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
