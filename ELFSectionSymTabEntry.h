#ifndef ELF_SECTION_SYMTAB_ENTRY_H
#define ELF_SECTION_SYMTAB_ENTRY_H

#include "ELFTypes.h"

#include "utils/term.h"

#include <boost/shared_ptr.hpp>
#include <iomanip>
#include <iostream>
#include <string>

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

protected:
  ELFSectionSymTabEntry_CRTP() { }
  ~ELFSectionSymTabEntry_CRTP() { }

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

  bool isValid() const {
    // FIXME: Should check the correctness of the section header.
    return true;
  }

  template <typename Archiver>
  static boost::shared_ptr<ConcreteELFSectionSymTabEntry>
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

template <size_t Bitwidth>
inline char const *ELFSectionSymTabEntry_CRTP<Bitwidth>::getName() const {
  ELFSectionHeaderTable<Bitwidth> const &shtab =
    *owner->getSectionHeaderTable();
  size_t const index = shtab[std::string(".strtab")]->getIndex();
  ELFSection<Bitwidth> const *section = owner->getSectionByIndex(index);
  ELFSectionStrTab<Bitwidth> const &strtab =
    *static_cast<ELFSectionStrTab<Bitwidth> const *>(section);
  return strtab[getNameIndex()];
}

template <size_t Bitwidth>
template <typename Archiver>
inline boost::shared_ptr<ELFSectionSymTabEntry<Bitwidth> >
ELFSectionSymTabEntry_CRTP<Bitwidth>::read(Archiver &AR,
                                           ELFObject<Bitwidth> const *owner,
                                           size_t index) {
  if (!AR) {
    // Archiver is in bad state before calling read function.
    // Return NULL and do nothing.
    return boost::shared_ptr<ConcreteELFSectionSymTabEntry>();
  }

  boost::shared_ptr<ConcreteELFSectionSymTabEntry> sh(
    new ConcreteELFSectionSymTabEntry());

  if (!sh->serialize(AR)) {
    // Unable to read the structure.  Return NULL.
    return boost::shared_ptr<ConcreteELFSectionSymTabEntry>();
  }

  if (!sh->isValid()) {
    // SymTabEntry read from archiver is not valid.  Return NULL.
    return boost::shared_ptr<ConcreteELFSectionSymTabEntry>();
  }

  // Set the section header index
  sh->index = index;

  // Set the owner elf object
  sh->owner = owner;

  return sh;
}

template <size_t Bitwidth>
inline void ELFSectionSymTabEntry_CRTP<Bitwidth>::
  print(bool shouldPrintHeader) const {
  using namespace term::color;
  using namespace std;

  std::ios_base::fmtflags prev_flags = cout.flags();

  if (shouldPrintHeader) {
    cout << endl << setw(79) << setfill('=') << '=' << setfill(' ') << endl;
    cout << light::white()
         << "ELF Symbol Table Entry " << getIndex() << normal() << endl;
    cout << setw(79) << setfill('-') << '-' << setfill(' ') << endl;
  } else {
    cout << setw(79) << setfill('-') << '-' << setfill(' ') << endl;
    cout << light::yellow()
         << "ELF Symbol Table Entry " << getIndex() << " : " << normal() << endl;
  }

  cout << setw(79) << setfill('-') << '-' << endl << setfill(' ');
#define PRINT_LINT(title, value) \
  cout << left << "  " << setw(11) \
       << (title) << " : " << right << (value) << endl
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

  cout.flags( prev_flags );
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
