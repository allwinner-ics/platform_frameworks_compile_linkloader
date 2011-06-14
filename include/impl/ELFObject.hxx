#ifndef ELF_OBJECT_HXX
#define ELF_OBJECT_HXX

#include "ELFHeader.h"
#include "ELFReloc.h"
#include "ELFSection.h"
#include "ELFSectionHeaderTable.h"
#include "StubLayout.h"

#include <llvm/ADT/SmallVector.h>

#include "utils/rsl_assert.h"
#include <elf.h>

template <unsigned Bitwidth>
template <typename Archiver>
inline ELFObject<Bitwidth> *
ELFObject<Bitwidth>::read(Archiver &AR) {
  llvm::OwningPtr<ELFObjectTy> object(new ELFObjectTy());

  // Read header
  object->header.reset(ELFHeaderTy::read(AR));
  if (!object->header) {
    return 0;
  }

  // Read section table
  object->shtab.reset(ELFSectionHeaderTableTy::read(AR, object.get()));
  if (!object->shtab) {
    return 0;
  }

  // Read each section
  llvm::SmallVector<size_t, 4> progbits_ndx;
  for (size_t i = 0; i < object->header->getSectionHeaderNum(); ++i) {
    if ((*object->shtab)[i]->getType() == SHT_PROGBITS) {
      object->stab.push_back(NULL);
      progbits_ndx.push_back(i);
    } else {
      llvm::OwningPtr<ELFSectionTy> sec(
        ELFSectionTy::read(AR, object.get(), (*object->shtab)[i]));
      object->stab.push_back(sec.take());
    }
  }

  for (size_t i = 0; i < progbits_ndx.size(); ++i) {
    size_t index = progbits_ndx[i];

    llvm::OwningPtr<ELFSectionTy> sec(
      ELFSectionTy::read(AR, object.get(), (*object->shtab)[index]));
    object->stab[index] = sec.take();
  }

  return object.take();
}

template <unsigned Bitwidth>
inline char const *ELFObject<Bitwidth>::getSectionName(size_t i) const {
  ELFSectionTy const *sec = stab[header->getStringSectionIndex()];

  if (sec) {
    ELFSectionStrTabTy const &st =
      static_cast<ELFSectionStrTabTy const &>(*sec);
    return st[i];
  }

  return NULL;
}

template <unsigned Bitwidth>
inline ELFSection<Bitwidth> const *
ELFObject<Bitwidth>::getSectionByIndex(size_t i) const {
  return stab[i];
}

template <unsigned Bitwidth>
inline ELFSection<Bitwidth> *
ELFObject<Bitwidth>::getSectionByIndex(size_t i) {
  return stab[i];
}

template <unsigned Bitwidth>
inline ELFSection<Bitwidth> const *
ELFObject<Bitwidth>::getSectionByName(std::string const &str) const {
  size_t idx = getSectionHeaderTable()->getByName(str)->getIndex();
  return stab[idx];
}

template <unsigned Bitwidth>
inline ELFSection<Bitwidth> *
ELFObject<Bitwidth>::getSectionByName(std::string const &str) {
  ELFObjectTy const *const_this = this;
  ELFSectionTy const *sptr = const_this->getSectionByName(str);
  // Const cast for the same API's const and non-const versions.
  return const_cast<ELFSectionTy *>(sptr);
}


template <unsigned Bitwidth>
inline void ELFObject<Bitwidth>::
relocateARM(void *(*find_sym)(void *context, char const *name),
            void *context,
            ELFSectionRelTableTy *reltab,
            ELFSectionProgBitsTy *text) {
  // FIXME: Should be implement in independent files.
  rsl_assert(Bitwidth == 32 && "ARM only have 32 bits.");

  ELFSectionSymTabTy *symtab =
    static_cast<ELFSectionSymTabTy *>(getSectionByName(".symtab"));

  for (size_t i = 0; i < reltab->size(); ++i) {
    // FIXME: Can not implement here, use Fixup!
    ELFRelocTy *rel = (*reltab)[i];
    ELFSymbolTy *sym = (*symtab)[rel->getSymTabIndex()];

    // FIXME: May be not uint32_t *.
    typedef int32_t Inst_t;
    Inst_t *inst = (Inst_t *)&(*text)[rel->getOffset()];
    Inst_t P = (Inst_t)(int64_t)inst;
    Inst_t A = 0;
    Inst_t S = (Inst_t)(int64_t)sym->getAddress();

    switch (rel->getType()) {
    default:
      rsl_assert(0 && "Not implemented relocation type.");
      break;

      // FIXME: Predefine relocation codes.
    case 28: // R_ARM_CALL
      {
#define SIGN_EXTEND(x, l) (((x)^(1<<((l)-1)))-(1<<(l-1)))
        A = (Inst_t)(int64_t)SIGN_EXTEND(*inst & 0xFFFFFF, 24);
#undef SIGN_EXTEND

        switch (sym->getType()) {
        default:
          rsl_assert(0 && "Wrong type for R_ARM_CALL relocation.");
          abort();
          break;

        case STT_FUNC:
          if (S == 0) {
            rsl_assert(0 && "We should get function address at previous "
                   "sym->getAddress() function call.");
            abort();
          }
          break;

        case STT_NOTYPE:
          if (S == 0) {
            void *ext_func = find_sym(context, sym->getName());
            S = (Inst_t)(uintptr_t)ext_func;
            sym->setAddress(ext_func);

            uint32_t result = (S >> 2) - (P >> 2) + A;
            if (result > 0x007fffff && result < 0xff800000) {
#ifndef __arm__
              // We have not implement function stub in this runtime env
              rsl_assert(0 && "Target address is far from call instruction");
              abort();
#else
              void *stub = getStubLayout()->allocateStub(ext_func);
              if (!stub) {
                llvm::errs() << "unable to allocate stub." << "\n";
                exit(EXIT_FAILURE);
              }

              //out() << "stub: for " << ext_func << " at " << stub << "\n";
              sym->setAddress(stub);
              S = (uint32_t)stub;
#endif
            }
          }
          break;
        }

        uint32_t result = (S >> 2) - (P >> 2) + A;

        if (result > 0x007fffff && result < 0xff800000) {
          rsl_assert(0 && "Stub is still too far");
          abort();
        }

        *inst = ((result) & 0x00FFFFFF) | (*inst & 0xFF000000);
      }
      break;
    case 44: // R_ARM_MOVT_ABS
      S >>= 16;
    case 43: // R_ARM_MOVW_ABS_NC
      {
        // No need sign extend.
        A = ((*inst & 0xF0000) >> 4) | (*inst & 0xFFF);
        uint32_t result = (S+A);
        *inst = (((result) & 0xF000) << 4) |
          ((result) & 0xFFF) |
          (*inst & 0xFFF0F000);
      }
      break;
    }
    //llvm::errs() << "S:     " << (void *)S << '\n';
    //llvm::errs() << "A:     " << (void *)A << '\n';
    //llvm::errs() << "P:     " << (void *)P << '\n';
    //llvm::errs() << "S+A:   " << (void *)(S+A) << '\n';
    //llvm::errs() << "S+A-P: " << (void *)(S+A-P) << '\n';
  }
}

template <unsigned Bitwidth>
inline void ELFObject<Bitwidth>::
relocateX86_64(void *(*find_sym)(void *context, char const *name),
               void *context,
               ELFSectionRelTableTy *reltab,
               ELFSectionProgBitsTy *text) {
  rsl_assert(Bitwidth == 64 && "Only support X86_64.");

  ELFSectionSymTabTy *symtab =
    static_cast<ELFSectionSymTabTy *>(getSectionByName(".symtab"));

  for (size_t i = 0; i < reltab->size(); ++i) {
    // FIXME: Can not implement here, use Fixup!
    ELFRelocTy *rel = (*reltab)[i];
    ELFSymbolTy *sym = (*symtab)[rel->getSymTabIndex()];

    //typedef uint64_t Inst_t;
    typedef int32_t Inst_t;
    Inst_t *inst = (Inst_t *)&(*text)[rel->getOffset()];
    Inst_t P = (Inst_t)(int64_t)inst;
    Inst_t A = (Inst_t)(int64_t)rel->getAddend();
    Inst_t S = (Inst_t)(int64_t)sym->getAddress();

    if (S == 0) {
      S = (Inst_t)(int64_t)find_sym(context, sym->getName());
      sym->setAddress((void *)S);
    }

    switch (rel->getType()) {
      default:
        rsl_assert(0 && "Not implemented relocation type.");
        break;

      // FIXME: XXX: R_X86_64_64 is 64 bit, there is a big problem here.
      case 1: // R_X86_64_64
        *inst = (S+A);
        break;

      case 2: // R_X86_64_PC32
        *inst = (S+A-P);
        break;

      case 10: // R_X86_64_32
      case 11: // R_X86_64_32S
        *inst = (S+A);
        break;
    }
  }
}

template <unsigned Bitwidth>
inline void ELFObject<Bitwidth>::
relocateX86_32(void *(*find_sym)(void *context, char const *name),
               void *context,
               ELFSectionRelTableTy *reltab,
               ELFSectionProgBitsTy *text) {
  rsl_assert(Bitwidth == 32 && "Only support X86.");

  ELFSectionSymTabTy *symtab =
    static_cast<ELFSectionSymTabTy *>(getSectionByName(".symtab"));

  for (size_t i = 0; i < reltab->size(); ++i) {
    // FIXME: Can not implement here, use Fixup!
    ELFRelocTy *rel = (*reltab)[i];
    ELFSymbolTy *sym = (*symtab)[rel->getSymTabIndex()];

    //typedef uint64_t Inst_t;
    typedef int32_t Inst_t;
    Inst_t *inst = (Inst_t *)&(*text)[rel->getOffset()];
    Inst_t P = (Inst_t)(uintptr_t)inst;
    Inst_t A = (Inst_t)(uintptr_t)*inst;
    Inst_t S = (Inst_t)(uintptr_t)sym->getAddress();

    if (S == 0) {
      S = (Inst_t)(uintptr_t)find_sym(context, sym->getName());
      sym->setAddress((void *)S);
    }

    switch (rel->getType()) {
    default:
      rsl_assert(0 && "Not implemented relocation type.");
      break;

    case R_386_PC32:
      *inst = (S+A-P);
      break;

    case R_386_32:
      *inst = (S+A);
      break;
    }
  }
}

template <unsigned Bitwidth>
inline void ELFObject<Bitwidth>::
relocate(void *(*find_sym)(void *context, char const *name), void *context) {

  for (size_t i = 0; i < stab.size(); ++i) {
    ELFSectionHeaderTy *sh = (*shtab)[i];
    if (sh->getType() != SHT_REL && sh->getType() != SHT_RELA) {
      continue;
    }
    ELFSectionRelTableTy *reltab =
      static_cast<ELFSectionRelTableTy *>(stab[i]);
    rsl_assert(reltab && "Relocation section can't be NULL.");

    const char *reltab_name = sh->getName();
    const char *need_rel_name;
    if (sh->getType() == SHT_REL) {
      need_rel_name = reltab_name + 4;
      // ".rel.xxxx"
      //      ^ start from here.
    } else {
      need_rel_name = reltab_name + 5;
    }

    ELFSectionProgBitsTy *need_rel =
      static_cast<ELFSectionProgBitsTy *>(getSectionByName(need_rel_name));
    rsl_assert(need_rel && "Need be relocated section can't be NULL.");

    switch (getHeader()->getMachine()) {
      case EM_ARM:
        relocateARM(find_sym, context, reltab, need_rel);
        break;
      case EM_386:
        relocateX86_32(find_sym, context, reltab, need_rel);
        break;
      case EM_X86_64:
        relocateX86_64(find_sym, context, reltab, need_rel);
        break;

      default:
        rsl_assert(0 && "Only support ARM, X86, and X86_64 relocation.");
        break;
    }
  }

  for (size_t i = 0; i < stab.size(); ++i) {
    ELFSectionHeaderTy *sh = (*shtab)[i];
    if (sh->getType() == SHT_PROGBITS || sh->getType() == SHT_NOBITS) {
      if (stab[i]) {
        static_cast<ELFSectionBitsTy *>(stab[i])->protect();
      }
    }
  }
}

template <unsigned Bitwidth>
inline void ELFObject<Bitwidth>::print() const {
  header->print();
  shtab->print();

  for (size_t i = 0; i < stab.size(); ++i) {
    ELFSectionTy *sec = stab[i];
    if (sec) {
      sec->print();
    }
  }
}

#endif // ELF_OBJECT_HXX
