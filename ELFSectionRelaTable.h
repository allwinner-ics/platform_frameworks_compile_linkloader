#ifndef ELF_SECTION_RELA_TABLE_H
#define ELF_SECTION_RELA_TABLE_H

#include "ELFSectionTable_CRTP.h"
#include "ELFSectionRela.h"

#include <string>

template <size_t Bitwidth> class ELFSectionRela;

template <size_t Bitwidth>
class ELFSectionRelaTable :
  public ELFSectionTable_CRTP<Bitwidth,
                              ELFSectionRelaTable<Bitwidth>,
                              ELFSectionRela<Bitwidth> > {
  friend class ELFSectionTable_CRTP<Bitwidth,
                                    ELFSectionRelaTable<Bitwidth>,
                                    ELFSectionRela<Bitwidth> >;
private:
  static char const *TABLE_NAME;

public:
  typedef ELFSectionRela<Bitwidth> Rela;

private:
  //std::vector<Rela *> rel_table;

private:
  ELFSectionRelaTable() { }

public:
  // Note: Inherit from Table CRTP.

  //template <typename Archiver>
  //static ELFSectionRelaTable *
  //read(Archiver &AR,
  //     ELFObject<Bitwidth> *owner,
  //     ELFSectionHeader<Bitwidth> const *sh);

  //virtual void print() const;

  //size_t size() const;

  //Rela const *operator[](size_t index) const;
};


//==================Inline Member Function Definition==========================

template <size_t Bitwidth>
char const *ELFSectionRelaTable<Bitwidth>::
  TABLE_NAME = "Relocation Table(with Addend)";

#endif // ELF_SECTION_RELA_TABLE_H
