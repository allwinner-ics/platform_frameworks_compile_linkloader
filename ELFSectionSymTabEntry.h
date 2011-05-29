#ifndef ELF_SECTION_SYMTAB_ENTRY_H
#define ELF_SECTION_SYMTAB_ENTRY_H

#include "ELFTypes.h"

#include <llvm/ADT/OwningPtr.h>
#include <elf.h>

#include <string>
#include <cassert>
#include <cstdlib>
#include <algorithm>

#include <malloc.h>
#include <stdint.h>

template <size_t Bitwidth> class ELFObject;
template <size_t Bitwidth> class ELFSectionSymTabEntry;
template <size_t Bitwidth> class ELFSectionSymTabEntry_CRTP;


class ELFSectionSymTabEntryHelperMixin {
protected:
  static char const *getTypeStr(uint8_t);
  static char const *getBindingAttributeStr(uint8_t);
  static char const *getVisibilityStr(uint8_t);
};


template <size_t Bitwidth>
class ELFSectionSymTabEntry_CRTP : private ELFSectionSymTabEntryHelperMixin {
public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

  typedef ELFSectionSymTabEntry<Bitwidth> ConcreteELFSectionSymTabEntry;

protected:
  ELFObject<Bitwidth> const *owner;

  size_t index;

  word_t st_name;
  byte_t st_info;
  byte_t st_other;
  half_t st_shndx;
  addr_t st_value;
  xword_t st_size;

  mutable void *my_addr;

protected:
  ELFSectionSymTabEntry_CRTP() { my_addr = 0; }
  ~ELFSectionSymTabEntry_CRTP() {
    if (my_addr != 0 &&
        getType() == STT_OBJECT &&
        getSectionIndex() == SHN_COMMON) {
      std::free(my_addr);
    }
  }

public:
  size_t getIndex() const {
    return index;
  }

  word_t getNameIndex() const {
    return st_name;
  }

  char const *getName() const;

// I don't want to include elf.h in .h file, so define those macro by ourself.
#define ELF_ST_BIND(i)   ((i)>>4)
#define ELF_ST_TYPE(i)   ((i)&0xf)
#define ELF_ST_INFO(b,t) (((b)<<4)+((t)&0xf))
  byte_t getType() const {
    return ELF_ST_TYPE(st_info);
  }

  byte_t getBindingAttribute() const {
    return ELF_ST_BIND(st_info);
  }
#undef ELF_ST_BIND
#undef ELF_ST_TYPE
#undef ELF_ST_INFO

#define ELF_ST_VISIBILITY(o) ((o)&0x3)
  byte_t getVisibility() const {
    return ELF_ST_VISIBILITY(st_other);
  }
#undef ELF_ST_VISIBILITY

  half_t getSectionIndex() const {
    return st_shndx;
  }

  addr_t getValue() const {
    return st_value;
  }

  xword_t getSize() const {
    return st_size;
  }

  void *getAddress() const;

  void setAddress(void *addr) {
    my_addr = addr;
  }

  bool isValid() const {
    // FIXME: Should check the correctness of the section header.
    return true;
  }

  template <typename Archiver>
  static ConcreteELFSectionSymTabEntry *
  read(Archiver &AR, ELFObject<Bitwidth> const *owner, size_t index = 0);

  void print(bool shouldPrintHeader = false) const;

private:
  ConcreteELFSectionSymTabEntry *concrete() {
    return static_cast<ConcreteELFSectionSymTabEntry *>(this);
  }

  ConcreteELFSectionSymTabEntry const *concrete() const {
    return static_cast<ConcreteELFSectionSymTabEntry const *>(this);
  }
};

//==================Inline Member Function Definition==========================

#include "ELFSectionHeaderTable.h"
#include "ELFSection.h"
#include "ELFSectionStrTab.h"

#include "ELFObject.h"
#include "ELFSectionHeaderTable.h"
#include "ELFSectionProgBits.h"
#include "ELFSectionNoBits.h"

template <size_t Bitwidth>
inline char const *ELFSectionSymTabEntry_CRTP<Bitwidth>::getName() const {
  ELFSectionHeaderTable<Bitwidth> const &shtab =
    *owner->getSectionHeaderTable();
  size_t const index = shtab.getByName(std::string(".strtab"))->getIndex();
  ELFSection<Bitwidth> const *section = owner->getSectionByIndex(index);
  ELFSectionStrTab<Bitwidth> const &strtab =
    *static_cast<ELFSectionStrTab<Bitwidth> const *>(section);
  return strtab[getNameIndex()];
}

template <size_t Bitwidth>
template <typename Archiver>
inline ELFSectionSymTabEntry<Bitwidth> *
ELFSectionSymTabEntry_CRTP<Bitwidth>::read(Archiver &AR,
                                           ELFObject<Bitwidth> const *owner,
                                           size_t index) {
  if (!AR) {
    // Archiver is in bad state before calling read function.
    // Return NULL and do nothing.
    return 0;
  }

  llvm::OwningPtr<ConcreteELFSectionSymTabEntry> sh(
    new ConcreteELFSectionSymTabEntry());

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

template <size_t Bitwidth>
inline void ELFSectionSymTabEntry_CRTP<Bitwidth>::
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

template <size_t Bitwidth>
void *ELFSectionSymTabEntry_CRTP<Bitwidth>::getAddress() const {
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




template <>
class ELFSectionSymTabEntry<32> : public ELFSectionSymTabEntry_CRTP<32> {
  friend class ELFSectionSymTabEntry_CRTP<32>;

private:
  ELFSectionSymTabEntry() {
  }

  template <typename Archiver>
  bool serialize(Archiver &AR) {
    AR.prologue(TypeTraits<ELFSectionSymTabEntry>::size);

    AR & st_name;
    AR & st_value;
    AR & st_size;
    AR & st_info;
    AR & st_other;
    AR & st_shndx;

    AR.epilogue(TypeTraits<ELFSectionSymTabEntry>::size);
    return AR;
  }

public:
};

template <>
class ELFSectionSymTabEntry<64> : public ELFSectionSymTabEntry_CRTP<64> {
  friend class ELFSectionSymTabEntry_CRTP<64>;

private:
  ELFSectionSymTabEntry() {
  }

  template <typename Archiver>
  bool serialize(Archiver &AR) {
    AR.prologue(TypeTraits<ELFSectionSymTabEntry>::size);

    AR & st_name;
    AR & st_info;
    AR & st_other;
    AR & st_shndx;
    AR & st_value;
    AR & st_size;

    AR.epilogue(TypeTraits<ELFSectionSymTabEntry>::size);
    return AR;
  }

public:
};


#endif // ELF_SECTION_SYMTAB_ENTRY_H
