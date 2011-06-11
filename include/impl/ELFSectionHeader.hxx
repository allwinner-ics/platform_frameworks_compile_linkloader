#ifndef ELF_SECTION_HEADER_HXX
#define ELF_SECTION_HEADER_HXX

#include "utils/raw_ostream.h"
#include <llvm/Support/Format.h>
#include <llvm/Support/raw_ostream.h>

#include "ELFObject.h"

template <unsigned Bitwidth>
char const *ELFSectionHeader_CRTP<Bitwidth>::getName() const {
  return owner->getSectionName(getNameIndex());
}

template <unsigned Bitwidth>
template <typename Archiver>
typename ELFSectionHeader_CRTP<Bitwidth>::ELFSectionHeaderTy *
ELFSectionHeader_CRTP<Bitwidth>::read(Archiver &AR,
                                      ELFObjectTy const *owner,
                                      size_t index) {

  if (!AR) {
    // Archiver is in bad state before calling read function.
    // Return NULL and do nothing.
    return 0;
  }

  llvm::OwningPtr<ELFSectionHeaderTy> sh(new ELFSectionHeaderTy());

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

template <unsigned Bitwidth>
void ELFSectionHeader_CRTP<Bitwidth>::print(bool shouldPrintHeader) const {
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

#endif // ELF_SECTION_HEADER_HXX
