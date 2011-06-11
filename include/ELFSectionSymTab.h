#ifndef ELF_SECTION_SYMTAB_H
#define ELF_SECTION_SYMTAB_H

#include <vector>
#include <string>

template <unsigned Bitwidth> class ELFSymbol;

template <unsigned Bitwidth>
class ELFSectionSymTab : public ELFSection<Bitwidth> {
private:
  std::vector<ELFSymbol<Bitwidth> *> table;

private:
  ELFSectionSymTab() { }

public:
  ~ELFSectionSymTab();

  template <typename Archiver>
  static ELFSectionSymTab *
  read(Archiver &AR,
       ELFObject<Bitwidth> *owner,
       ELFSectionHeader<Bitwidth> const *sh);

  virtual void print() const;

  size_t size() const {
    return table.size();
  }

  ELFSymbol<Bitwidth> const *operator[](size_t index) const {
    return table[index];
  }

  ELFSymbol<Bitwidth> *operator[](size_t index) {
    return table[index];
  }

  ELFSymbol<Bitwidth> const *getByName(std::string const &name) const {
    for (size_t i = 0; i < table.size(); ++i) {
      if (table[i]->getName() == name){
        return table[i];
      }
    }
    return NULL;
  }

  ELFSymbol<Bitwidth> *getByName(std::string const &name) {
    return const_cast<ELFSymbol<Bitwidth> *>(
           const_cast<ELFSectionSymTab<Bitwidth> const *>(this)->getByName(name));
  }

};

#include "impl/ELFSectionSymTab.hxx"

#endif // ELF_SECTION_SYMTAB_H
