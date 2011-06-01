#ifndef ELF_SECTION_SYMTAB_H
#define ELF_SECTION_SYMTAB_H

#include "ELFSectionTable_CRTP.h"

#include <vector>
#include <string>

template <unsigned Bitwidth> class ELFSymbol;

template <unsigned Bitwidth>
class ELFSectionSymTab :
  public ELFSectionTable_CRTP<Bitwidth,
                              ELFSectionSymTab<Bitwidth>,
                              ELFSymbol<Bitwidth> > {
  friend class ELFSectionTable_CRTP<Bitwidth,
                                    ELFSectionSymTab<Bitwidth>,
                                    ELFSymbol<Bitwidth> >;
private:
  static char const *TABLE_NAME;
public:
  typedef ELFSymbol<Bitwidth> SymTabEntry;

private:
  //std::vector<SymTabEntry> *> symbol_table;

private:
  ELFSectionSymTab() { }

public:
  // Note: Inherit from Table CRTP.

  //template <typename Archiver>
  //static ELFSectionSymTab *
  //read(Archiver &AR,
  //     ELFObject<Bitwidth> *owner,
  //     ELFSectionHeader<Bitwidth> const *sh);

  //virtual void print() const;

  //size_t size() const;

  //SymTabEntry const *operator[](size_t index) const;

  SymTabEntry const *getByName(const std::string &str) const;
};


//==================Inline Member Function Definition==========================

#include "ELFSymbol.h"

template <unsigned Bitwidth>
char const *ELFSectionSymTab<Bitwidth>::TABLE_NAME = "Symbol Table";

template <unsigned Bitwidth>
inline ELFSymbol<Bitwidth> const *
ELFSectionSymTab<Bitwidth>::getByName(const std::string &str) const {
  // TODO: Use map
  for (size_t i = 0; i < this->table.size(); ++i) {
    if (str == std::string(this->table[i]->getName())) {
      return this->table[i];
    }
  }
  // Return STN_UNDEF entry.
  return this->table[0];
}

#endif // ELF_SECTION_SYMTAB_H
