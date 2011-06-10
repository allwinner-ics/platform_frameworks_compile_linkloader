#ifndef ELF_SECTION_REL_TABLE_H
#define ELF_SECTION_REL_TABLE_H

#include "ELFSectionTable_CRTP.h"
#include "ELFRel.h"

#include <string>

template <unsigned Bitwidth> class ELFRel;

template <unsigned Bitwidth>
class ELFSectionRelTable :
  public ELFSectionTable_CRTP<Bitwidth,
                              ELFSectionRelTable<Bitwidth>,
                              ELFRel<Bitwidth> > {
  friend class ELFSectionTable_CRTP<Bitwidth,
                                    ELFSectionRelTable<Bitwidth>,
                                    ELFRel<Bitwidth> >;
private:
  static char const *TABLE_NAME;

public:
  typedef ELFRel<Bitwidth> Rel;

private:
  //std::vector<Rel *> rel_table;

private:
  ELFSectionRelTable() { }

public:
  // Note: Inherit from Table CRTP.

  //template <typename Archiver>
  //static ELFSectionRelTable *
  //read(Archiver &AR,
  //     ELFObject<Bitwidth> *owner,
  //     ELFSectionHeader<Bitwidth> const *sh);

  //virtual void print() const;

  //size_t size() const;

  //Rel const *operator[](size_t index) const;
};


//==================Inline Member Function Definition==========================

template <unsigned Bitwidth>
char const *ELFSectionRelTable<Bitwidth>::TABLE_NAME = "Relocation Table";

#endif // ELF_SECTION_REL_TABLE_H
