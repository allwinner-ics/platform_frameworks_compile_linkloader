#ifndef ELF_OBJECT_H
#define ELF_OBJECT_H

#include "StubLayout.h"

#include <llvm/ADT/OwningPtr.h>

#include <vector>
#include <elf.h>
#include <cassert>
#include <string>

template <size_t Bitwidth> class ELFHeader;
template <size_t Bitwidth> class ELFSection;
template <size_t Bitwidth> class ELFSectionHeaderTable;

template <size_t Bitwidth>
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
  StubLayout *getStubLayout() {
    if (!stubs) {
      stubs.reset(new StubLayout());
    }
    return stubs.get();
  }
#endif

  void relocate(void *(find_sym)(char const *name, void *context),
                void *context);

  void print() const;

  ~ELFObject() {
    for (size_t i = 0; i < stab.size(); ++i) {
      // Delete will check the pointer is nullptr or not by himself.
      delete stab[i];
    }
  }
};

//==================Inline Member Function Definition==========================


#include "ELFHeader.h"
#include "ELFSection.h"
#include "ELFSectionHeaderTable.h"

template <size_t Bitwidth>
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

template <size_t Bitwidth>
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

template <size_t Bitwidth>
inline ELFSection<Bitwidth> const *
ELFObject<Bitwidth>::getSectionByIndex(size_t i) const {
  return stab[i];
}

template <size_t Bitwidth>
inline ELFSection<Bitwidth> *
ELFObject<Bitwidth>::getSectionByIndex(size_t i) {
  return stab[i];
}

template <size_t Bitwidth>
inline ELFSection<Bitwidth> const *
ELFObject<Bitwidth>::getSectionByName(std::string const &str) const {
  size_t idx = getSectionHeaderTable()->getByName(str)->getIndex();
  return stab[idx];
}

template <size_t Bitwidth>
inline ELFSection<Bitwidth> *
ELFObject<Bitwidth>::getSectionByName(std::string const &str) {
  ELFObject<Bitwidth> const *const_this = this;
  ELFSection<Bitwidth> const *sptr = const_this->getSectionByName(str);
  // Const cast for the same API's const and non-const versions.
  return const_cast<ELFSection<Bitwidth> *>(sptr);
}


template <size_t Bitwidth>
inline void ELFObject<Bitwidth>::
relocate(void *(find_sym)(char const *name, void *context), void *context) {
  // FIXME: Can not implement here!
  switch ((uint32_t)getHeader()->getMachine()) {
    default:
      assert(0 && "Only support ARM ans X86_64 relocation.");
      break;

    case EM_ARM:
      // FIXME: Can not implement here!
      {
        assert(Bitwidth == 32 && "ARM only have 32 bits.");
        // FIXME: Can not only relocate .rel.text!
        ELFSectionRelTable<Bitwidth> *reltab =
          static_cast<ELFSectionRelTable<Bitwidth> *>(
              getSectionByName(".rel.text"));
        ELFSectionProgBits<Bitwidth> *text =
          static_cast<ELFSectionProgBits<Bitwidth> *>(
              getSectionByName(".text"));
        ELFSectionSymTab<Bitwidth> *symtab =
          static_cast<ELFSectionSymTab<Bitwidth> *>(
              getSectionByName(".symtab"));
        for (size_t i = 0; i < reltab->size(); ++i) {
          // FIXME: Can not implement here, use Fixup!
          ELFSectionRel<Bitwidth> *rel = (*reltab)[i];
          ELFSectionSymTabEntry<Bitwidth> *sym =
            (*symtab)[rel->getSymTabIndex()];
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
      break;

    case EM_X86_64:
      {
        assert(Bitwidth == 64 && "Only support X86_64.");
        ELFSectionSymTab<Bitwidth> *symtab =
          static_cast<ELFSectionSymTab<Bitwidth> *>(
              getSectionByName(".symtab"));
        size_t const size = 2;
        char const *name[size] = {".text", ".eh_frame"};
        for (size_t i = 0; i < size; ++i) {
          ELFSectionRelaTable<Bitwidth> *relatab =
            static_cast<ELFSectionRelaTable<Bitwidth> *>(
                getSectionByName((std::string(".rela") + name[i]).c_str()));
          ELFSectionProgBits<Bitwidth> *text =
            static_cast<ELFSectionProgBits<Bitwidth> *>(
                getSectionByName(name[i]));
          for (size_t i = 0; i < relatab->size(); ++i) {
            // FIXME: Can not implement here, use Fixup!
            ELFSectionRela<Bitwidth> *rela = (*relatab)[i];
            ELFSectionSymTabEntry<Bitwidth> *sym =
              (*symtab)[rela->getSymTabIndex()];
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

template <size_t Bitwidth>
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
