#ifndef ELF_OBJECT_H
#define ELF_OBJECT_H

#include <llvm/ADT/OwningPtr.h>

#include <vector>
#include <string>

#include <assert.h>
#include <elf.h>

class StubLayout;

template <unsigned Bitwidth> class ELFHeader;
template <unsigned Bitwidth> class ELFSection;
template <unsigned Bitwidth> class ELFSectionHeaderTable;

template <unsigned Bitwidth>
class ELFObject {
private:
  llvm::OwningPtr<ELFHeader<Bitwidth> > header;
  llvm::OwningPtr<ELFSectionHeaderTable<Bitwidth> > shtab;
  std::vector<ELFSection<Bitwidth> *> stab;

#ifdef __arm__
  llvm::OwningPtr<StubLayout> stubs;
#endif

private:
  ELFObject() { }

public:
  template <typename Archiver>
  static ELFObject *read(Archiver &AR);

  ELFHeader<Bitwidth> const *getHeader() const {
    return header.get();
  }

  ELFSectionHeaderTable<Bitwidth> const *getSectionHeaderTable() const {
    return shtab.get();
  }

  char const *getSectionName(size_t i) const;
  ELFSection<Bitwidth> const *getSectionByIndex(size_t i) const;
  ELFSection<Bitwidth> *getSectionByIndex(size_t i);
  ELFSection<Bitwidth> const *getSectionByName(std::string const &str) const;
  ELFSection<Bitwidth> *getSectionByName(std::string const &str);

#ifdef __arm__
  StubLayout *getStubLayout();
#endif

  void relocate(void *(*find_sym)(char const *name, void *context),
                void *context);

  void print() const;

  ~ELFObject() {
    for (size_t i = 0; i < stab.size(); ++i) {
      // Delete will check the pointer is nullptr or not by himself.
      delete stab[i];
    }
  }

private:
  void relocateARM(void *(*find_sym)(char const *name, void *context),
                   void *context);

  void relocateX86_32(void *(*find_sym)(char const *name, void *context),
                      void *context);

  void relocateX86_64(void *(*find_sym)(char const *name, void *context),
                      void *context);

};

//==================Inline Member Function Definition==========================

#include "ELFHeader.h"
#include "ELFSection.h"
#include "ELFSectionHeaderTable.h"
#include "StubLayout.h"

#ifdef __arm__
template <unsigned Bitwidth>
inline StubLayout *ELFObject<Bitwidth>::getStubLayout() {
  if (!stubs) {
    stubs.reset(new StubLayout());
  }
  return stubs.get();
}
#endif

template <unsigned Bitwidth>
template <typename Archiver>
inline ELFObject<Bitwidth> *
ELFObject<Bitwidth>::read(Archiver &AR) {
  llvm::OwningPtr<ELFObject<Bitwidth> > object(new ELFObject<Bitwidth>());

  // Read header
  object->header.reset(ELFHeader<Bitwidth>::read(AR));
  if (!object->header) {
    return 0;
  }

  // Read section table
  object->shtab.reset(ELFSectionHeaderTable<Bitwidth>::read(AR, object.get()));
  if (!object->shtab) {
    return 0;
  }

  // Read each section
  for (size_t i = 0; i < object->header->getSectionHeaderNum(); ++i) {
    llvm::OwningPtr<ELFSection<Bitwidth> > sec(
      ELFSection<Bitwidth>::read(AR, object.get(), (*object->shtab)[i]));
    object->stab.push_back(sec.take());
  }

  return object.take();
}

template <unsigned Bitwidth>
inline char const *ELFObject<Bitwidth>::getSectionName(size_t i) const {
  ELFSection<Bitwidth> const *sec =
    stab[header->getStringSectionIndex()];

  if (sec) {
    ELFSectionStrTab<Bitwidth> const &st =
      static_cast<ELFSectionStrTab<Bitwidth> const &>(*sec);
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
  ELFObject<Bitwidth> const *const_this = this;
  ELFSection<Bitwidth> const *sptr = const_this->getSectionByName(str);
  // Const cast for the same API's const and non-const versions.
  return const_cast<ELFSection<Bitwidth> *>(sptr);
}


template <unsigned Bitwidth>
inline void ELFObject<Bitwidth>::
relocateARM(void *(*find_sym)(char const *name, void *context),
            void *context) {
  // FIXME: Should be implement in independent files.
  assert(Bitwidth == 32 && "ARM only have 32 bits.");

  // FIXME: Can not only relocate .rel.text!
  ELFSectionRelTable<Bitwidth> *reltab =
    static_cast<ELFSectionRelTable<Bitwidth> *>(getSectionByName(".rel.text"));
  ELFSectionProgBits<Bitwidth> *text =
    static_cast<ELFSectionProgBits<Bitwidth> *>(getSectionByName(".text"));
  ELFSectionSymTab<Bitwidth> *symtab =
    static_cast<ELFSectionSymTab<Bitwidth> *>(getSectionByName(".symtab"));

  for (size_t i = 0; i < reltab->size(); ++i) {
    // FIXME: Can not implement here, use Fixup!
    ELFRel<Bitwidth> *rel = (*reltab)[i];
    ELFSymbol<Bitwidth> *sym = (*symtab)[rel->getSymTabIndex()];

    // FIXME: May be not uint32_t *.
    typedef int32_t Inst_t;
    Inst_t *inst = (Inst_t *)&(*text)[rel->getOffset()];
    Inst_t P = (Inst_t)(int64_t)inst;
    Inst_t A = 0;
    Inst_t S = (Inst_t)(int64_t)sym->getAddress();

    switch ((uint32_t)rel->getType()) {
    default:
      assert(0 && "Not implemented relocation type.");
      break;

      // FIXME: Predefine relocation codes.
    case 28: // R_ARM_CALL
      {
#define SIGN_EXTEND(x, l) (((x)^(1<<((l)-1)))-(1<<(l-1)))
        A = (Inst_t)(int64_t)SIGN_EXTEND(*inst & 0xFFFFFF, 24);
#undef SIGN_EXTEND
        if (S == 0) {
          S = (Inst_t)(int64_t)find_sym(sym->getName(), context);
          sym->setAddress((void *)S);
        }
        //switch (sym->getType()) {
        //  default:
        //    assert(0 && "Wrong type for R_ARM_CALL relocation.");
        //    break;
        //  case STT_FUNC:
        //    {
        //      S = (uint32_t)sym->getAddress();
        //    }
        //    break;
        //  case STT_NOTYPE:
        //    {
        //      if (sym->getAddress() == 0) {
        //        sym->setAddress(find_sym(sym->getName(), context));
        //      }
        //      S = (uint32_t)sym->getAddress();
        //    }
        //    break;
        //}
        S >>= 2;
        P >>= 2;
        uint32_t result = (S+A-P);
#ifdef __arm__
        if ((result & 0xFF000000) != 0) {
          out() << "far stub for: " << sym->getAddress() << " ";
          void *stub = getStubLayout()->allocateStub((void *)sym->getAddress());
          if (!stub) {
            out() << "unable to allocate stub." << "\n";
            exit(EXIT_FAILURE);
          }
          out() << "is at " << stub << "\n";
          S = ((uint32_t)stub) >> 2;
          result = (S+A-P);
        }
#else
        // TODO: Stub.
        assert(((result & 0xFF000000) == 0) && "Too far, need stub.");
#endif
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
relocateX86_64(void *(*find_sym)(char const *name, void *context),
               void *context) {
  assert(Bitwidth == 64 && "Only support X86_64.");

  ELFSectionSymTab<Bitwidth> *symtab =
    static_cast<ELFSectionSymTab<Bitwidth> *>(getSectionByName(".symtab"));

  char const *name[] = {".text", ".eh_frame"};
  size_t size = sizeof(name) / sizeof(char const *);

  for (size_t i = 0; i < size; ++i) {
    ELFSectionRelaTable<Bitwidth> *relatab =
      static_cast<ELFSectionRelaTable<Bitwidth> *>(
          getSectionByName((std::string(".rela") + name[i]).c_str()));
    ELFSectionProgBits<Bitwidth> *text =
      static_cast<ELFSectionProgBits<Bitwidth> *>(getSectionByName(name[i]));

    for (size_t i = 0; i < relatab->size(); ++i) {
      // FIXME: Can not implement here, use Fixup!
      ELFRela<Bitwidth> *rela = (*relatab)[i];
      ELFSymbol<Bitwidth> *sym = (*symtab)[rela->getSymTabIndex()];

      //typedef uint64_t Inst_t;
      typedef int32_t Inst_t;
      Inst_t *inst = (Inst_t *)&(*text)[rela->getOffset()];
      Inst_t P = (Inst_t)(int64_t)inst;
      Inst_t A = (Inst_t)(int64_t)rela->getAddend();
      Inst_t S = (Inst_t)(int64_t)sym->getAddress();

      if (S == 0) {
        S = (Inst_t)(int64_t)find_sym(sym->getName(), context);
        sym->setAddress((void *)S);
      }

      switch ((uint32_t)rela->getType()) {
        default:
          assert(0 && "Not implemented relocation type.");
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
}

template <unsigned Bitwidth>
inline void ELFObject<Bitwidth>::
relocateX86_32(void *(*find_sym)(char const *name, void *context),
               void *context) {
  assert(Bitwidth == 32 && "Only support X86.");

  ELFSectionSymTab<Bitwidth> *symtab =
    static_cast<ELFSectionSymTab<Bitwidth> *>(
      getSectionByName(".symtab"));

  char const *name[] = {".text"};
  size_t const size = sizeof(name) / sizeof(char const *);

  for (size_t i = 0; i < size; ++i) {
    ELFSectionRelTable<Bitwidth> *reltab =
      static_cast<ELFSectionRelTable<Bitwidth> *>(
        getSectionByName((std::string(".rel") + name[i]).c_str()));

    ELFSectionProgBits<Bitwidth> *text =
      static_cast<ELFSectionProgBits<Bitwidth> *>(
        getSectionByName(name[i]));

    for (size_t i = 0; i < reltab->size(); ++i) {
      // FIXME: Can not implement here, use Fixup!
      ELFRel<Bitwidth> *rel = (*reltab)[i];
      ELFSymbol<Bitwidth> *sym = (*symtab)[rel->getSymTabIndex()];

      //typedef uint64_t Inst_t;
      typedef int32_t Inst_t;
      Inst_t *inst = (Inst_t *)&(*text)[rel->getOffset()];
      Inst_t P = (Inst_t)inst;
      Inst_t A = (Inst_t)*inst;
      Inst_t S = (Inst_t)sym->getAddress();

      if (S == 0) {
        S = (Inst_t)find_sym(sym->getName(), context);
        sym->setAddress((void *)S);
      }

      switch ((uint32_t)rel->getType()) {
      default:
        assert(0 && "Not implemented relocation type.");
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
}

template <unsigned Bitwidth>
inline void ELFObject<Bitwidth>::
relocate(void *(*find_sym)(char const *name, void *context), void *context) {
  switch ((uint32_t)getHeader()->getMachine()) {
    case EM_ARM:    relocateARM(find_sym, context); break;
    case EM_386:    relocateX86_32(find_sym, context); break;
    case EM_X86_64: relocateX86_64(find_sym, context); break;

    default:
      assert(0 && "Only support ARM and X86_64 relocation.");
      break;
  }

  for (size_t i = 0; i < stab.size(); ++i) {
    ELFSectionHeader<Bitwidth> *sh = (*shtab)[i];
    if (sh && (sh->getType() == SHT_PROGBITS ||
               sh->getType() == SHT_NOBITS)) {
      static_cast<ELFSectionBits<Bitwidth> *>(stab[i])->memory_protect();
    }
  }
}

template <unsigned Bitwidth>
inline void ELFObject<Bitwidth>::print() const {
  header->print();
  shtab->print();

  for (size_t i = 0; i < stab.size(); ++i) {
    ELFSection<Bitwidth> *sec = stab[i];
    if (sec) {
      sec->print();
    }
  }
}

#endif // ELF_OBJECT_H
