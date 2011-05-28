#ifndef ELF_SECTION_REL_TABLE_H
#define ELF_SECTION_REL_TABLE_H

#include "ELFSectionTable_CRTP.h"
#include "ELFSectionRel.h"

#include <string>

template <size_t Bitwidth> class ELFSectionRel;

template <size_t Bitwidth>
class ELFSectionRelTable :
  public ELFSectionTable_CRTP<Bitwidth,
                              ELFSectionRelTable<Bitwidth>,
                              ELFSectionRel<Bitwidth> > {
  friend class ELFSectionTable_CRTP<Bitwidth,
                                    ELFSectionRelTable<Bitwidth>,
                                    ELFSectionRel<Bitwidth> >;
private:
  static char const *TABLE_NAME;

public:
  typedef ELFSectionRel<Bitwidth> Rel;

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

template <size_t Bitwidth>
char const *ELFSectionRelTable<Bitwidth>::TABLE_NAME = "Relocation Table";

#endif // ELF_SECTION_REL_TABLE_H
