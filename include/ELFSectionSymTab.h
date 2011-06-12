#ifndef ELF_SECTION_SYMTAB_H
#define ELF_SECTION_SYMTAB_H

#include "ELFTypes.h"

#include <vector>
#include <string>

template <unsigned Bitwidth>
class ELFSectionSymTab : public ELFSection<Bitwidth> {
public:
  ELF_TYPE_INTRO_TO_TEMPLATE_SCOPE(Bitwidth);

private:
  std::vector<ELFSymbolTy *> table;

private:
  ELFSectionSymTab() { }

public:
  ~ELFSectionSymTab();

  template <typename Archiver>
  static ELFSectionSymTab *
  read(Archiver &AR, ELFObjectTy *owner, ELFSectionHeaderTy const *sh);

  virtual void print() const;

  size_t size() const {
    return table.size();
  }

  ELFSymbolTy const *operator[](size_t index) const {
    return table[index];
  }

  ELFSymbolTy *operator[](size_t index) {
    return table[index];
  }

  ELFSymbolTy const *getByName(std::string const &name) const {
    for (size_t i = 0; i < table.size(); ++i) {
      if (table[i]->getName() == name){
        return table[i];
      }
    }
    return NULL;
  }

  ELFSymbolTy *getByName(std::string const &name) {
    return const_cast<ELFSymbolTy *>(
           const_cast<ELFSectionSymTabTy const *>(this)->getByName(name));
  }

  size_t getExternFuncCount() const;

};

#include "impl/ELFSectionSymTab.hxx"

#endif // ELF_SECTION_SYMTAB_H
