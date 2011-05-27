#ifndef ELF_SECTION_REL_H
#define ELF_SECTION_REL_H

#include "ELFTypes.h"

#include "utils/raw_ostream.h"

#include <string>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Format.h>

#include <stdint.h>

template <size_t Bitwidth> class ELFObject;
template <size_t Bitwidth> class ELFSectionRel;
template <size_t Bitwidth> class ELFSectionRel_CRTP;


class ELFSectionRelHelperMixin {
protected:
//  static char const *getTypeStr(uint8_t);
//  static char const *getBindingAttributeStr(uint8_t);
//  static char const *getVisibilityStr(uint8_t);
};


template <size_t Bitwidth>
class ELFSectionRel_CRTP : private ELFSectionRelHelperMixin {
public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

  typedef ELFSectionRel<Bitwidth> ConcreteELFSectionRel;

protected:
  ELFObject<Bitwidth> const *owner;

  size_t index;

  addr_t r_offset;
  xword_t r_info;

protected:
  ELFSectionRel_CRTP() { }
  ~ELFSectionRel_CRTP() { }

public:
  size_t getIndex() const {
    return index;
  }

  addr_t getOffset() const {
    return this->r_offset;
  }

  bool isValid() const {
    // FIXME: Should check the correctness of the section header.
    return true;
  }

  template <typename Archiver>
  static ConcreteELFSectionRel *
  read(Archiver &AR, ELFObject<Bitwidth> const *owner, size_t index = 0);

  void print(bool shouldPrintHeader = false) const;

private:
  template <typename Archiver>
  bool serialize(Archiver &AR) {
    AR.prologue(TypeTraits<ConcreteELFSectionRel>::size);

    AR & r_offset;
    AR & r_info;

    AR.epilogue(TypeTraits<ConcreteELFSectionRel>::size);
    return AR;
  }

  ConcreteELFSectionRel *concrete() {
    return static_cast<ConcreteELFSectionRel *>(this);
  }

  ConcreteELFSectionRel const *concrete() const {
    return static_cast<ConcreteELFSectionRel const *>(this);
  }
};

//==================Inline Member Function Definition==========================

template <size_t Bitwidth>
template <typename Archiver>
inline ELFSectionRel<Bitwidth> *
ELFSectionRel_CRTP<Bitwidth>::read(Archiver &AR,
                                   ELFObject<Bitwidth> const *owner,
                                   size_t index) {
  if (!AR) {
    // Archiver is in bad state before calling read function.
    // Return NULL and do nothing.
    return 0;
  }

  llvm::OwningPtr<ConcreteELFSectionRel> sh(new ConcreteELFSectionRel());

  if (!sh->serialize(AR)) {
    // Unable to read the structure.  Return NULL.
    return 0;
  }

  if (!sh->isValid()) {
    // Rel read from archiver is not valid.  Return NULL.
    return 0;
  }

  // Set the section header index
  sh->index = index;

  // Set the owner elf object
  sh->owner = owner;

  return sh.take();
}

template <size_t Bitwidth>
inline void ELFSectionRel_CRTP<Bitwidth>::
  print(bool shouldPrintHeader) const {
  using namespace llvm;
  if (shouldPrintHeader) {
    out() << '\n' << fillformat('=', 79) << '\n';
    out().changeColor(raw_ostream::WHITE, true);
    out() << "ELF Relaocation Table Entry "
          << this->getIndex() << '\n';
    out().resetColor();
    out() << fillformat('-', 79) << '\n';
  } else {
    out() << fillformat('-', 79) << '\n';
    out().changeColor(raw_ostream::YELLOW, true);
    out() << "ELF Relaocation Table Entry "
          << this->getIndex() << " : " << '\n';
    out().resetColor();
  }

#define PRINT_LINT(title, value) \
  out() << format("  %-13s : ", (char const *)(title)) << (value) << '\n'
  PRINT_LINT("Offset",       concrete()->getOffset()       );
  PRINT_LINT("SymTab Index", concrete()->getSymTabIndex()  );
  PRINT_LINT("Type",         concrete()->getType()         );
#undef PRINT_LINT
}




template <>
class ELFSectionRel<32> : public ELFSectionRel_CRTP<32> {
  friend class ELFSectionRel_CRTP<32>;

// Note: Protected for Rela
protected:
  ELFSectionRel() {
  }

//#define ELF32_R_SYM(i)  ((i)>>8)
  xword_t getSymTabIndex() const {
    return ELF32_R_SYM(this->r_info);
  }
//#undef ELF32_R_SYM

//#define ELF32_R_TYPE(i)   ((unsigned char)(i))
  xword_t getType() const {
    return ELF32_R_TYPE(this->r_info);
  }
//#undef ELF32_R_TYPE

public:
};

template <>
class ELFSectionRel<64> : public ELFSectionRel_CRTP<64> {
  friend class ELFSectionRel_CRTP<64>;

// Note: Protected for Rela
protected:
  ELFSectionRel() {
  }

//#define ELF64_R_SYM(i)    ((i)>>32)
  xword_t getSymTabIndex() const {
    return ELF64_R_SYM(this->r_info);
  }
//#undef ELF64_R_SYM

//#define ELF64_R_TYPE(i)   ((i)&0xffffffffL)
  xword_t getType() const {
    return ELF64_R_TYPE(this->r_info);
  }
//#undef ELF64_R_TYPE

public:
};


#endif // ELF_SECTION_REL_H
