#ifndef ELF_SYMBOL_HXX
#define ELF_SYMBOL_HXX

#include "ELFSectionHeaderTable.h"
#include "ELFSection.h"
#include "ELFSectionStrTab.h"

#include "ELFObject.h"
#include "ELFSectionHeaderTable.h"
#include "ELFSectionProgBits.h"
#include "ELFSectionNoBits.h"

template <unsigned Bitwidth>
inline char const *ELFSymbol_CRTP<Bitwidth>::getName() const {
  ELFSectionHeaderTable<Bitwidth> const &shtab =
    *owner->getSectionHeaderTable();
  size_t const index = shtab.getByName(std::string(".strtab"))->getIndex();
  ELFSection<Bitwidth> const *section = owner->getSectionByIndex(index);
  ELFSectionStrTab<Bitwidth> const &strtab =
    *static_cast<ELFSectionStrTab<Bitwidth> const *>(section);
  return strtab[getNameIndex()];
}

template <unsigned Bitwidth>
template <typename Archiver>
inline ELFSymbol<Bitwidth> *
ELFSymbol_CRTP<Bitwidth>::read(Archiver &AR,
                                           ELFObject<Bitwidth> const *owner,
                                           size_t index) {
  if (!AR) {
    // Archiver is in bad state before calling read function.
    // Return NULL and do nothing.
    return 0;
  }

  llvm::OwningPtr<ConcreteELFSymbol> sh(
    new ConcreteELFSymbol());

  if (!sh->serialize(AR)) {
    // Unable to read the structure.  Return NULL.
    return 0;
  }

  if (!sh->isValid()) {
    // SymTabEntry read from archiver is not valid.  Return NULL.
    return 0;
  }

  // Set the section header index
  sh->index = index;

  // Set the owner elf object
  sh->owner = owner;

  return sh.take();
}

template <unsigned Bitwidth>
inline void ELFSymbol_CRTP<Bitwidth>::
  print(bool shouldPrintHeader) const {
  using namespace llvm;

  if (shouldPrintHeader) {
    out() << '\n' << fillformat('=', 79) << '\n';
    out().changeColor(raw_ostream::WHITE, true);
    out() << "ELF Symbol Table Entry "
          << this->getIndex() << '\n';
    out().resetColor();
    out() << fillformat('-', 79) << '\n';
  } else {
    out() << fillformat('-', 79) << '\n';
    out().changeColor(raw_ostream::YELLOW, true);
    out() << "ELF Symbol Table Entry "
          << this->getIndex() << " : " << '\n';
    out().resetColor();
  }

#define PRINT_LINT(title, value) \
  out() << format("  %-11s : ", (char const *)(title)) << (value) << '\n'
  PRINT_LINT("Name",        getName()                                    );
  PRINT_LINT("Type",        getTypeStr(getType())                        );
  PRINT_LINT("Bind",        getBindingAttributeStr(getBindingAttribute()));
  PRINT_LINT("Visibility",  getVisibilityStr(getVisibility())            );
  PRINT_LINT("Shtab Index", getSectionIndex()                            );
  PRINT_LINT("Value",       getValue()                                   );
  PRINT_LINT("Size",        getSize()                                    );
#undef PRINT_LINT

// TODO: Horizontal type or vertical type can use option to decide.
#if 0
  using namespace term::color;
  using namespace std;
          
  cout << setw(20) << getName() <<
          setw(10) << getTypeStr(getType()) <<
          setw(10) << getBindingAttributeStr(getBindingAttribute()) <<
          setw(15) << getVisibilityStr(getVisibility()) <<
          setw(10) << getSectionIndex() <<
          setw(7) << getValue() <<
          setw(7) << getSize() <<
          endl;
#endif
}

template <unsigned Bitwidth>
void *ELFSymbol_CRTP<Bitwidth>::getAddress() const {
  if (my_addr != 0) {
    return my_addr;
  }
  size_t idx = (size_t)getSectionIndex();
  switch (getType()) {
    default:
      break;

    case STT_OBJECT:
      switch (idx) {
        default:
          {
#ifndef NDEBUG
            ELFSectionHeaderTable<Bitwidth> const *header =
              owner->getSectionHeaderTable();
            assert(((*header)[idx]->getType() == SHT_PROGBITS ||
                    (*header)[idx]->getType() == SHT_NOBITS) &&
                   "STT_OBJECT with not BITS section.");
#endif
            ELFSection<Bitwidth> const *sec = owner->getSectionByIndex(idx);
            assert(sec != 0 && "STT_OBJECT with null section.");

            ELFSectionBits<Bitwidth> const &st =
              static_cast<ELFSectionBits<Bitwidth> const &>(*sec);
            my_addr = const_cast<unsigned char *>(&st[0] + (off_t)getValue());
          }
          break;

        case SHN_COMMON:
          {
#if _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
            int r = posix_memalign(&my_addr,
                                   std::max((size_t)getValue(), sizeof(void*)),
                                   (size_t)getSize());
            assert(r==0 && "posix_memalign failed.");
#else
            my_addr = memalign(std::max((size_t)getValue(), sizeof(void *)),
                               (size_t)getSize());

            assert(my_addr != NULL && "memalign failed.");
#endif
          }
          break;

        case SHN_ABS:
        case SHN_UNDEF:
        case SHN_XINDEX:
          assert(0 && "STT_OBJECT with special st_shndx.");
          break;
      }
      break;


    case STT_FUNC:
      switch (idx) {
        default:
          {
#ifndef NDEBUG
            ELFSectionHeaderTable<Bitwidth> const *header =
              owner->getSectionHeaderTable();
            assert((*header)[idx]->getType() == SHT_PROGBITS &&
                   "STT_FUNC with not PROGBITS section.");
#endif
            ELFSection<Bitwidth> const *sec = owner->getSectionByIndex(idx);
            assert(sec != 0 && "STT_FUNC with null section.");

            ELFSectionProgBits<Bitwidth> const &st =
              static_cast<ELFSectionProgBits<Bitwidth> const &>(*sec);
            my_addr = const_cast<unsigned char *>(&st[0] + (off_t)getValue());
          }
          break;

        case SHN_ABS:
        case SHN_COMMON:
        case SHN_UNDEF:
        case SHN_XINDEX:
          assert(0 && "STT_FUNC with special st_shndx.");
          break;
      }
      break;


    case STT_SECTION:
      switch (idx) {
        default:
          {
#ifndef NDEBUG
            ELFSectionHeaderTable<Bitwidth> const *header =
              owner->getSectionHeaderTable();
            assert(((*header)[idx]->getType() == SHT_PROGBITS ||
                    (*header)[idx]->getType() == SHT_NOBITS) &&
                   "STT_SECTION with not BITS section.");
#endif
            ELFSection<Bitwidth> const *sec = owner->getSectionByIndex(idx);
            assert(sec != 0 && "STT_SECTION with null section.");

            ELFSectionBits<Bitwidth> const &st =
              static_cast<ELFSectionBits<Bitwidth> const &>(*sec);
            my_addr = const_cast<unsigned char *>(&st[0] + (off_t)getValue());
          }
          break;

        case SHN_ABS:
        case SHN_COMMON:
        case SHN_UNDEF:
        case SHN_XINDEX:
          assert(0 && "STT_SECTION with special st_shndx.");
          break;
      }
      break;

    case STT_NOTYPE:
      return 0;

    case STT_COMMON:
    case STT_FILE:
    case STT_TLS:
    case STT_LOOS:
    case STT_HIOS:
    case STT_LOPROC:
    case STT_HIPROC:
      assert(0 && "Not implement.");
      return 0;
  }
  return my_addr;
}

#endif // ELF_SYMBOL_HXX
