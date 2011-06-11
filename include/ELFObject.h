#ifndef ELF_OBJECT_H
#define ELF_OBJECT_H

#include <llvm/ADT/OwningPtr.h>

#include <string>
#include <vector>

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

#include "ELFObject.hxx"

#endif // ELF_OBJECT_H
