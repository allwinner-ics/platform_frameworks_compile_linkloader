#ifndef ELF_RELOC_H
#define ELF_RELOC_H

#include "ELFTypes.h"

#include "utils/raw_ostream.h"

#include <string>

#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/Format.h>
#include <llvm/Support/raw_ostream.h>

#include <stdint.h>

template <unsigned Bitwidth> class ELFReloc;
template <unsigned Bitwidth> class ELFReloc_CRTP;


template <unsigned Bitwidth>
class ELFReloc_CRTP {
public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

  typedef ELFReloc<Bitwidth> ConcreteELFReloc;

protected:
  size_t index;

  addr_t    r_offset;
  relinfo_t r_info;
  addend_t  r_addend;

protected:
  ELFReloc_CRTP() : index(0), r_offset(0), r_addend(0) { }
  ~ELFReloc_CRTP() { }

public:
  size_t getIndex() const {
    return index;
  }

  addr_t getOffset() const {
    return r_offset;
  }

  addend_t getAddend() const {
    return r_addend;
  }

  bool isValid() const {
    // FIXME: Should check the correctness of the relocation entite.
    return true;
  }

  template <typename Archiver>
  static ConcreteELFReloc *readRel(Archiver &AR, size_t index);

  template <typename Archiver>
  static ConcreteELFReloc *readRela(Archiver &AR, size_t index);

  void print(bool shouldPrintHeader = false) const;

private:
  ConcreteELFReloc *concrete() {
    return static_cast<ConcreteELFReloc *>(this);
  }

  ConcreteELFReloc const *concrete() const {
    return static_cast<ConcreteELFReloc const *>(this);
  }

  template <typename Archiver>
  bool serializeRel(Archiver &AR) {
    assert(r_addend == 0 && "r_addend should be zero before serialization.");

    AR.prologue(TypeTraits<ELFRelocRel<Bitwidth> >::size);

    AR & r_offset;
    AR & r_info;

    AR.epilogue(TypeTraits<ELFRelocRel<Bitwidth> >::size);
    return AR;
  }

  template <typename Archiver>
  bool serializeRela(Archiver &AR) {
    AR.prologue(TypeTraits<ELFRelocRela<Bitwidth> >::size);

    AR & r_offset;
    AR & r_info;
    AR & r_addend;

    AR.epilogue(TypeTraits<ELFRelocRela<Bitwidth> >::size);
    return AR;
  }

};

//==================Inline Member Function Definition==========================

template <unsigned Bitwidth>
template <typename Archiver>
inline ELFReloc<Bitwidth> *
ELFReloc_CRTP<Bitwidth>::readRela(Archiver &AR, size_t index) {
  if (!AR) {
    // Archiver is in bad state before calling read function.
    // Return NULL and do nothing.
    return 0;
  }

  llvm::OwningPtr<ConcreteELFReloc> sh(new ConcreteELFReloc());

  if (!sh->serializeRela(AR)) {
    // Unable to read the structure.  Return NULL.
    return 0;
  }

  if (!sh->isValid()) {
    // Rel read from archiver is not valid.  Return NULL.
    return 0;
  }

  // Set the section header index
  sh->index = index;

  return sh.take();
}

template <unsigned Bitwidth>
template <typename Archiver>
inline ELFReloc<Bitwidth> *
ELFReloc_CRTP<Bitwidth>::readRel(Archiver &AR, size_t index) {
  if (!AR) {
    // Archiver is in bad state before calling read function.
    // Return NULL and do nothing.
    return 0;
  }

  llvm::OwningPtr<ConcreteELFReloc> sh(new ConcreteELFReloc());

  sh->r_addend = 0;
  if (!sh->serializeRel(AR)) {
    return 0;
  }

  if (!sh->isValid()) {
    // Rel read from archiver is not valid.  Return NULL.
    return 0;
  }

  // Set the section header index
  sh->index = index;

  return sh.take();
}

template <unsigned Bitwidth>
inline void ELFReloc_CRTP<Bitwidth>::print(bool shouldPrintHeader) const {
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
  PRINT_LINT("Addend",       concrete()->getAddend()       );
#undef PRINT_LINT
}




template <>
class ELFReloc<32> : public ELFReloc_CRTP<32> {
  friend class ELFReloc_CRTP<32>;

private:
  ELFReloc() {
  }

public:
  word_t getSymTabIndex() const {
    return ELF32_R_SYM(this->r_info);
  }

  word_t getType() const {
    return ELF32_R_TYPE(this->r_info);
  }

};

template <>
class ELFReloc<64> : public ELFReloc_CRTP<64> {
  friend class ELFReloc_CRTP<64>;

private:
  ELFReloc() {
  }

public:
  xword_t getSymTabIndex() const {
    return ELF64_R_SYM(this->r_info);
  }

  xword_t getType() const {
    return ELF64_R_TYPE(this->r_info);
  }
};


#endif // ELF_RELOC_H
