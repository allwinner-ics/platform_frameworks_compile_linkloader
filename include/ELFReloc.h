#ifndef ELF_RELOC_H
#define ELF_RELOC_H

#include "ELFTypes.h"

#include <llvm/ADT/OwningPtr.h>
#include <string>
#include <stdint.h>

template <unsigned Bitwidth>
class ELFReloc_CRTP {
public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

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
  static ELFRelocTy *readRel(Archiver &AR, size_t index);

  template <typename Archiver>
  static ELFRelocTy *readRela(Archiver &AR, size_t index);

  void print(bool shouldPrintHeader = false) const;

private:
  ELFRelocTy *concrete() {
    return static_cast<ELFRelocTy *>(this);
  }

  ELFRelocTy const *concrete() const {
    return static_cast<ELFRelocTy const *>(this);
  }

  template <typename Archiver>
  bool serializeRel(Archiver &AR) {
    assert(r_addend == 0 && "r_addend should be zero before serialization.");

    AR.prologue(TypeTraits<ELFRelocRelTy>::size);

    AR & r_offset;
    AR & r_info;

    AR.epilogue(TypeTraits<ELFRelocRelTy>::size);
    return AR;
  }

  template <typename Archiver>
  bool serializeRela(Archiver &AR) {
    AR.prologue(TypeTraits<ELFRelocRelaTy>::size);

    AR & r_offset;
    AR & r_info;
    AR & r_addend;

    AR.epilogue(TypeTraits<ELFRelocRelaTy>::size);
    return AR;
  }

};

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

#include "impl/ELFReloc.hxx"

#endif // ELF_RELOC_H
