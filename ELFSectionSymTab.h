#ifndef ELF_SECTION_SYMTAB_H
#define ELF_SECTION_SYMTAB_H

#include "ELFSectionTable_CRTP.h"

#include <vector>
#include <string>

template <size_t Bitwidth> class ELFSectionSymTabEntry;

template <size_t Bitwidth>
class ELFSectionSymTab :
  public ELFSectionTable_CRTP<Bitwidth,
                              ELFSectionSymTab<Bitwidth>,
                              ELFSectionSymTabEntry<Bitwidth> > {
  friend class ELFSectionTable_CRTP<Bitwidth,
                                    ELFSectionSymTab<Bitwidth>,
                                    ELFSectionSymTabEntry<Bitwidth> >;
private:
  static std::string const TABLE_NAME;
public:
  typedef ELFSectionSymTabEntry<Bitwidth> SymTabEntry;

private:
  //std::vector<boost::shared_ptr<SymTabEntry> > symbol_table;

private:
  ELFSectionSymTab() { }

public:
  // Note: Inherit from Table CRTP.

  //template <typename Archiver>
  //static boost::shared_ptr<ELFSectionSymTab>
  //read(Archiver &AR,
  //     ELFObject<Bitwidth> *owner,
  //     ELFSectionHeader<Bitwidth> const *sh);

  //virtual void print() const;

  //size_t size() const;

  //SymTabEntry const *operator[](size_t index) const;

  SymTabEntry const *operator[](const std::string &str) const;
};


//==================Inline Member Function Definition==========================

#include "ELFSectionSymTabEntry.h"

template <size_t Bitwidth>
std::string const ELFSectionSymTab<Bitwidth>::TABLE_NAME("Symbol Table");

template <size_t Bitwidth>
inline ELFSectionSymTabEntry<Bitwidth> const *
ELFSectionSymTab<Bitwidth>::operator[](const std::string &str) const {
  // TODO: Use map
  for (size_t i = 0; i < this->table.size(); ++i) {
    if (str == string(this->table[i]->getName())) {
      return this->table[i].get();
    }
  }
  // Return STN_UNDEF entry.
  return this->table[0].get();
}

#endif // ELF_SECTION_SYMTAB_H
